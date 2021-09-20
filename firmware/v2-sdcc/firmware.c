// Copyright(C) Tomas Uktveris 2015
// www.wzona.info

#include <mcs51/stc12.h>

#define uchar unsigned char
#define uint unsigned int
__xdata volatile uchar display[2][8][8]; // 8x8x8 = (Z,Y,X)
volatile uchar frame = 0;   // current visible frame (frontbuffer) index
volatile uchar temp =  1;   // not visible frame (backbuffer) index
volatile uchar layer = 0;   // layer, that is being re-painted

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

///////////////////////////////////////////////////////////
// interrupt driven uart with ring buffer
void uart_isr() __interrupt (4)
{
    EA = 0;

    if (RI) // received a byte
    {
        RI = 0; // Clear receive interrupt flag

        if (!(rx_write == rx_read && rx_in > 0)) {
            rx_buffer[rx_write] = SBUF;
            rx_write = (rx_write+1)%MAX_BUFFER;
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

///////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////
// send a byte via uart [is blocking]
void send_serial(uchar dat)
{
    while(send_uart(dat) != 0)
    {
        __asm__("nop");
    }
}
#endif

///////////////////////////////////////////////////////////
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
        rx_read = (rx_read+1)%MAX_BUFFER;
        rx_in--;
    }

    EA = 1;
    return value;
}

///////////////////////////////////////////////////////////
// blocks until a byte is received from uart, returns the byte
uchar read_serial()
{
    int value;
    while ((value = recv_uart()) == -1)
    {
        __asm__("nop");
    }
    return (uchar)(value & 0xFF);
}

///////////////////////////////////////////////////////////

void delay5us(void) // some magic wait - as in original code
{
    unsigned char a,b;
    for(b=7; b>0; b--)
        for(a=2; a>0; a--);
}

///////////////////////////////////////////////////////////

void delay(uint i)
{
    while (i--)
    {
        delay5us();
    }
}

///////////////////////////////////////////////////////////
// assign all cube registers/rows the same value, usually 0, idx - 0/1 for front/back buffer
void clear(char idx, char val)
{
    uchar i,j;
    for (j = 0; j < 8; ++j) {
        for (i=0; i<8; ++i) {
            display[idx][j][i] = val;
        }
    }
}

///////////////////////////////////////////////////////////

// light a specific point on the cube (x,y,z), enable = on/off
void point(uchar x, uchar y, uchar z, uchar enable)
{
    uchar ch1 = 1 << x;
    if (enable) {
        display[frame][z][y] = display[frame][z][y] | ch1;
    }
    else {
        display[frame][z][y] = display[frame][z][y] & (~ch1);
    }
}

///////////////////////////////////////////////////////////
// sets one row of a layer to the specified value,
// i.e. value = 0 (all 8 leds off), value = 0xFF (all 8 leds on), etc.
void line(uchar y, uchar z, uchar value)
{
    display[frame][z][y] = value;
}

///////////////////////////////////////////////////////////
// swap back buffer with front buffer (i.e. show contents of back buffer)
void swap()
{
    if (frame) {
        frame = 0;
        temp = 1;
    }
    else {
        frame = 1;
        temp = 0;
    }

    clear(temp, 0); // start painting on new clean backbuffer
}

///////////////////////////////////////////////////////////

__code uchar dat[128]= { /*railway*/
    0x0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xdc,0xbc,0x9c,0x7c,0x5c,0x3c,
    0x1c,0x18,0x14,0x10,0xc,0x8,0x4,0x25,0x45,0x65,0x85,0xa5,0xc5,0xc9,0xcd,0xd1,0xd5,0xd9,0xb9,0x99,0x79,0x59,0x39,0x35,0x31,
    0x2d,0x29,0x4a,0x6a,0x8a,0xaa,0xae,0xb2,0xb6,0x96,0x76,0x56,0x52,0x4e,0x6f,0x8f,0x93,0x73,0x6f,0x8f,0x93,0x73,0x4a,0x6a,
    0x8a,0xaa,0xae,0xb2,0xb6,0x96,0x76,0x56,0x52,0x4e,0x25,0x45,0x65,0x85,0xa5,0xc5,0xc9,0xcd,0xd1,0xd5,0xd9,0xb9,0x99,0x79,
    0x59,0x39,0x35,0x31,0x2d,0x29,0x0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xdc,0xbc,0x9c,
    0x7c,0x5c,0x3c,0x1c,0x18,0x14,0x10,0xc,0x8,0x4
};

/*
    cpp - distance from the midpoint
    le - draw or clean.
*/

void cirp(char cpp, uchar dir, uchar le)
{
    uchar a, b, c, cp;
    if ((cpp < 128) & (cpp >= 0)) {
        if (dir) {
            cp = 127 - cpp;
        }
        else {
            cp = cpp;
        }

        a = (dat[cp] >> 5) & 0x07;
        b = (dat[cp] >> 2) & 0x07;
        c = dat[cp] & 0x03;
        if (cpp > 63) {
            c=7-c;
        }
        point(a,b,c,le);
    }
}

///////////////////////////////////////////////////////////
// default animation included in with the ledcube with some modifications
__bit flash_2()
{
    uchar i;
    for (i=129; i>0; i--)
    {
        if (rx_in > 0) return 1; // RX command detected
        cirp(i-2,0,1);
        delay(8000);
        cirp(i-1,0,0);
    }

    delay(8000);

    for (i=0; i<136; i++)
    {
        if (rx_in > 0) return 1; // RX command detected
        cirp(i,1,1);
        delay(8000);
        cirp(i-8,1,0);
    }

    delay(8000);

    for (i=129; i>0; i--)
    {
        if (rx_in > 0) return 1; // RX command detected
        cirp(i-2,0,1);
        delay(8000);
    }

    delay(8000);

    for (i=0; i<128; i++)
    {
        if (rx_in > 0) return 1; // RX command detected
        cirp(i-8,1,0);
        delay(8000);
    }

    delay(60000);
    return 0;
}

///////////////////////////////////////////////////////////

void main()
{
    int value;

    __bit uart_detected = 0;
    __bit frame_started = 0;

    uchar received = 0;

    // init uart - 9600bps@12.000MHz MCU
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
    TR0 = 1;        // timer0 start

    ET0 = 1; // enable timer0 interrupt
    EA = 1;  // enable global interrupts

    // clear main buffer and back buffer
    clear(frame, 0);
    clear(temp, 0);

    while(1)
    {
        if (uart_detected) // is the cube is being controlled via uart?
        {
            value = read_serial(); // blocks until a byte comes

            if (!frame_started)
            {
                if (value == 0xF2) // start receiving batch
                {
                    frame_started = 1; // begin receiving frame data
                    received = 0;        // no rows received
                }
            }
            else
            {
                if (received < 64) // full cube data still not processed
                {
                    display[temp][received/8][received%8] = value;
                    received++; // one more row/byte received
                }

                if (received >= 64) // full cube info received
                {
                    swap();                      // show leds lights
                    frame_started = 0; // need new frame data
                }
            }
        }
        else
        {
            // run default animation if no UART commands
            // if detected - switch working mode
            uart_detected = flash_2();
        }
    }
}

///////////////////////////////////////////////////////////

//P0;  //573 in
//P1;  //uln2803
//P2;  //573 LE

void print() __interrupt (1) // timer0 interrupt
{
    uchar y;
    P1 = 0;

    // update one layer at a time
    for (y=0; y<8; y++)
    {
        P2 = 1<<y;
        delay(3);
        P0 = display[frame][layer][y]; // shift every layer byte
        delay(3);
    }

    P1 = 1<<layer;
    layer = (layer+1)%8; // rewind - ensure we loop in 0-7 layers

    // reset timer0
    TH0 = 0xc0;
    TL0 = 0;
}
