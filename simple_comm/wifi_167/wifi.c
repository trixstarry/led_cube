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
#define DATA 2 //starting position of data bytes
#define DATA2 33

#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400

#define SS PA6
#define MOSI PA4
#define SCK PA5

#define TOGGLE_LATCH PORTA |= (1<<SS);\
	                 PORTA &= ~(1<<SS);

#define CUBE1   0x01
#define CUBE2   0x02
#define CUBE3   0x04
#define CUBE4   0x08
#define ID_SELF CUBE1

#define ACK     0x11
#define DATA_S    0x22
#define NACK    0x33

#define MSB1  0x80
#define MSB2  0xC0
#define MSB3  0xE0
#define MSB4  0xF0
#define MSB5  0xF8
#define MSB6  0xFC
#define MSB7  0xFE

#define LSB1  0x01
#define LSB2  0x03
#define LSB3  0x07
#define LSB4  0x0F
#define LSB5  0x1F
#define LSB6  0x3F
#define LSB7  0x7F

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
    _delay_ms(100);
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
    uint8_t j = 0;
    uint8_t temp = 0;
    for(i=0;i<5;i++){
        //transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        //level(i);
        //transmit2(i);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(3);
        //_delay_us(300);
        switch(i){
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
        //level(i);
        for(j=5;j-- > 0; ){
            temp = (frame[i][j]);
            //load first byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};

            temp = (frame[i][j]>>8);
            //load second byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};
            frame[i][j] = 0;
        }

        //Toggle latch
        TOGGLE_LATCH
            _delay_ms(100);
    }
}

void test_cube(uint8_t *buffer){
    uint32_t red = 0;
    uint32_t blue = 0;
    uint32_t green = 0;


    red = (buffer[0])|(buffer[0+1]<<8)|(buffer[0+2]<<16)|((buffer[0+3]&MSB1)<<24);
    blue = (buffer[0+15]&(~MSB5))|(buffer[0+16]<<3)|(buffer[0+17]<<11)|((buffer[0+18]&MSB6)<<19);
    green = ((buffer[30+1]&(~MSB2)))|(buffer[30+2]<<6)|(buffer[30+3]<<14)|((buffer[30+4]&MSB3)<<22);
    leds(0,red,blue,green);
    //LAYER 1
    red = ((buffer[0+3]&(~MSB1)))|(buffer[0+4]<<7)|(buffer[0+5]<<15)|((buffer[0+6]&MSB2)<<23);
    blue = (buffer[0+18]&(~MSB6))|(buffer[0+19]<<2)|(buffer[0+20]<<10)|((buffer[0+21]&MSB7)<<18);
    green = ((buffer[30+4]&(~MSB3)))|(buffer[30+5]<<5)|(buffer[30+6]<<13)|((buffer[30+7]&MSB4)<<21);
    leds(1,red,blue,green);
    //LAYER 2
    red = (buffer[0+6]&(~MSB2))|(buffer[0+7]<<6)|(buffer[0+8]<<14)|((buffer[0+9]&MSB3)<<22);
    blue = (buffer[0+21]&(~MSB7))|(buffer[0+22]<<1)|(buffer[0+23]<<9)|((buffer[0+24])<<17);
    green = ((buffer[30+7]&(~MSB4)))|(buffer[30+8]<<4)|(buffer[30+9]<<12)|((buffer[30+10]&MSB5)<<20);
    leds(2,red,blue,green);
    //LAYER 3
    red = ((buffer[0+9]&(~MSB3)))|(buffer[0+10]<<5)|(buffer[0+11]<<13)|((buffer[0+12]&MSB4)<<21);
    blue = (buffer[0+25])|(buffer[0+26]<<8)|(buffer[0+27]<<16)|((buffer[0+28]&MSB1)<<24);
    green = ((buffer[30+10]&(~MSB5)))|(buffer[30+11]<<3)|(buffer[30+12]<<11)|((buffer[30+13]&MSB6)<<19);
    leds(3,red,blue,green);
    //LAYER 4
    red = ((buffer[0+12]&(~MSB4)))|(buffer[0+13]<<4)|(buffer[0+14]<<12)|((buffer[0+15]&MSB5)<<20);
    blue = ((buffer[0+28]&(~MSB1)))|(buffer[0+29]<<7)|(buffer[30]<<15)|((buffer[30+1]&MSB2)<<23);
    green = ((buffer[30+13]&(~MSB6)))|(buffer[30+14]<<2)|(buffer[30+15]<<10)|((buffer[30+15]&MSB7)<<18);
    leds(4,red,blue,green);
    //test_frame();
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t temp = 0;
    for(i=0;i<5;i++){
        //transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        //level(i);
        //transmit2(i);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(3);
        //_delay_us(300);
        switch(i){
            case 0:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(0<<PB5)|(0<<PB6));
                break;
            case 1:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((1<<PB4)|(0<<PB5)|(0<<PB6));
                break;
            case 2:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(1<<PB5)|(0<<PB6));
                break;
            case 3:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((1<<PB4)|(1<<PB5)|(0<<PB6));
                break;
            case 4:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(0<<PB5)|(1<<PB6));
                break;
            default:
                PORTB |= ((1<<PB4)|(1<<PB5)|(1<<PB6));
        }
        //level(i);
        for(j=5;j-- > 0; ){
            temp = (frame[i][j]);
            //load first byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};

            temp = (frame[i][j]>>8);
            //load second byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};
            frame[i][j] = 0;
        }

        //Toggle latch
        TOGGLE_LATCH
            if(i < 4){
                _delay_ms(2);
            }
            else if(4 == i){
                //_delay_us(100);
            }
            else{
                _delay_ms(500);
            }
    }
}

