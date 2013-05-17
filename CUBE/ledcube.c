//************************************************************************\\
// ledcube.c
//
//  Oregon State University Senior Design Project
//  Project: Multiple LED Cube Controller
//  Group: 2
// 	Author:	Doug Dziggel
// 	Date:	5.6.13
//
// Description:
// This is the program for the LED Cubes. Each Cube is programmed with
// a separate cube ID and a separate channel.
//
// Currently the patterns are hardcoded and the NRF24L01 is very fragile
// for the LED Cube. Interesting to note that it never breaks on the USB
// end.
//************************************************************************//

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

#define DATA_START 12

#define CUBE1   0x01
#define CUBE2   0x02
#define CUBE3   0x04
#define CUBE4   0x08
#define ID_SELF CUBE3
#define PACKET1 0x04
#define PACKET2 0x08

#define ACK     0x11
#define DATA_S    0x22
#define NACK    0x33
#define NO_RESPONSE 0x00
#define RESPONSE 0x01

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

#define PATTERN1 1
#define PATTERN2 2
#define PATTERN3 3
#define PATTERN4 4
#define PATTERN5 5
#define PATTERNS 6

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"
#include "ledcube.h"

uint16_t frame[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; //5 layers and 5 led drivers
uint32_t color[3] = {0,0,0};
//uint32_t current[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
uint8_t updated = 0;
uint8_t clear = 0;
uint8_t item = 0;
uint8_t frame_num = 0;


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

/*
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
*/

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


/*
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
*/

void pattern(uint8_t selection,uint8_t num){
    uint8_t red = 0;
    uint8_t blue = 1;
    uint8_t green = 2;

    
    //Load Pattern into frame
    //Appears frame is not cleared when loaded to drivers. Create a flag and only load a new frame from memory when
    //a new frame appears.
    leds(0,pgm_read_dword(&(patterns[num][0][red])),pgm_read_dword(&(patterns[num][0][blue])),pgm_read_dword(&(patterns[num][0][green])));
    leds(1,pgm_read_dword(&(patterns[num][1][red])),pgm_read_dword(&(patterns[num][1][blue])),pgm_read_dword(&(patterns[num][1][green])));
    leds(2,pgm_read_dword(&(patterns[num][2][red])),pgm_read_dword(&(patterns[num][2][blue])),pgm_read_dword(&(patterns[num][2][green])));
    leds(3,pgm_read_dword(&(patterns[num][3][red])),pgm_read_dword(&(patterns[num][3][blue])),pgm_read_dword(&(patterns[num][3][green])));
    leds(4,pgm_read_dword(&(patterns[num][4][red])),pgm_read_dword(&(patterns[num][4][blue])),pgm_read_dword(&(patterns[num][4][green])));

    //Insert a test

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
            frame[i][j] = 0; //Clears Frame after data is sent
                             // should change to also react to flag
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
            _delay_ms(500); //why delay?
        }
    }
}

void display(uint8_t *buffer){
    uint8_t red = 0;
    uint8_t blue = 1;
    uint8_t green = 2;

    if(updated == 1){
        //Layer 0
        color[0] = ((uint32_t)(buffer[3]))|((uint32_t)(buffer[2])<<8)|((uint32_t)(buffer[1])<<16)|((uint32_t)(buffer[0])<<24);
        color[1] = ((uint32_t)(buffer[3+20]))|((uint32_t)(buffer[2+20])<<8)|((uint32_t)(buffer[1+20])<<16)|((uint32_t)(buffer[0+20])<<24);
        color[2] = ((uint32_t)(buffer[3+40]))|((uint32_t)(buffer[2+40])<<8)|((uint32_t)(buffer[1+40])<<16)|((uint32_t)(buffer[0+40])<<24);
        leds(0,color[0],color[1],color[2]);

        //Layer 1
        color[0] = ((uint32_t)(buffer[7]))|((uint32_t)(buffer[6])<<8)|((uint32_t)(buffer[5])<<16)|((uint32_t)(buffer[4])<<24);
        color[1] = ((uint32_t)(buffer[7+20]))|((uint32_t)(buffer[6+20])<<8)|((uint32_t)(buffer[5+20])<<16)|((uint32_t)(buffer[4+20])<<24);
        color[2] = ((uint32_t)(buffer[7+40]))|((uint32_t)(buffer[6+40])<<8)|((uint32_t)(buffer[5+40])<<16)|((uint32_t)(buffer[4+40])<<24);
        leds(1,color[0],color[1],color[2]);

        //Layer 2
        color[0] = ((uint32_t)(buffer[11]))|((uint32_t)(buffer[10])<<8)|((uint32_t)(buffer[9])<<16)|((uint32_t)(buffer[8])<<24);
        color[1] = ((uint32_t)(buffer[11+20]))|((uint32_t)(buffer[10+20])<<8)|((uint32_t)(buffer[9+20])<<16)|((uint32_t)(buffer[8+20])<<24);
        color[2] = ((uint32_t)(buffer[11+40]))|((uint32_t)(buffer[10+40])<<8)|((uint32_t)(buffer[9+40])<<16)|((uint32_t)(buffer[8+40])<<24);
        leds(2,color[0],color[1],color[2]);

        //Layer 3
        color[0] = ((uint32_t)(buffer[15]))|((uint32_t)(buffer[14])<<8)|((uint32_t)(buffer[13])<<16)|((uint32_t)(buffer[12])<<24);
        color[1] = ((uint32_t)(buffer[15+20]))|((uint32_t)(buffer[14+20])<<8)|((uint32_t)(buffer[13+20])<<16)|((uint32_t)(buffer[12+20])<<24);
        color[2] = ((uint32_t)(buffer[15+40]))|((uint32_t)(buffer[14+40])<<8)|((uint32_t)(buffer[13+40])<<16)|((uint32_t)(buffer[12+40])<<24);
        leds(3,color[0],color[1],color[2]);

        //Layer 4
        color[0] = ((uint32_t)(buffer[19]))|((uint32_t)(buffer[18])<<8)|((uint32_t)(buffer[17])<<16)|((uint32_t)(buffer[16])<<24);
        color[1] = ((uint32_t)(buffer[19+20]))|((uint32_t)(buffer[18+20])<<8)|((uint32_t)(buffer[17+20])<<16)|((uint32_t)(buffer[16+20])<<24);
        color[2] = ((uint32_t)(buffer[19+40]))|((uint32_t)(buffer[18+40])<<8)|((uint32_t)(buffer[17+40])<<16)|((uint32_t)(buffer[16+40])<<24);
        leds(4,color[0],color[1],color[2]);
        updated = 0;
    }

    /*
    leds(0,current[0][0],current[0][1],current[0][2]);
    leds(0,current[1][0],current[1][1],current[1][2]);
    leds(0,current[2][0],current[2][1],current[2][2]);
    leds(0,current[3][0],current[3][1],current[3][2]);
    leds(0,current[4][0],current[4][1],current[4][2]);
    */

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
            if(clear == 1){
                frame[i][j] = 0; //Clears Frame after data is sent
            }
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
            _delay_ms(500); //why delay?
        }
    }
}

