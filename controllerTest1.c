//******************************************************************************
// Author:	Patrick Miesen 
// Group:	2 (Multiple LED Cube Controler)
// File: 	controllerTest.c
// Date:	19 Jan. 2013
// Chip:	ATtiny 167
//******************************************************************************

#define TRUE 1
#define FALSE 0
#define F_CPU 8000000		// 8 MHz cpu speed
#define LEVELS 5			// the LED array has 5 levels
#define PIN_NUM 5*5*3		// each level has 5x5 LEDs with 3 pins per LED
#define HALL_NUM 4			// there are 4 hall effect sensors per cube
//#define CUBE_ID 1			// an ID to identify this cube

#include <avr/io.h>
#include <util/delay.h>


#define SS 6
#define MOSI 4
#define SCK 5
//#include "controllerTest.h"


//	init
//******************************************************************************
void init()
{
	DDRB	= ~(1<<PB0);	// Outputs except PB0
	PORTB	= (1<<PB3);		// initial value

	// ZigBee send/receive (PA0), ZigBee SS_N (PA1), SCK (PA5), LED Driver Toggle (PA6)
	//DDRA	= (1<<PA0) | (1<<PA1) | (1<<PA4) | (1<<PA5) | (1<<PA6);

	// Set MOSI and SCK output 
	//DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK);
	// Enable SPI, Master, set clock rate clk/8
	//SPCR	|= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	//SPSR	|= (1<<SPI2X);
}

void spi_init(){

	DDRA |= (1<<SS)|(1<<MOSI)|(1<<SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR |= (1<<SPI2X);


}


//	SPI_MasterTransmit
//******************************************************************************
void SPI_MasterTransmit(char cData)
{
	// Start transmission
	SPDR = cData;
	// Wait for transmission complete
	while( bit_is_clear(SPSR,SPIF)){};
	return SPDR;
}


//	Main
//******************************************************************************
int main()
{
	init();
	spi_init();


	int counter2 = 0;
	char cData;
	uint8_t input = 0;
	uint8_t counter1 = 0;
	uint8_t levelDecode[8] = {
		0b00000000, //0
		0b00010000, //1
		0b00100000, //2
		0b00110000, //3
		0b01000000, //4
		0b01010000, //5
		0b01100000, //6
		0b01110000, //7
	};
	
	while( 1 == 1 )
	{
		if( 0 == (counter2 % 2) ){
			cData = 'a';
		}
		else{
			cData = 'b';
		}
		SPI_MasterTransmit(cData);


		if( 0 == (counter2 % 10000) )
		{
			PORTB = (1<<PB3);
			//		_delay_ms(10);
			input = (PINB & (1<<PB0));

			if( 1 == input )
			{
				PORTB |= (1<<PB2);	// PB1 is busted on MCU
				PORTB |= (1<<PB4);
			}
			PORTB |= levelDecode[(counter1 % 8)];

			counter1++;
		}
		counter2++;

//		SPI_MasterTransmit('a');
	}

}

