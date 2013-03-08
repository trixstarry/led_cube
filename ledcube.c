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
#define SENSORS (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3))) 

#define CUBE1   0x01
#define CUBE2   0x02
#define CUBE3   0x04
#define CUBE4   0x08
#define ID_SELF CUBE1
#define PACKET1 0x04
#define PACKET2 0x08

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
    ///frame[layer][0] = 0;
    //frame[layer][1] = 0;
    //frame[layer][2] = 0;
    //frame[layer][3] = 0;
    //frame[layer][4] = 0;
    red_led(layer,red);
    blue_led(layer,blue);
    green_led(layer,green);
}

void test_frame(){
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t temp = 0;
    for(i=0;i<5;i++){
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

void translate(uint8_t *buffer){
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
}

void test_cube(uint8_t *buffer){
    uint32_t red = 0;
    uint32_t blue = 0;
    uint32_t green = 0;


    red = (buffer[0])|(buffer[0+1]<<8)|(buffer[0+2]<<16)|((buffer[0+3]&LSB1)<<24);
    blue = (buffer[0+15]&(~LSB5))|(buffer[0+16]<<3)|(buffer[0+17]<<11)|((buffer[0+18]&LSB6)<<19);
    green = ((buffer[30+1]&(~LSB2)))|(buffer[30+2]<<6)|(buffer[30+3]<<14)|((buffer[30+4]&LSB3)<<22);
    //red = ((buffer[0+3]&LSB1)<<24)|(buffer[0+2]<<16)|(buffer[0+1]<<8)|(buffer[0])
    leds(0,red,blue,green);
    //LAYER 1
    red = ((buffer[0+3]&(~LSB1)))|(buffer[0+4]<<7)|(buffer[0+5]<<15)|((buffer[0+6]&LSB2)<<23);
    blue = (buffer[0+18]&(~LSB6))|(buffer[0+19]<<2)|(buffer[0+20]<<10)|((buffer[0+21]&LSB7)<<18);
    green = ((buffer[30+4]&(~LSB3)))|(buffer[30+5]<<5)|(buffer[30+6]<<13)|((buffer[30+7]&LSB4)<<21);
    leds(1,red,blue,green);
    //LAYER 2
    red = (buffer[0+6]&(~LSB2))|(buffer[0+7]<<6)|(buffer[0+8]<<14)|((buffer[0+9]&LSB3)<<22);
    blue = (buffer[0+21]&(~LSB7))|(buffer[0+22]<<1)|(buffer[0+23]<<9)|((buffer[0+24])<<17);
    green = ((buffer[30+7]&(~LSB4)))|(buffer[30+8]<<4)|(buffer[30+9]<<12)|((buffer[30+10]&LSB5)<<20);
    leds(2,red,blue,green);
    //LAYER 3
    red = ((buffer[0+9]&(~LSB3)))|(buffer[0+10]<<5)|(buffer[0+11]<<13)|((buffer[0+12]&LSB4)<<21);
    blue = (buffer[0+25])|(buffer[0+26]<<8)|(buffer[0+27]<<16)|((buffer[0+28]&LSB1)<<24);
    green = ((buffer[30+10]&(~LSB5)))|(buffer[30+11]<<3)|(buffer[30+12]<<11)|((buffer[30+13]&LSB6)<<19);
    leds(3,red,blue,green);
    //LAYER 4
    red = ((buffer[0+12]&(~LSB4)))|(buffer[0+13]<<4)|(buffer[0+14]<<12)|((buffer[0+15]&LSB5)<<20);
    blue = ((buffer[0+28]&(~LSB1)))|(buffer[0+29]<<7)|(buffer[30]<<15)|((buffer[30+1]&LSB2)<<23);
    green = ((buffer[30+13]&(~LSB6)))|(buffer[30+14]<<2)|(buffer[30+15]<<10)|((buffer[30+15]&LSB7)<<18);
    leds(4,red,blue,green);
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t temp = 0;
    for(i=0;i<5;i++){
        level(i);
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
}


int8_t Receive(uint8_t *buffer,uint8_t *receive_buffer,uint8_t buffersize){
        uint8_t i = 0;
		while (!mirf_data_ready()){
            test_cube(buffer);
            //i = (i+1) % 5;
            //if(i > 0x1FFFF){
                //LED1_ON;
            //    return -1;
           // }
            //i++;
        }
		mirf_get_data(receive_buffer);
        //test_cube(buffer);
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

void init(){
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
}
void init2(uint8_t *buffer){
	// Configure mirf
	mirf_config();
}

uint8_t sensors(){
    return (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
}


int main (void)
{
    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
    init();
    uint8_t temp_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t receive_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t buffer [64] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint16_t i = 0;
	sei();
    init2(receive_buffer);
    uint16_t counter = 0;
	
	while (1)
	{
        /*
        //static uint32_t layer = 0;
        static uint8_t layer = 0;
        static uint32_t red = 0x02;
        static uint32_t blue = 1;
        static uint32_t green = 1;
        leds(layer,red,red,red);
        //red = red<<1;
        //if(red == 0){
        //    red = 1;
       // }
        layer = (layer+1)%5;
        uint8_t i = 0;
        uint8_t j = 0;
        uint8_t temp = 0;
        for(i=0;i<5;i++){
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
                   // _delay_ms(2);
                }
                else if(4 == i){
                    //_delay_us(100);
                }
                else{
                   // _delay_ms(500);
                }
            if(i == (layer-1)){
            _delay_ms(500);
            }
        }
        //_delay_ms(500);
        */
        if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
            //translate(buffer);
            if((ID_SELF == receive_buffer[0])&&(PACKET1 == receive_buffer[1])){
                for(i = 2; i < BUFFER_SIZE;i++){
                    buffer[i-2] = receive_buffer[i];
                }
                //test_cube(buffer);
                receive_buffer[0] = ID_SELF;
                receive_buffer[1] = ACK;
                receive_buffer[2] = SENSORS;
                counter = 0;
                while (mirf_send (receive_buffer, BUFFER_SIZE) && counter < 1000)
                {
                    _delay_us(10);
                }
                if (counter >= 1000)
                {
                    //transmit_string("e");
                }
            }
            if((receive_buffer[0] == ID_SELF)&&(receive_buffer[1] == PACKET2)){
                for(i = 34; i < 64; i++){
                    buffer[i-2] = receive_buffer[i-32];
                }

                receive_buffer[0] = ID_SELF;
                receive_buffer[1] = ACK;
                receive_buffer[2] = SENSORS;
                Transmit(receive_buffer,BUFFER_SIZE);
            }
        }
    }

}
