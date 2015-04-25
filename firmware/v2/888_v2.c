#include <STC12C5A60S2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

volatile uchar display[2][8][8]; // 8x8x8 = (Z,Y,X)
volatile uchar frame = 0;
volatile uchar temp = 1;
volatile uchar layer = 0;

#define MAX_BUFFER  128					// UART ring buffer size
//#define TX_ENABLED

volatile bit busy;
volatile uchar rx_buffer[MAX_BUFFER];
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

void uart_isr() interrupt 4
{
		EA = 0;
	
    if (RI)	// received a byte
    {
        RI = 0;             //Clear receive interrupt flag
			
				if (!(rx_write == rx_read && rx_in > 0)) 
				{
					rx_buffer[rx_write] = SBUF;
					rx_write = (rx_write+1)%MAX_BUFFER;
					rx_in++;
				}
    }
#ifdef TX_ENABLED
		else if (TI) // byte was sent
		{
        TI = 0;             //Clear transmit interrupt flag
			
				if (tx_out > 0) 
				{
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
	int send_uart(uchar dat)
	{
		int res;
		EA = 0;
			
		if (tx_read == tx_write && tx_out > 0) 
		{
			// buffer is full
			res = -1;
		} 
		else 
		{
			tx_buffer[tx_write] = dat;
			tx_write = (tx_write+1)%MAX_BUFFER;
			tx_out++;
			res = 0;
			
			if (TI == 0) 
			{
				TI = 1; // instruct to run interrupt & send the data
			}
		}
			
		EA = 1;
		return res;
	}

	///////////////////////////////////////////////////////////

	void send_str(char* s)
	{
		while (*s) 
		{
			while (send_uart(*s++) != 0) 
			{
				_nop_();
			}
		}
	}

	///////////////////////////////////////////////////////////

	void send_serial(uchar dat) 
	{
		while(send_uart(dat) != 0) 
		{
			_nop_();
		}
	}
#endif
///////////////////////////////////////////////////////////

int recv_uart() 
{
	int value;
	EA = 0;
	
	if (rx_in == 0) { 
		value = -1;
	} else {	
		value = rx_buffer[rx_read];
		rx_read = (rx_read+1)%MAX_BUFFER;
		rx_in--;
	}
	
	EA = 1;
	return value;
}

///////////////////////////////////////////////////////////

uchar read_serial() 
{
	int value;
	while ((value = recv_uart()) == -1) 
	{
		_nop_();
	}
	return (uchar)(value & 0xFF);
}

///////////////////////////////////////////////////////////

void delay5us(void)
{
	unsigned char a,b;
	for(b=7; b>0; b--)
		for(a=2; a>0; a--);
}

///////////////////////////////////////////////////////////

void delay(uint i)
{
	while (i--) {
		delay5us();
	}
}
	
///////////////////////////////////////////////////////////

void clear(char idx, char val) // assign all cube registers the same value, usually 0
{
	uchar i,j;
	for (j=0; j<8; j++) {
		for (i=0; i<8; i++)
			display[idx][j][i] = val;
	}
}

///////////////////////////////////////////////////////////

// light a specific point on the cube (x,y,z), enable = on/off
void point(uchar x, uchar y, uchar z, uchar enable)
{
	uchar ch1 = 1<<x;
	if (enable)
		display[frame][z][y] = display[frame][z][y]|ch1;
	else
		display[frame][z][y] = display[frame][z][y]&(~ch1);
}

///////////////////////////////////////////////////////////

void line(uchar y, uchar z, uchar value)
{
	display[frame][z][y] = value;
}

///////////////////////////////////////////////////////////

void print();

void swap() 
{
	if (frame) 
	{
		frame = 0;
		temp = 1;
	}
	else
	{
		frame = 1;
		temp = 0;
	}
	
	clear(temp,0);
}

///////////////////////////////////////////////////////////

void main()
{
	int value;
	uchar started = 0, 
			received = 0;
	
	// 9600bps@12.000MHz
	PCON &= 0x7F;		//Baudrate no doubled
	SCON = 0x50;		//8bit and variable baudrate, 1 stop bit, no parity
	AUXR |= 0x04;		//BRT's clock is Fosc (1T)
	BRT = 0xD9;			//Set BRT's reload value
	AUXR |= 0x01;		//Use BRT as baudrate generator
	AUXR |= 0x10;		//BRT running
	
	ES = 1;  // enable UART interrupt
	
	// setup timer0
	TH0 = 0xc0;		// reload value
	TL0 = 0;
	TR0 = 1;			// timer0 start
	
	ET0 = 1; // enable timer0 interrupt
	EA = 1;  // enable global interrupts
	
	clear(frame, 0);
	clear(temp, 0);
	
	line(0,0,0xFF); //TODO: remove test line

	while(1) 
	{
		value = read_serial();
		
		if (!started) 
		{
			if (value == 0xF2) { // start receiving batch
				started = 1;
				received = 0;
			}
		} 
		else
		{
			if (received < 64) // already receiving stuff
			{
				display[temp][received/8][received%8] = value;
				received++;
			}
			
			if (received >= 64) // overflow?
			{
				swap();
				started = 0;
			}
		}
	}
}

///////////////////////////////////////////////////////////

//P0;  //573 in
//P1;  //uln2803
//P2;  //573 LE

void print() interrupt 1 // timer0 interrupt
{
	uchar y;
	P1 = 0;
	
	// update one layer at a time
	for (y=0; y<8; y++) {
		P2 = 1<<y;
		delay(3);
		P0 = display[frame][layer][y]; // shift every layer byte
		delay(3);
	}
	
	P1 = 1<<layer;
	if (layer<7)
		layer++;
	else
		layer = 0;
	
	// reset timer0
	TH0 = 0xc0;
	TL0 = 0;
}
