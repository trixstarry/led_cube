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
//oh yea

#define F_CPU 8000000UL	// 1Mhz clock
#define BAUD 250000
#define BUFFER_SIZE 32
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

void Transmit(uint8_t *buffer,uint8_t buffersize){
    mirf_send(buffer,buffersize);
    _delay_ms(5);
}

void Receive(uint8_t *buffer,uint8_t buffersize){
    while (!mirf_data_ready()){};
    mirf_get_data(buffer);
    uint8_t i;
    for(i = 0; i < (buffersize); i++)
    {
        USART_Transmit(buffer[i]);
    }
    USART_Transmit("\n");
}

void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[15]++;
    if (buffer[15] < ' ' || buffer[15] > 'z')
    {
        buffer[15] = ' ';
    }
    Transmit(buffer,len);
}

void test_protocol(uint8_t *buffer, uint8_t len){
    Transmit(buffer,len);
    Receive(buffer,len);
}

int main (void)
{
	USART_Init(MY_UBBR);
	
	char buffer [32] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}',
                        'l','a','u','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
	uint8_t buffersize = BUFFER_SIZE;
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts sei(); 
	mirf_read_register (STATUS, buffer, 1);
	// Configure mirf
	mirf_config();
	// Test transmitting
	buffer[0] = 'h';
	
	while (1)
	{
        //Receive()
        test_protocol(buffer,BUFFER_SIZE);
	}
	
}
