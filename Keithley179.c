/*
* Interface for ICL71c03 ADC 
* Designed for the Keithley 179 True RMS Multimeter
*/


//#ifndef F_CPU
#define F_CPU 4000000UL //4MHz
//#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BAUDRATE 960

#define baudrate ((F_CPU / UART_BAUDRATE * 16) - 1)

//Locations of ADC Outups (ICL71c03)
#define b1 PC0
#define b2 PC1
#define b3 PC2
#define b4 PC3

#define d1 PD7
#define d2 PD4
#define d3 PD5
#define d4 PD6
#define d5 PC3

#define busy PB7 //Busy not Used

//Status
uint8_t UART_RX;
uint8_t UART_RX_END;
uint8_t UART_isInit;

//Buffers
char out[5]; //Buffer for ADC Input
char in[16]; //UART Input Buffer
uint8_t in_cntr; //Counter for UART Input Buffer

//UART Rcieve Interrupt
ISR(USART_RXC_vect)
{
	cli();
	in[in_cntr] = UDR;
	if(in[in_cntr] == '\n')
		UART_RX_END = 1; //Set RX End Flag
	
	//Check if Buffer is full
	if(in_cntr < 15)
	in_cntr ++;
	else
	{
		in_cntr = 0;
		UART_RX_END = 1; //Set RX End Flag
	}
	sei();
}//USART_RXC_vect

//Prototypen
void UART_Init();
/*
*Initializes the  UART Interface
*/

void UART_Handle();
/*
* Handles UART Communication
* Not Implemented
*/

void UART_Send(char *_cSend);
/*
*Sends String over UART
*/

void UART_Putc(unsigned char c);

//Startpoint
int main(void)
{
	//clear Buffers
	for(int i = 0; i < 5;i++)
	{
		out[i] = 0;
	}
	
	for(int i = 0; i < 16;i++)
	{
		in[i] = 0;
	}
	
	//Init Status vars
	char temp = 0;
	in_cntr = 0;
	
	UART_RX = 0;
	UART_RX_END = 0;
	UART_isInit = 0;
	
	//Set Inputs
	DDRD = 0;
	DDRC = 0;
	
	DDRD |= (1<<PD1); //TX Pin
	
	sei();
	
	//INIT UART Interface
	UART_Init();
	//UART_Send("Hello World!");
	UART_Putc('A');
	//main loop
	while(1)
	{
		while(!(PORTD & (1<<d1)));
		temp = PINB << 4;
		out[4] = temp >> 4;
		temp = 0;
		
		while(!(PORTD & (1<<d2)));
		temp = PINB << 4;
		out[3] = temp >> 4;
		temp = 0;
		
		while(!(PORTD & (1<<d3)));
		temp = PINB << 4;
		out[2] = temp >> 4;
		temp = 0;
		
		while(!(PORTD & (1<<d4)));
		temp = PINB << 4;
		out[1] = temp >> 4;
		temp = 0;
		
		while(!(PORTC & (1<<d5)));
		temp = PINB << 4;
		out[0] = temp >> 4;
		temp = 0;
		
		
		//Convert to ASCII
		for(int i = 0; i < 5;i++)
		{
			out[i] = out[i] + 48;
		}
		
		UART_Send(out);
		//Handle UART
		//UART_Handle(); //Not Used
		
		//Reset Buffer
		for(int i = 0; i < 5;i++)
		{
			out[i] = 0;
		}
		
	}
	
	return 0;
}//main

void UART_Init()
{
	if(UART_isInit == 0)
	{
		//Calculate Value for Register
		//uint16_t baudrate = (F_CPU / (UART_BAUDRATE * 16)) - 1;
		
		//Set Baudrate
		UBRRL = baudrate;
		UBRRH = (baudrate >> 8);
		
		UCSRB |= (1<<RXEN) | (1<<TXEN); //Enable Reciever and Transmitter
		UCSRB |= (1<<RXCIE); //Enable Recieve Complete Interrupt
		
		UART_isInit = 1;
	}
	
}//UART_Init



void UART_Handle()
{
	if(UART_isInit == 1 && UART_RX_END == 1)
	{
		//Not Used
		//char in[16] is UART InputBuffer
	}
}//UART_Handle

void UART_Send(char *_cSend)
{
	if(UART_isInit == 1)
	{
		while(*_cSend)
		{
			/*while (!(UCSRA & (1<<UDRE)));
			UDR = *_cSend;*/
			UART_Putc(*_cSend);
			_cSend++;
		}
	}
}//UART_send

void UART_Putc(unsigned char c)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = c;
}