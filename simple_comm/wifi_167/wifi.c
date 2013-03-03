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

#define SS PA6
#define MOSI PA4
#define SCK PA5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"

void Transmit(uint8_t *buffer, uint8_t buffersize){
    	mirf_send(buffer,buffersize);
		//_delay_ms(10);
}


int8_t Receive(uint8_t *buffer,uint8_t buffersize){
        //uint64_t i = 0;
		while (!mirf_data_ready()){
            //if(i > 0x1FFFF){
                //LED1_ON;
            //    return -1;
           // }
            //led1_on();
            //i++;
        }
		mirf_get_data(buffer);
        //SPI_Transmit_All(buffer,buffersize);
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
      //LED1_ON;
    }
}

void init(){
    //DDRA |= (1<<PB0)|(1<<PB1);
    //PORTB |= (1<<PB0)|(1<<PB1);
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
    //PORTB |= (1<<PB0)|(1<<PB1);
}
void init2(uint8_t *buffer){
	mirf_read_register (STATUS, buffer, 1);
	
	// Configure mirf
	mirf_config();
	// Test transmitting
    //PORTA |= (0<<PA0)|(1<<PA1);
}

void transmit_led(uint8_t data){
    SPI_Transmit(data);
}


void led1_on(){
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x01);
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}
void led_off(){
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
    
}
void led2_on(){
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x02);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x02);
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}
void led3_on(){
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x00);
    transmit_led(0x03);
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}

void test_xbee_pins(){

    PORTA |= ((1<<PA0));
    //PORTA |= ((1<<PA1));
    //PORTA |= ((1<<PA0)|(1<<PA1));
    _delay_ms(500);
    PORTA &= ~((1<<PA0)|(1<<PA1));

}

int main (void)
{
    init();
    //LED1_ON;
    //LED2_ON;
    //char buffer [16] = {'.','e','l','l','o',' ','n','o','o','d','l','e','!','.','.','}'};
    char buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F',};
	sei();
    init2(buffer);
    buffer[0] = 'a';
    //LED2_ON;
    //LED1_ON;
    //PORTB |= (1<<PB1);
    //_delay_ms(500);
    //LED1_OFF;
   // while(1){};
   //led1_off();
   //led2_off();
   //rx_powerup();
   //_delay_ms(1);
   led1_on();
   _delay_ms(50);
   led_off();
   _delay_ms(50);
   led1_on();
   _delay_ms(50);
   led_off();
   _delay_ms(50);
   led1_on();
   _delay_ms(50);
   led_off();
   _delay_ms(50);
   led1_on();
   _delay_ms(50);
   led_off();
   _delay_ms(50);
   led1_on();
   _delay_ms(50);
   led_off();
	
	while (1)
	{
        //test_xbee_pins();
        //_delay_ms(100);
        //PORTB |= (1<<PB1);
        /*
        led1_on();
        led2_off();
        test_Transmit(buffer, BUFFER_SIZE);
        _delay_ms(100);
        led1_off();
        led2_on();
        _delay_ms(100);
        led2_off();
        led1_off();
        //test_protocol(buffer,BUFFER_SIZE);
        //_delay_ms(500);
        //LED1_OFF;
        */
        //led1_on();
        //rx_powerup();
        //_delay_ms(1);
        //test_Transmit(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
        //_delay_ms(500);
        //led_off();
        //_delay_ms(500);
        //_delay_ms(50);
        if(Receive(buffer,BUFFER_SIZE) == 1){
            //if((buffer[3] == 'l')){//}&&(buffer[31] == '}')){
                led1_on();
                _delay_ms(50);
                led_off();
                Transmit(buffer,BUFFER_SIZE);
//                //rx_powerup();
//                //_delay_ms(1);
//                //led1_on();
//                //led2_off();
////            }
//            else{
//            led2_on();
//            led_off();
//            }
        }
        else{
            led2_on();
            _delay_ms(50);
            led_off();
        }
        //led_off();
        _delay_ms(50);
    }

}
