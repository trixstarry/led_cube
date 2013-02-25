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
#include "usart.h"

void Transmit(uint8_t *buffer,uint8_t buffersize){
    mirf_send(buffer,buffersize);
    _delay_ms(5);
}

int8_t Receive(uint8_t *buffer,uint8_t buffersize){
    uint64_t i = 0;
    while (!mirf_data_ready()){
        if(i > 0x1FFFF){
            return -1;
        }
        i++;
    }
    mirf_get_data(buffer);
    for(i = 0; i < (buffersize); i++)
    {
        USART_Transmit(buffer[i]);
    }
    USART_Transmit('\n');
    return 1;
}

void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[len-1]++;
    if (buffer[len-1] < ' ' || buffer[15] > 'z')
    {
        buffer[len-1] = ' ';
    }
    Transmit(buffer,len);
    
}

void test_protocol(uint8_t *buffer, uint8_t len){
    transmit_string("Transmitting\r\n");
    Transmit(buffer,len);
    _delay_ms(5);
    transmit_string("Rx_Powerup\r\n");
    rx_powerup();
    _delay_ms(1);
    transmit_string("Receiving\r\n");
    if(Receive(buffer,len)==1){
        transmit_string("received\r\n");
    }
    else{
        transmit_string("nothing received\r\n");
    }
}

int main (void)
{
	USART_Init(MY_UBBR);
	
	char buffer [32] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}',
                        'l','r','b','l','v',' ','n','o','o','d','l','e','!','.','.','\n'};
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
    //rx_powerup();
	
	while (1)
	{
        //test_Transmit(buffer,BUFFER_SIZE);
        
        //Transmit(buffer,BUFFER_SIZE);
        //_delay_ms(5);
        rx_powerup();
        _delay_ms(1);
        //transmit_string("preSending\n");
        Receive(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
       //Transmit(buffer,BUFFER_SIZE);
        //transmit_string("Did i send?\r\n");
        //_delay_ms(100);

        //test_protocol(buffer,BUFFER_SIZE);
	}
	
}