void translate(uint8_t *buffer){
    uint32_t red = 0;
    uint32_t blue = 0;
    uint32_t green = 0;


    red = (buffer[0])|((uint32_t)(buffer[0+1])<<8)|((uint32_t)(buffer[0+2])<<16)|((uint32_t)(buffer[0+3]&MSB1)<<24);
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

/*
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
*/

void Transmit(uint8_t *buffer, uint8_t buffersize){
        uint16_t counter = 0;
        while (mirf_send (buffer, BUFFER_SIZE) && counter < 1000)
        {
            _delay_us(10);
            counter++;
        }

        if (counter >= 1000)
        {
            uint8_t config  = 0;
            mirf_read_register (CONFIG, &config, 1);
            mirf_config_register(CONFIG, (config & ~(1<<PWR_UP)));
            _delay_us(4500);
            mirf_config_register(CONFIG, (config | (1<<PWR_UP)));
            _delay_us(4500);
            //transmit_string("e");
        }
}


int8_t Receive(uint8_t *buffer,uint8_t *receive_buffer,uint8_t buffersize){
        //uint8_t i = 0;
		while (!mirf_data_ready()){
            //test_cube(buffer);
            //pattern(item,frame_num);
            display(buffer);
            _delay_ms(2);
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


/*
void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[len-1]++;
    if (buffer[len-1] < ' ' || buffer[len-1] > 'z')
    {
        buffer[len-1] = ' ';
    }
    Transmit(buffer,len);
}
*/

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
    //uint8_t temp_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
    //    'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    uint8_t receive_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    //uint8_t frame_buffer [32] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
    //    'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    //uint32_t color = 0;
    uint8_t buffer [64] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F'};
    //uint8_t i = 0;
    uint16_t i = 0;
    
	sei();
    init2(receive_buffer);
    //uint16_t counter = 0;
    leds(0,1,0,0);
    leds(1,1,0,0);
    leds(2,1,0,0);
    leds(3,1,0,0);
    leds(4,1,0,0);
    for(i = 0; i<64;i++){
        buffer[i] = 0;
    }
	
	while (1)
	{
        if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
            if(ID_SELF == receive_buffer[0]){
                frame_num = receive_buffer[2];
                item = 1;

                for(i = 0; i<20;i++){
                    buffer[i] = receive_buffer[i+DATA_START];
                }

                //color[0] = (receive_buffer[8]) | ((uint32_t)(receive_buffer[7])<<8) | ((uint32_t)(receive_buffer[6])<<16) | ((uint32_t)(receive_buffer[5])<<24);
                if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
                    if(ID_SELF == receive_buffer[0]){
                        frame_num = receive_buffer[2];
                        item = 1;

                        for(i = 0; i<20;i++){
                            buffer[i+20] = receive_buffer[i+DATA_START];
                        }
                        //color[1] = (receive_buffer[8]) | ((uint32_t)(receive_buffer[7])<<8) | ((uint32_t)(receive_buffer[6])<<16) | ((uint32_t)(receive_buffer[5])<<24);
                        if(Receive(buffer,receive_buffer,BUFFER_SIZE) == 1){
                            if(ID_SELF == receive_buffer[0]){
                                frame_num = receive_buffer[2];
                                item = 1;

                                for(i = 0; i<20;i++){
                                    buffer[i+40] = receive_buffer[i+DATA_START];
                                }
                                clear = 1;
                                display(buffer);
                                clear = 0;
                                updated = 1;
                                //color[1] = (receive_buffer[8]) | ((uint32_t)(receive_buffer[7])<<8) | ((uint32_t)(receive_buffer[6])<<16) | ((uint32_t)(receive_buffer[5])<<24);
                                //color += ((receive_buffer[6]) + (receive_buffer[5]<<8)<<16);
                                //color += ((receive_buffer[6]) + (receive_buffer[5]<<8)<<16);
                            }
                        }
                    }
                }

                receive_buffer[0] = ID_SELF;
                receive_buffer[1] = ACK;
                receive_buffer[2] = SENSORS;
                //Transmit(receive_buffer,BUFFER_SIZE);
                if (receive_buffer[3] == RESPONSE){
                    Transmit(receive_buffer,BUFFER_SIZE);
                }
                //Transmit(receive_buffer,BUFFER_SIZE);
            }
            
        }
    }

}
