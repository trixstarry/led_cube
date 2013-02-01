//************************************************************************\\
// wifi.c
//
// 	Author:	Thao-Trang "Valerie Hoang
// 	Date:	1.16.13
//
// Description:
// This program sends and receives data between two nrf24l01 chips.
//
// Notable information:
// Both the Reads on rising edge and changes on falling edge.
//************************************************************************//

#define F_CPU 8000000UL	// 1Mhz clock
#define BAUD 9600
#define MY_UBBR 51 // F_CPU/(16*BAUD)-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}

transmit_string(char *ptr)
{
	while(*ptr)
	{
		USART_Transmit(*ptr);
		ptr++;
	}
}

int main (void)
{
	USART_Init(MY_UBBR);
	DDRB |= (1<<PB1); // Test passed LED
	DDRB |= (1<<PB0); // Test failed LED
	
	char buffer [16] = {'h','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
	uint8_t buffersize = 16;
	
	// Activate interrupts
	sei();
	
	uint8_t i = 0;
	
	while(1)
	{
		USART_Transmit(buffer[i]);
		i++;
		i = i%15;
	}
}
