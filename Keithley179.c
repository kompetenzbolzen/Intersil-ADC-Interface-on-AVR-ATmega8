
#ifndef F_CPU
#define F_CPU 4000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BAUDRATE 9600

#define UBRR_VAL (F_CPU / UART_BAUDRATE * 16) - 1

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

void UART_Init();
/*
*Initializes the  UART Interface
*/

void UART_Handle();
/*
* should be called on Interrupt of UART
* Handles UART Communication
*/

void UART_Send(char *_cSend);
/*
*Sends String over UART
*/

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
	
	
	char temp = 0;
	in_cntr = 0;
	
	UART_RX = 0;
	UART_RX_END = 0;
	UART_isInit = 0;
	
	//Set DDRD as Input
	DDRD = 0;
	DDRC = 0;
	
	DDRD |= (1<<PD1); //TX Pin
	
	sei();
	
	//INIT UART Interface
	UART_Init();
	UART_Send("Hello World!");
	
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
		//UART_Handle();
		
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
		uint16_t baudrate = (4000000 / (9600 * 16)) - 1;
		
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
	if(UART_isInit == 1)
	{
		
	}
}//UART_Handle

void UART_Send(char *_cSend)
{
	if(UART_isInit == 1)
	{
		while(*_cSend)
		{
			while (!(UCSRA & (1<<UDRE)));
			UDR = _cSend;
			_cSend++;
		}
	}
}//UART_send