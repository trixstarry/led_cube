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

#define SS 6
#define MOSI 4
#define SCK 5

void spi_init(){

	DDRA |= (1<<SS)|(1<<MOSI)|(1<<SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(0<<SPR0);
	SPSR |= (1<<SPI2X);


}


//	SPI_MasterTransmit
//******************************************************************************
uint8_t SPI_Transmit(uint8_t cData)
{
	// Start transmission
	SPDR = cData;
	// Wait for transmission complete
	while( bit_is_clear(SPSR,SPIF)){};
	return SPDR;
}

void SPI_Transmit_All(uint8_t *data, uint8_t len)
{
    uint8_t i = 0;
    for(i = 0; i < len; i++){
        SPI_Transmit(data[i]);
    }
}

char SPI_Receive(void){
    SPDR = 0xFF;
    while(bit_is_clear(SPSR,SPIF)){};
    return SPDR;
}

void SPI_Receive_All(uint8_t *data,uint8_t len)
{
    uint8_t i = 0;
    for(i = 0; i < 32; i++){
        SPDR = 0x00;
        while(bit_is_clear(SPSR,SPIF)){};
        data[i] = SPDR;
    }
}

void SPI_Transceive(uint8_t *dataout, uint8_t *datain, uint8_t len)
{
    uint8_t i;
    for(i = 0; i < len; i++)
    {
        datain[i] = SPI_Transmit(dataout[i]); 
    }

}



