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

#define TOGGLE_LATCH PORTA |= (1<<SS);\
	                 PORTA &= ~(1<<SS);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"
#include "ledcube.h"

uint16_t frame[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; //5 layers and 5 led drivers

void level(uint8_t layer){
    switch(layer){
        case 0:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(0<<PB5)|(0<<PB6));
            return;
        case 1:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((1<<PB4)|(0<<PB5)|(0<<PB6));
            return;
        case 2:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(1<<PB5)|(0<<PB6));
            return;
        case 3:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((1<<PB4)|(1<<PB5)|(0<<PB6));
            return;
        case 4:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(0<<PB5)|(1<<PB6));
            return;
        default:
            PORTB |= ((1<<PB4)|(1<<PB5)|(1<<PB6));
            }
}

void transmit2(uint8_t layer){
	uint8_t temp = 0;
    uint8_t i = 0;
    level(layer);
    for(i=5;i-- > 0; ){
        temp = (frame[layer][i]);
        //load first byte
        SPDR = temp;
        //temp = 0x0F & display_count;
        while(bit_is_clear(SPSR,SPIF)){};

        temp = (frame[layer][i]>>8);
        //load second byte
        SPDR = temp;
        //temp = 0x0F & display_count;
        while(bit_is_clear(SPSR,SPIF)){};
        frame[layer][i] = 0;
    }

	//Toggle latch
	TOGGLE_LATCH
    //_delay_ms(1);
    //_delay_us(300);
}

void red_led(uint8_t layer,uint32_t red){
    uint8_t index = 0;
    uint8_t i = 0;
    uint8_t state = 0;
    for(index = 0; index<25;index++){
        state = ((red>>index)&(0x01));
        if(state){
            for(i=0;i<5;i++){
                frame[layer][i] |= pgm_read_word(&(R[index][i]));
            }
        }
    }
    
//
}

void blue_led(uint8_t layer,uint32_t blue){
    uint8_t index = 0;
    uint8_t i = 0;
    uint8_t state = 0;
    for(index = 0; index<25;index++){
        state = ((blue>>index)&(1));
        if(state){
            for(i=0;i<5;i++){
                frame[layer][i] |= pgm_read_word(&(B[index][i]));
            }
        }
    }
//
}

void green_led(uint8_t layer,uint32_t green){
    uint8_t index = 0;
    uint8_t i = 0;
    uint8_t state = 0;
    for(index = 0; index<25;index++){
        state = ((green>>index)&(1));
        if(state){
            for(i=0;i<5;i++){
                frame[layer][i] |= pgm_read_word(&(G[index][i]));
            }
        }
    }
//
}


void leds(uint8_t layer,uint32_t red, uint32_t blue, uint32_t green){
    red_led(layer,red);
    blue_led(layer,blue);
    green_led(layer,green);
}

void test_frame(){
    uint8_t i = 0;
    for(i=0;i<5;i++){
        //transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        //level(i);
        transmit2(i);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(3);
        //_delay_us(300);
    }
}

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
        leds(4,buffer[0],buffer[1],buffer[2]);
        test_frame();

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

void sensors(uint8_t *buffer){
    buffer[0] = (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
}


int main (void)
{
    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
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
               // led1_on();
               // _delay_ms(50);
               // led_off();
               sensors(buffer);
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
        //leds(4,buffer[0],buffer[1],buffer[2]);
        //test_frame();
    }

}