void Transmit(uint8_t *buffer, uint8_t buffersize){
        uint16_t counter = 0;
        while (mirf_send (buffer, BUFFER_SIZE) && counter < 1000)
        {
            _delay_us(10);
        }
        if (counter >= 1000)
        {
            //transmit_string("e");
        }
		//_delay_ms(10);
}


int8_t Receive(uint8_t *buffer,uint8_t *receive_buffer,uint8_t buffersize){
        //uint64_t i = 0;
		while (!mirf_data_ready()){
            test_cube(buffer);
            //if(i > 0x1FFFF){
                //LED1_ON;
            //    return -1;
           // }
            //led1_on();
            //i++;
        }
		mirf_get_data(receive_buffer);
        //leds(4,buffer[0],buffer[1],buffer[2]);
        //test_frame();

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
/*
void test_protocol(uint8_t *buffer, uint8_t len){
    if(Receive(buffer,len) == 1){
        Transmit(buffer,len);
    }
    //Transmit(buffer,len);
    else{
      //LED1_ON;
    }
}
*/

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

uint8_t sensors(){
    return (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
}


int main (void)
{
    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
    init();
    //LED1_ON;
    //LED2_ON;
    uint8_t temp_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t receive_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t buffer [64] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t i = 0;
	sei();
    init2(receive_buffer);
    //buffer[0] = 'a';
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
   uint8_t data[3] = {0,0,0};
	
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
        if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
            if((ID_SELF == receive_buffer[0])&&(0x04 == receive_buffer[1])){
                uint16_t counter = 0;
                uint16_t i = 0;
                for(i = 2; i < BUFFER_SIZE;i++){
                    buffer[i-2] = receive_buffer[i];
                }
                data[0] = receive_buffer[0];
                data[1] = receive_buffer[1];
                data[2] = receive_buffer[2];
                receive_buffer[0] = ID_SELF;
                receive_buffer[1] = ACK;
                receive_buffer[2] = sensors();
                //uint16_t counter = 0;
                while (mirf_send (buffer, BUFFER_SIZE) && counter < 1000)
                {
                    _delay_us(10);
                }
                if (counter >= 1000)
                {
                    //transmit_string("e");
                }
                receive_buffer[0] = data[0];
                receive_buffer[1] = data[1];
                receive_buffer[2] = data[2];
                //uint16_t i = 0;
                //for(i = 0; i < BUFFER_SIZE;i++){
                //    buffer[i] = receive_buffer[i];
                //}
                //led1_on();
                //_delay_ms(500);
                //led_off();
            }
            if((receive_buffer[0] == ID_SELF)&&(receive_buffer[1] == 0x08)){
                for(i = 34; i < 64; i++){
                    buffer[i-2] = receive_buffer[i-32];
                    //temp_buffer[i-32] = buffer[i];
                }
                Transmit(receive_buffer,BUFFER_SIZE);
            }
            /*
                while((receive_buffer[0] != ID_SELF)&&(receive_buffer[1] != 0x08)){
                    if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
                        if((receive_buffer[0] == ID_SELF)&&(receive_buffer[1] == 0x08)){
                            //led3_on();
                               //data[0] = buffer[0];
                               //data[1] = buffer[1];
                               //data[2] = buffer[2];
                               //buffer[0] = ID_SELF;
                               //buffer[1] = ACK;
                               //buffer[2] = (sensors()+0xF0);
                               //Transmit(buffer,BUFFER_SIZE);
                               //buffer[0] = data[0];
                               //buffer[1] = data[1];
                               //buffer[2] = data[2];

                            for(i = 34; i < 64; i++){
                                buffer[i-2] = receive_buffer[i-32];
                                //temp_buffer[i-32] = buffer[i];
                            }
                            Transmit(buffer,BUFFER_SIZE);

                            //led2_on();
                            //_delay_ms(500);
                            //led_off();
                        }
                    }
                }
            }
        */
            //if((buffer[3] == 'l')){//}&&(buffer[31] == '}')){
            // led1_on();
            // _delay_ms(50);
            // led_off();
            //data = buffer[0];
            //sensors(buffer);
            //Transmit(buffer,BUFFER_SIZE);
            //buffer[0] = data;

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
        //else{
            //led2_on();
            //_delay_ms(50);
            //led_off();
        //}
        //_delay_ms(100);
        //led_off();
        //_delay_ms(50);
        // red is bytes 0 - 13
        // blue is bytes
        // green is bytes
        //
        //LAYER 0
        //led2_on();
        //_delay_ms(500);
        //led_off();
        //_delay_ms(500);
        //test_cube(buffer);
        /*
    uint32_t red = 0;
    uint32_t blue = 0;
    uint32_t green = 0;


    red = (buffer[DATA])|(buffer[DATA+1]<<8)|(buffer[DATA+2]<<16)|((buffer[DATA+3]&MSB1)<<24);
    blue = (buffer[DATA+15]&(~MSB5))|(buffer[DATA+16]<<3)|(buffer[DATA+17]<<11)|((buffer[DATA+18]&MSB6)<<19);
    //green = ((buffer[DATA2+1]&(~MSB2)))|(buffer[DATA2+2]<<6)|(buffer[DATA2+3]<<14)|((buffer[DATA2+4]&MSB3)<<22);
    leds(0,red,blue,green);
    //LAYER 1
    red = ((buffer[DATA+3]&(~MSB1)))|(buffer[DATA+4]<<7)|(buffer[DATA+5]<<15)|((buffer[DATA+6]&MSB2)<<23);
    blue = (buffer[DATA+18]&(~MSB6))|(buffer[DATA+19]<<2)|(buffer[DATA+20]<<10)|((buffer[DATA+21]&MSB7)<<18);
    //green = ((buffer[DATA2+4]&(~MSB3)))|(buffer[DATA2+5]<<5)|(buffer[DATA2+6]<<13)|((buffer[DATA2+7]&MSB4)<<21);
    leds(1,red,blue,green);
    //LAYER 2
    red = (buffer[DATA+6]&(~MSB2))|(buffer[DATA+7]<<6)|(buffer[DATA+8]<<14)|((buffer[DATA+9]&MSB3)<<22);
    blue = (buffer[DATA+21]&(~MSB7))|(buffer[DATA+22]<<1)|(buffer[DATA+23]<<9)|((buffer[DATA+24])<<17);
    //green = ((buffer[DATA2+7]&(~MSB4)))|(buffer[DATA2+8]<<4)|(buffer[DATA2+9]<<12)|((buffer[DATA2+10]&MSB5)<<20);
    leds(2,red,blue,green);
    //LAYER 3
    red = ((buffer[DATA+9]&(~MSB3)))|(buffer[DATA+10]<<5)|(buffer[DATA+11]<<13)|((buffer[DATA+12]&MSB4)<<21);
    blue = (buffer[DATA+25])|(buffer[DATA+26]<<8)|(buffer[DATA+27]<<16)|((buffer[DATA+28]&MSB1)<<24);
    //green = ((buffer[DATA2+10]&(~MSB5)))|(buffer[DATA2+11]<<3)|(buffer[DATA2+12]<<11)|((buffer[DATA2+13]&MSB6)<<19);
    leds(3,red,blue,green);
    //LAYER 4
    red = ((buffer[DATA+12]&(~MSB4)))|(buffer[DATA+13]<<4)|(buffer[DATA+14]<<12)|((buffer[DATA+15]&MSB5)<<20);
    blue = ((buffer[DATA+28]&(~MSB1)))|(buffer[DATA+29]<<7)|(buffer[DATA2]<<15)|((buffer[DATA2+1]&MSB2)<<23);
    //green = ((buffer[DATA2+13]&(~MSB6)))|(buffer[DATA2+14]<<2)|(buffer[DATA2+15]<<10)|((buffer[DATA2+15]&MSB7)<<18);
    leds(4,red,blue,green);
    //test_frame();
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t temp = 0;
    for(i=0;i<5;i++){
        //transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        //level(i);
        //transmit2(i);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(3);
        //_delay_us(300);
        switch(i){
            case 0:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(0<<PB5)|(0<<PB6));
                break;
            case 1:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((1<<PB4)|(0<<PB5)|(0<<PB6));
                break;
            case 2:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(1<<PB5)|(0<<PB6));
                break;
            case 3:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((1<<PB4)|(1<<PB5)|(0<<PB6));
                break;
            case 4:
                PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
                PORTB |= ((0<<PB4)|(0<<PB5)|(1<<PB6));
                break;
            default:
                PORTB |= ((1<<PB4)|(1<<PB5)|(1<<PB6));
        }
        //level(i);
        for(j=5;j-- > 0; ){
            temp = (frame[i][j]);
            //load first byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};

            temp = (frame[i][j]>>8);
            //load second byte
            SPDR = temp;
            //temp = 0x0F & display_count;
            while(bit_is_clear(SPSR,SPIF)){};
            frame[i][j] = 0;
    }

    //Toggle latch
    TOGGLE_LATCH
        _delay_ms(100);
    }
    */
    }

}
