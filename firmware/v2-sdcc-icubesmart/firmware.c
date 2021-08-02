// Copyright(C) Tomas Uktveris 2015
// www.wzona.info
/*
This firmware.c file was created for a custom-made PCB that uses the STC12C5A60S2 to control an 8x8x8 monochrome LED Matrix.
This is a derivative of the original firmware/v2-sdcc/firmware.c compiled by Tomazas.
Credit goes to EdKeyes (from the Amulius - Embedded Engineering Discord Server) for helping me figure out how to correct frames being skipped.
*/
#include <mcs51/stc12.h>

#define uchar unsigned char
#define uint unsigned int

volatile uchar layer_z = 0;   // Z-layer being re-painted

__xdata volatile uchar matrix_display[8][8]; // 8x8x8 = (Y,Z), each subset contains a byte to draw X (from 0x00 - 0xFF).

#define MAX_BUFFER  128     // UART ring buffer size
//#define TX_ENABLED        // uncomment to enable uart TX function

__xdata volatile uchar rx_buffer[MAX_BUFFER];
volatile int rx_read = 0;
volatile int rx_write = 0;
volatile int rx_in = 0;

#ifdef TX_ENABLED
    volatile uchar tx_buffer[MAX_BUFFER];
    volatile int tx_read = 0;
    volatile int tx_write = 0;
    volatile int tx_out = 0;
#endif

// interrupt driven uart with ring buffer
void uart_isr() __interrupt (4)
{
    EA = 0;
    if (RI) // received a byte
    {
        RI = 0; // Clear receive interrupt flag

        if (!(rx_write == rx_read && rx_in > 0)) {
            rx_buffer[rx_write] = SBUF;
            rx_write = (rx_write + 1) % MAX_BUFFER;
            rx_in++;
        }
    }
#ifdef TX_ENABLED
    else if (TI) // byte was sent
    {
        TI = 0; // Clear transmit interrupt flag

        if (tx_out > 0) {
            SBUF = tx_buffer[tx_read];
            tx_read = (tx_read+1)%MAX_BUFFER;
            tx_out--;
        }
    }
#endif

    EA = 1;
}

#ifdef TX_ENABLED
// send a byte via uart (returns -1 if TX buffer full, otherwise 0 on success) [non blocking]
int send_uart(uchar dat)
{
    int res;
    EA = 0;

    if (tx_read == tx_write && tx_out > 0) {
        // buffer is full
        res = -1;
    }
    else {
        tx_buffer[tx_write] = dat;
        tx_write = (tx_write+1)%MAX_BUFFER;
        tx_out++;
        res = 0;

        if (TI == 0) {
            TI = 1; // instruct to run interrupt & send the data
        }
    }

    EA = 1;
    return res;
}

//  send a string via uart [is blocking]
void send_str(char* s)
{
    while (*s)
    {
        while (send_uart(*s++) != 0)
        {
            __asm__("nop");
        }
    }
}

// send a byte via uart [is blocking]
void send_serial(uchar dat)
{
    while(send_uart(dat) != 0)
    {
        __asm__("nop");
    }
}
#endif

// check if a byte is available in uart receive buffer
// returns -1 if not, otherwise - the byte value [non blocking]
int recv_uart()
{
    int value;
    EA = 0;

    if (rx_in == 0)
    {
        value = -1;
    }
    else
    {
        value = rx_buffer[rx_read];
        rx_read = (rx_read + 1) % MAX_BUFFER;
        rx_in--;
    }

    EA = 1;
    return value;
}

// Blocks until a byte is received from UART; returns the byte
uchar read_serial()
{
    int value;
    while ((value = recv_uart()) == -1)
    {
        __asm__("nop");
    }
    return (uchar)(value & 0xFF);
}

// Some magic wait - as in original code:
void delay5us(void)
{
    unsigned char a, b;
    for (b = 7; b > 0; b--)
        for (a = 2; a > 0; a--);
}

void delay(uint i)
{
    while (i--)
    {
        delay5us();
    }
}

// Clear the screen (removes artifacts/ghosting):
void clear_matrix()
{
  uchar i;
  for (i = 0; i < 8; i++) {
    P2 = 1<<i;
    P0 = 255;
  }
}

// Light up a specific point on the cube (x, y, z):
void point_matrix(uchar x, uchar y, uchar z)
{
    matrix_display[y][z] = 1<<x;
}

__bit refresh_screen()
{
  if (rx_in > 0) return 1; // RX command detected

  /*
    P0 range: 255 = all off; 0 = all on; 254 = rightmost on; 127 = leftmost on (Y)
    P1 range: 255 = all off; 0 = all on; 254 = bottommost on; 127 = topmost on (Z)
    P2 range: 1 = frontmost on; 128 = backmost on (X)
  */

  for (uchar y = 0; y < 8; y++) {

      // P2 controls the X-Axis:
      P2 = matrix_display[y][layer_z];

      // Longer the delay = brighter the LEDs shine:
      delay(3);

      // P1 controls the Z-Axis:
      P1 = 255 - (1<<layer_z);

      delay(3);

      // P0 controls the Y-Axis:
      // P0 must be set after P2 and P1 in order for the desired lighting to take effect.
      if (y == 0) {
        P0 = 127;
      } else if (y == 1) {
        P0 = 191;
      } else if (y == 2) {
        P0 = 223;
      } else if (y == 3) {
        P0 = 239;
      } else if (y == 4) {
        P0 = 247;
      } else if (y == 5) {
        P0 = 251;
      } else if (y == 6) {
        P0 = 253;
      } else if (y == 7) {
        P0 = 254;
      }

      delay(3);

      // Remove any ghosting:
      clear_matrix();
  }

  layer_z = (layer_z + 1) % 8; // Rewind and draw for each vertical layer.

  return 0;
}

void main()
{
    int value;

    __bit uart_detected = 0;
    __bit frame_started = 0;

    __bit paint_last_frame = 0; // Controls whether or not to paint the last frame (this should happen once every refresh).

    int received = 0;

    // init uart - 19200bps@24.000MHz MCU
    PCON &= 0x7F;       //Baudrate no doubled
    SCON = 0x50;        //8bit and variable baudrate, 1 stop __bit, no parity
    AUXR |= 0x04;       //BRT's clock is Fosc (1T)
    BRT = 0xD9;         //Set BRT's reload value
    AUXR |= 0x01;       //Use BRT as baudrate generator
    AUXR |= 0x10;       //BRT running

    ES = 1;  // enable UART interrupt

    // setup timer0
    TH0 = 0xc0;     // reload value
    TL0 = 0;
    EA = 1;  // enable global interrupts

    while(1)
    {
      if (uart_detected) // is the cube is being controlled via uart?
        {
          value = read_serial(); // blocks until a byte comes
          if (frame_started == 0)
          {
              if (value == 0xF2 && paint_last_frame == 0) // start receiving batch, but only once the last frame was painted
              {
                  frame_started = 1; // begin receiving frame data
                  received = 0;      // no rows received
              } else {
                uart_detected = 0;
                if (paint_last_frame == 1) {
                  matrix_display[7][7] = value; // Paint the last frame (last byte gets carried over for some reason)
                  paint_last_frame = 0;
                }
              }
          } else
          {
            if (received <= 62) {
              matrix_display[received / 8][received % 8] = value;
            }
            if (received == 62) {
              paint_last_frame = 1;
              frame_started = 0;
            }
            received++;
          }
        } else {
          uart_detected = refresh_screen();
        }
    }
}
