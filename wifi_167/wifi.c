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

#define LED1_ON     PORTB |= (1<<PB0)
#define LED1_OFF    PORTB &= ~(1<<PB0)
#define LED2_ON     PORTB |=  (1<<PB1)
#define LED2_OFF    PORTB &= ~(1<<PB1)



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


int8_t Receive(uint8_t *buffer,uint8_t buffersize){
        uint64_t i = 0;
		while (!mirf_data_ready()){
            if(i > 0x1FFFF){
       //         LED1_ON;
                return -1;
            }
            LED1_ON;
            i++;
        }
		mirf_get_data(buffer);
        SPI_Transmit_All(buffer,buffersize);
        return 1;
}

void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[len-1]++;
    if (buffer[len-1] < ' ' || buffer[len-1] > 'z')
    {
        buffer[len-1] = ' ';
    }
    Transmit(buffer,len);
}
void test_protocol(uint8_t *buffer, uint8_t len){
    if(Receive(buffer,len) == 1){
        Transmit(buffer,len);
    }
    //Transmit(buffer,len);
    else{
      LED1_ON;
    }
}

void init(){
    DDRB |= (1<<PB0)|(1<<PB1);
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
}
void init2(uint8_t *buffer){
	mirf_read_register (STATUS, buffer, 1);
	
	// Configure mirf
	mirf_config();
	// Test transmitting
}

int main (void)
{
    init();
    //char buffer [16] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
    char buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F',};
	sei();
    init2(buffer);
	buffer[0] = 'h';
    //LED2_ON;
    //LED1_ON;
	
	while (1)
	{
        //_delay_ms(100);
        //test_Transmit(buffer, BUFFER_SIZE);
        //test_protocol(buffer,BUFFER_SIZE);
        //_delay_ms(500);
        //LED1_OFF;
        if(Receive(buffer,BUFFER_SIZE) == 1){
            if(buffer[2] == 'e'){
                LED1_ON;
            }
        }
        else{
            LED2_ON;
        }
    }

}
