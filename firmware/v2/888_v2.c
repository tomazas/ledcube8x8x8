#include <STC12C5A60S2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

volatile uchar display[2][8][8]; // 8x8x8 = (Z,Y,X)
volatile uchar frame = 0;
volatile uchar temp = 1;
volatile uchar layer = 0;

#define FOSC        12000000L 	// mcu clock - 12MHz
#define BAUD_RATE 	9600
#define BAUD        (256 - FOSC / 32 / BAUD_RATE)
#define MAX_BUFFER 32

volatile bit busy;
volatile uchar io_buffer[MAX_BUFFER];
volatile int io_read = 0;
volatile int io_write = 0;

///////////////////////////////////////////////////////////

void init_uart()
{
    SCON = 0x50;                //set UART mode as 8-bit variable baudrate
    TMOD = 0x20;                //timer1 as 8-bit auto reload mode
    AUXR = 0x40;                //timer1 work at 1T mode
    TH1 = TL1 = BAUD;           //115200 bps
    TR1 = 1;										//timer1 start
		ES = 1;                 //Enable UART interrupt
    EA = 1;                 //Open master interrupt switch
}

void uart_isr() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
			
				if (!((io_write+1)%MAX_BUFFER == io_read)) {
					io_buffer[io_write] = SBUF;
					io_write = (io_write+1)%MAX_BUFFER;
				}
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

void send_uart(uchar dat)
{
    while (busy);           //Wait for the completion of the previous data is sent
    busy = 1;
    SBUF = dat;             //Send data to UART buffer
}

///////////////////////////////////////////////////////////

/*void send_uart(uchar dat)
{
    while (!TI);                //wait pre-data sent
    TI = 0;                     //clear TI flag
    SBUF = dat;                 //send current data
}*/

///////////////////////////////////////////////////////////

void send_str(char* s)
{
	while (*s) {
		send_uart(*s++);
	}
}

///////////////////////////////////////////////////////////

/*uchar recv_uart()
{
    while (!RI);                //wait receive complete
    RI = 0;                     //clear RI flag
    return SBUF;                //return receive data
}*/

int recv_uart() {
	uint value;
	if (io_write-io_read == 0) { 
		return -1;
	}
	
	value = io_buffer[io_read];
	io_read = (io_read+1)%MAX_BUFFER;
	return value;
}

///////////////////////////////////////////////////////////

void delay5us(void)   //误差 -0.026765046296us STC 1T 22.1184Mhz
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
	}//12t的mcu 注释这个延时即可
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

void main()
{
	int value;
	uchar i, 
			started = 0, 
			received = 0;
	
	IE = 0x82;
	TCON = 0x01;
	TH0 = 0xc0;
	TL0 = 0;
	TR0 = 1;			// timer0 start
	
	clear(frame, 0);
	clear(temp, 0);
	
	init_uart();

	while(1) 
	{
		value = recv_uart();
		if (value == -1) continue;
		
		if (!started) 
		{
			if (value == 0xF2) { // start receiving batch
				started = 1;
				send_str("started");
			} //else if (value == 0xF0) { // clear only 
				//value = recv_uart();
				//clear(temp,value);
				//swap();
				//print();
			//} else if (value == 0xF1) { // light up one column
			//	column = recv_uart();
				//value = recv_uart();
			//	line(column%8,column/8,value);
			//}
		} 
		else
		{
			if (received < 64) // already receiving stuff
			{
				display[temp][received/8][received%8] = value;
				received++;
				send_uart(value);
			}
			
			if (received >= 64) // overflow?
			{
				send_str("re-paint");
				/*
				for (i=0; i<10; i++) {
					delay(8000);
					point(1,1,1,1);
					delay(8000);
					point(1,1,1,0);
				}*/
				
				swap();
				received = started = 0;	
			}
		}
	}
}

///////////////////////////////////////////////////////////

//P0;  //573 in
//P1;  //uln2803
//P2;  //573 LE

void print() interrupt 1
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
	
	// reset timer counter
	TH0 = 0xc0;
	TL0 = 0;
}
