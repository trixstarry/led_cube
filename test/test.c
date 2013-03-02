// test.c
// Simple tests for ATtiny2313
// 2-22-12

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi.h"

#define F_CPU 8000000UL // 1Mhz clock
#define BAUD 250000
#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400

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

// SPI test
void spi_test ()
{
    spi(0x45);
}

// USART test
void usart_test ()
{
    USART_Transmit('a');	
}

int main (void)
{
    spi_init();
    USART_Init(MY_UBBR);
    while(1)
    {
        spi_test();
        usart_test();
    }
}
