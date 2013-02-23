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
#define BAUD 250000
//#define MY_UBBR 1 // F_CPU/(16*BAUD)-1
#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400
//#define BAUD 9600
//#define MY_UBBR 51 // F_CPU/(16*BAUD)-1

//#define SET_U2X 1 // Set U2X in UCSRA (double speed USART)
//#define MY_UBBR 12 // 76800


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"

void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	if (SET_U2X)
	{
		UCSRA |= (1<<U2X);
	}
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
	
	char buffer [16] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
	uint8_t buffersize = 16;
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
	sei();
	
	/*while (1)
	{
		char s = USART_Receive();
		USART_Transmit(s+1);
		transmit_string("Hallo Welt!\r\n");
	}*/
	
	//while (1)
	//{
		mirf_read_register (STATUS, buffer, 1);
		/*if (buffer[0] == 0x0E)
		{	
			PORTB |= (1<<PB1); // Test passed LED on
		}
		else
		{
			PORTB |= (1<<PB0); // Test failed LED on
		}*/
		//_delay_ms (100);
	//}
	
	// Configure mirf
	mirf_config();
	// Test transmitting
	buffer[0] = 'h';
	
	char testing_sender = 1;
	while (testing_sender)
	{
		buffer[15]++;
		if (buffer[15] < 'a' || buffer[15] > 'z')
		{
			buffer[15] = 'a';
		}
		
		mirf_send(buffer,buffersize);
		_delay_ms(5);
	}
	
	char expected [16] = {'h','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
	// Test receiving
	while (1)
	{
		while (!mirf_data_ready());
		mirf_get_data(buffer);
		PORTB |= (1<<PB1);
		char i;
		char matched = 1;
		//transmit_string("data = ");
		for(i = 0; i < 15; i++)
		{
			if (expected[i] != buffer[i])
			{
				matched = 0;
			}
			//USART_Transmit(buffer[i]);
		}
		USART_Transmit(buffer[15]);
		if (matched)
		{
			transmit_string(" OK\r\n");
		}
		else
		{
			transmit_string(" BAD\r\n");
		}
	}

	/*enable_spi(1); // Initialize SPI stuff

	DDRB |= (1<<PB2); // Chip select line set to output

	while (1)
	{
		PORTB &= ~(1<<PB2); // CSN goes low, radio selected

		_delay_us(10); // Wait a little bit for the heck of it...  (it might take a couple uS for the CSN line to move)
		spi (0x07); // Request status byte
		char data = spi (0x00); // Get back the status byte
		if (data == 0x0E) // Default value for status is 0x0E
		{
			PORTB |= (1<<PB0); // Do something here to indicate we got the expected value :)
		}	
		else
		{
			PORTB |= (1<<PB1); // Do something here to indicate we got a bad value... :(
		}

		PORTB |= (1<<PB2); // CSN goes high, radio unselected
		
		_delay_ms (200);// Wait a while before doing it again
	}*/
}
