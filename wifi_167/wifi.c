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
//

#define F_CPU 8000000UL	// 16Mhz clock
#define BAUD 250000

#define BUFFER_SIZE 32

#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"

void Transmit(uint8_t *buffer, uint8_t buffersize){
    	mirf_send(buffer,buffersize);
		_delay_ms(10);
}


void Receive(uint8_t *buffer,uint8_t buffersize){
		while (!mirf_data_ready());
		mirf_get_data(buffer);
        SPI_Transmit_All(buffer,buffersize);
}

void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[BUFFER_SIZE-1]++;
    if (buffer[BUFFER_SIZE-1] < ' ' || buffer[BUFFER_SIZE-1] > 'z')
    {
        buffer[BUFFER_SIZE-1] = ' ';
    }
    Transmit(buffer,BUFFER_SIZE);
}
void test_protocol(uint8_t *buffer, uint8_t len){
    Receive(buffer,len);
    Transmit(buffer,len);
}

int main (void)
{
	
    //char buffer [16] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
    char buffer [32] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}',
        'l','a','u','g','h',' ','i','t',' ','u','p',' ','R','u','t','h',};
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
	sei();
	
	mirf_read_register (STATUS, buffer, 1);
	
	// Configure mirf
	mirf_config();
	// Test transmitting
	buffer[0] = 'h';
	
	while (1)
	{
        //test_Transmit(buffer, BUFFER_SIZE);
        test_protocol(buffer,BUFFER_SIZE);
	}

}
