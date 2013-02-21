/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define PORT_SPI    PORTA
#define DDR_SPI     DDRA
#define DI			PA4
#define DO			PA2
#define USCK		PA5

void spi_init()
// Initialize pins for spi communication
{
    DDR_SPI &= ~(1<<DI);
	PORT_SPI |= (1<<DI); 
	DDR_SPI |= (1<<USCK)|(1<<DO); 
	USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK); 
	
	// For disabling SPI
	//DDRB &= ~((1<<USCK)|(1<<DI)|(1<<DO)); 
	//USICR = 0; 
}

unsigned char spi(unsigned char val) 
{ 
	USIDR = val; 
	USISR = (1<<USIOIF); 
	do
	{ 
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC); 
	}
	while ((USISR & (1<<USIOIF)) == 0); 
	return USIDR; 
} 

void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
	uint8_t i;      
	for (i = 0; i < len; i++)
	{
		datain[i] = spi(dataout[i]);
	}
}

void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
	uint8_t i;      
	for (i = 0; i < len; i++)
	{
		spi(dataout[i]);
	}
}

uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
	return spi(data);
}

