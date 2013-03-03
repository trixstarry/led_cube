// ledcube.c 
// Doug Dziggel
// 2.10.13
// Code for LED Cube microcontroller - controls LEDs, Radio board and reads hall effect sensors.


// Expected Connections:
// LED Driver Pinout     atTiny167
// -----------------    ----------------------    
// P1		GND			GND
// P3		CLK         PORTB bit 1 (sclk)
// P2		SIN         PORTB bit 2 (mosi)
// P4		LATCH		PORTB bit 7 
// P5-12	LED1-8
// P13		VDD			VCC
// P14		RSET		22K Ohm to GND
// P15		SOUT		FLOAT
// P16		BLANK		GND
// P17-24	LED16-9
//
// Expected Connections:
// Radio Board Pinout    atTiny167
// -----------------    ----------------------    
// P1		GND			GND
// P3		CLK         PORTB bit 1 (sclk)
// P2		SIN         PORTB bit 2 (mosi)
// P4		LATCH		PORTB bit 7 
// P5-12	LED1-8
// P13		VDD			VCC
// P14		RSET		22K Ohm to GND
// P15		SOUT		FLOAT
// P16		BLANK		GND
// P17-24	LED16-9
//
// Expected Connections:
// Hall Effect Sensors    LED board or mega128
// -------------------    ----------------------    
// Sensor1				      PORTB Bit 0
// Sensor2	                  PORTB bit 1 
// Sensor3		              PORTB bit 2
// Sensor4		    		  PORTB bit 3 


#define TRUE 1
#define FALSE 0
#define F_CPU 8000000UL	// 8Mhz clock
#define BAUD 250000

#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400

#define SS PA6
#define MOSI PA4
#define SCK PA5

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "spi.h"
#include <avr/interrupt.h>
#include "ledcube.h"



uint16_t frame[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; //5 layers and 5 led drivers
uint8_t transmit_buffer[32] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',};
uint8_t receive_buffer[32] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',};
uint8_t next = 0;

void initialize_interrupts(void){
    TIMSK1 = (1<<TOIE1);
    }

/***********************************************************************/
//                            spi_init                               
//Initalizes the SPI port on the mega128. Does not do any further   
//external device specific initalizations.  Sets up SPI to be:                        
//master mode, clock=clk/2, cycle half phase, low polarity, MSB first
//interrupts disabled, poll SPIF bit in SPSR to check xmit completion
/***********************************************************************/
/*
void spi_init(void){
    //
	DDRB  |=   (1<<SS_N)|(1<<MOSI)|(1<<SCK);          //Turn on SS, MOSI, SCLK, MISO
	SPCR  |=   (1<<SPE)|(1<<MSTR);           //set up SPI mode
	//Bit 7 = SPIE = SPI interrupt enable   
	//Bit 6 = SPE Enable, 
	//Bit 5 = DORD = Data Order, 0 = MSB transmitted first
	//Bit 4 = MSTR = Master/Slave Select = Master/slave select
	//Bit 3 = C1POL = Clock polarity (0 means low polarity)
	//Bit 2 = C1PHA = Clock phase
	//Bits 1, 0 = SPR1, SPR0 =  
	//SPSR  |=   (1<<SPI2X);           // double speed operation
    //

    //ATtiny167 SPI INIT
	DDRA |= (1<<SS)|(1<<MOSI)|(1<<SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(0<<SPR0);
	SPSR |= (1<<SPI2X);

}//spi_init
*/


/***********************************************************************/
//                              tcnt0_init                             
//Initalizes timer/counter0 (TCNT0). TCNT0 is running in async mode
//with external 32khz crystal.  Runs in normal mode with no prescaling.
//Interrupt occurs at overflow 0xFF.
//
/*
void tcnt0_init(void){
	ASSR   |=  (1<<AS0);    //ext osc TOSC
	TIMSK  |=  (1<<TOIE0);  //enable timer/counter0 overflow interrupt
	TCCR0  |=  (1<<CS00);   //normal mode, no prescale
}
*/

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

void transmit1(uint16_t data1,uint16_t data2,uint16_t data3,uint16_t data4,uint16_t data5){
	//break the data up into 4 bytes
	
	uint8_t temp = 0;

	temp = (data1);
	//load first byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data1>>8);
	//load second byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data2);
	//load third byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data2>>8);
	//load fourth byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data3);
	//load first byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data3>>8);
	//load second byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data4);
	//load third byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data4>>8);
	//load fourth byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data5);
	//load first byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data5>>8);
	//load second byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
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
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
    _delay_ms(3);
    //_delay_us(300);
}


void level_test(void){
    uint16_t on = 0xFFFF;
    uint16_t off = 0x0000;
    uint8_t i = 0;
    for(i = 0; i<5;i++){

        switch(i){
            case 0:
                level(i);
                transmit1(on,on,on,on,on);
                break;
            case 1:
                level(i);
                transmit1(on,on,on,on,on);
                break;
            case 2:
                level(i);
                transmit1(on,on,on,on,on);
                break;
            case 3:
                level(i);
                transmit1(on,on,on,on,on);
                break;
            case 4:
                level(i);
                transmit1(on,on,on,on,on);
                break;
        }
    }
}


void shift_LED(void){
    static uint16_t LED1 = 0x0001;
    static uint16_t LED2 = 0x0000;
    static uint16_t LED3 = 0x0000;
    static uint16_t LED4 = 0x0000;
    static uint16_t LED5 = 0x0000;

    static uint8_t first1 = 1;
    static uint8_t first2 = 1;
    static uint8_t first3 = 1;
    static uint8_t first4 = 1;
    static uint8_t first5 = 1;

    static uint8_t layer = 0;

    level(layer);
    //level(0);


    if(LED1 == 0)
    {
        if(first2){
            LED2 = 0x0001;
            first2 = 0;
        }
        if(~first2 && LED2){
            LED2 = LED2 << 1;
        }
        else{
            if(first3){
                LED3= 0x0001;
                first3 = 0;
            }
            if(~first3 && LED3){
                LED3 = LED3 << 1;
            }
            else{
                if(first4){
                    LED4= 0x0001;
                    first4 = 0;
                }
                if(~first4 && LED4){
                    LED4 = LED4 << 1;
                }
                else{
                    if(first5){
                        LED5= 0x0001;
                        first5 = 0;
                    }
                    if(~first5 && LED5){
                        LED5 = LED5 << 1;
                    }
                    else{
                        LED1 = 0x0001;
                        first1 = first2 = first3 = first4 = first5 = 1;
                        layer++;
                        layer = layer % 5;
                    }
                }
            }
        }
    }
    else
    {
        if(first1){
            first1 = 0;
        }
        else{
            LED1 = LED1 << 1;
        }
    }

    transmit1(LED5,LED4,LED3,LED2,LED1);
    //transmit1(LED1,LED2,LED3,LED4,LED5);
    _delay_ms(50);
    _delay_ms(500);
    }

void hall_test(void){
    uint8_t input = 0;
    while(1){
        input = (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
        //transmit1(C1P3,0,0,0,0);
        //level(1);
        switch(input){
            case 0:
                level(0);
                transmit1(0,0,C1P3,C1P2,0);
                break;
            case 1:
                level(1);
                transmit1(0,0,C1P3,C1P2,0);
                break;
            case 2:
                level(2);
                transmit1(0,0,C1P3,C1P2,0);
                break;
            case 4:
                level(3);
                transmit1(0,0,C1P3,C1P2,0);
                break;
            case 8:
                level(4);
                transmit1(0,0,C1P3,C1P2,0);
                break;
            default:
                level(0);
                transmit1(C1P2,0,0,0,0);
                
    }

}
}

void red(uint8_t position){
    switch(position){
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P8;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 2:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P7;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 3:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P15;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 4:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P14;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 5:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P9;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 6:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P11;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 7:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P3;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 8:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P3;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 9:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P8;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 10:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P7;
            frame[0][4] = 0x0000;
            break;
        case 11:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P14;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 12:
            frame[0][0] = C1P6;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 13:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P11;
            frame[0][4] = 0x0000;
            break;
        case 14:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P6;
            frame[0][4] = 0x0000;
            break;
        case 15:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P3;
            frame[0][4] = 0x0000;
            break;
        case 16:
            frame[0][0] = C1P7;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 17:
            frame[0][0] = C1P3;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 18:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P14;
            frame[0][4] = 0x0000;
            break;
        case 19:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P13;
            break;
        case 20:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P12;
            break;
        case 21:
            frame[0][0] = C1P10;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 22:
            frame[0][0] = C1P16;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 23:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P3;
            break;
        case 24:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P4;
            break;
        case 25:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P9;
            break;
        default:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;

    }


}

void green(uint8_t position){
    switch(position){
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P9;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 2:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P6;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 3:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P16;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 4:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P13;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 5:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P10;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 6:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P12;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 7:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P2;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 8:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P4;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 9:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P7;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 10:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P8;
            frame[0][4] = 0x0000;
            break;
        case 11:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P15;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 12:
            frame[0][0] = C1P5;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 13:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P12;
            frame[0][4] = 0x0000;
            break;
        case 14:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P5;
            frame[0][4] = 0x0000;
            break;
        case 15:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P2;
            frame[0][4] = 0x0000;
            break;
        case 16:
            frame[0][0] = C1P8;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 17:
            frame[0][0] = C1P2;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 18:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P15;
            frame[0][4] = 0x0000;
            break;
        case 19:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P14;
            break;
        case 20:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P11;
            break;
        case 21:
            frame[0][0] = C1P11;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 22:
            frame[0][0] = C1P15;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 23:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P2;
            break;
        case 24:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P5;
            break;
        case 25:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P8;
            break;
        default:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;

    }

}

void blue(uint8_t position){
    switch(position){
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P10;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 2:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P5;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 3:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P1;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 4:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P12;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 5:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P11;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 6:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P13;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 7:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P1;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 8:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P5;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 9:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = C3P6;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 10:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P9;
            frame[0][4] = 0x0000;
            break;
        case 11:
            frame[0][0] = 0x0000;
            frame[0][1] = C2P16;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 12:
            frame[0][0] = C1P4;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 13:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P13;
            frame[0][4] = 0x0000;
            break;
        case 14:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P4;
            frame[0][4] = 0x0000;
            break;
        case 15:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P1;
            frame[0][4] = 0x0000;
            break;
        case 16:
            frame[0][0] = C1P9;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 17:
            frame[0][0] = C1P1;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 18:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = C4P16;
            frame[0][4] = 0x0000;
            break;
        case 19:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P15;
            break;
        case 20:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P10;
            break;
        case 21:
            frame[0][0] = C1P12;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 22:
            frame[0][0] = C1P14;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 23:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P1;
            break;
        case 24:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P6;
            break;
        case 25:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = C5P7;
            break;
        default:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;

    }
}

// NOTE: Colors are
//      0  Red
//      1  Green
//      2  Blue
// Layers can be 0 - 4
// positions can be 1 - 25
void LED_test(uint8_t color, uint8_t position,uint8_t layer){
        level(layer);
        switch(color){
            case 0:
                red(position);
                break;
            case 1:
                green(position);
                break;
            case 2:
                blue(position);
                break;
            default:
                break;
        }
        transmit1(frame[0][4],frame[0][3],frame[0][2],frame[0][1],frame[0][0]);

}

void test_led(){
    static uint8_t i = 0;
    static uint8_t j = 0;
    static uint8_t layer = 0;
    for(i=0;i<3;i++){
        LED_test(i,j,layer);
        if(i == 2){
            j++;
            if(j == 26){
                j = 1;
                layer++;
                layer = layer % 5;
            }
        }
        _delay_ms(500);
    }
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

void clear_frame(){
    uint8_t i = 0;
    uint8_t j = 0;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            frame[i][j] = 0;
        }
    } 
    }

void test_pattern(){
    uint32_t plane = 0x1FFFFFF;
    uint32_t row4 = 0x1F00000;
    uint32_t row3 = 0xF8000;
    uint32_t row2 = 0x7C00;
    uint32_t row1 = 0x3E0;
    uint32_t row0 = 0x1F;
    uint16_t counter = 0;
    while(1){
        while(counter <0x0018){
            leds(0,row0,0,0);
            leds(1,0,row0,0);
            leds(2,0,0,row0);
            leds(3,0,0,row0);
            leds(4,row0,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row1,0,0);
            leds(1,0,row1,0);
            leds(2,0,0,row1);
            leds(3,0,0,row1);
            leds(4,row1,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row2,0,0);
            leds(1,0,row2,0);
            leds(2,0,0,row2);
            leds(3,0,0,row2);
            leds(4,row2,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter< 0x0018){
            leds(0,row3,0,0);
            leds(1,0,row3,0);
            leds(2,0,0,row3);
            leds(3,0,0,row3);
            leds(4,row3,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter < 0x0018){
            leds(0,row4,0,0);
            leds(1,0,row4,0);
            leds(2,0,0,row4);
            leds(3,0,0,row4);
            leds(4,row4,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter< 0x0018){
            leds(0,row3,0,0);
            leds(1,0,row3,0);
            leds(2,0,0,row3);
            leds(3,0,0,row3);
            leds(4,row3,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row2,0,0);
            leds(1,0,row2,0);
            leds(2,0,0,row2);
            leds(3,0,0,row2);
            leds(4,row2,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row1,0,0);
            leds(1,0,row1,0);
            leds(2,0,0,row1);
            leds(3,0,0,row1);
            leds(4,row1,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,plane,0,0);
            leds(1,0,plane,0);
            leds(2,0,0,plane);
            leds(3,0,0,plane);
            leds(4,plane,0,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row0,0,0);
            leds(1,0,row0,0);
            leds(2,0,0,row0);
            leds(3,0,0,row0);
            leds(4,row0,0,0);
            test_frame();
            counter++;
            
        }
    }
}

void test_pattern1(){
    uint32_t plane = 0x1FFFFFF;
    uint32_t row4 = 0x1F00000;
    uint32_t row3 = 0xF8000;
    uint32_t row2 = 0x7C00;
    uint32_t row1 = 0x3E0;
    uint32_t row0 = 0x1F;
    uint16_t counter = 0;
    while(1){
        while(next == 0){
            leds(0,row0,0,0);
            leds(1,0,row0,0);
            leds(2,0,0,row0);
            leds(3,0,0,row0);
            leds(4,0,row0,0);
            test_frame();
            //counter++;
        }
        next = 1;
        //counter = 0;
        while(next == 0){
            leds(0,row1,0,0);
            leds(1,0,row1,0);
            leds(2,0,0,row1);
            leds(3,0,0,row1);
            leds(4,0,row1,0);
            test_frame();
            //counter++;
        }
        //counter = 0;
        next = 1;
        while(counter <0x0018){
            leds(0,row2,0,0);
            leds(1,0,row2,0);
            leds(2,0,0,row2);
            leds(3,0,0,row2);
            leds(4,0,row2,0);
            test_frame();
            //counter++;
        }
        counter = 0;
        while(counter< 0x0018){
            leds(0,row3,0,0);
            leds(1,0,row3,0);
            leds(2,0,0,row3);
            leds(3,0,0,row3);
            leds(4,0,row3,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter < 0x0018){
            leds(0,row4,0,0);
            leds(1,0,row4,0);
            leds(2,0,0,row4);
            leds(3,0,0,row4);
            leds(4,0,row4,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter< 0x0018){
            leds(0,row3,0,0);
            leds(1,0,row3,0);
            leds(2,0,0,row3);
            leds(3,0,0,row3);
            leds(4,0,row3,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row2,0,0);
            leds(1,0,row2,0);
            leds(2,0,0,row2);
            leds(3,0,0,row2);
            leds(4,0,row2,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row1,0,0);
            leds(1,0,row1,0);
            leds(2,0,0,row1);
            leds(3,0,0,row1);
            leds(4,0,row1,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,plane,0,0);
            leds(1,0,plane,0);
            leds(2,0,0,plane);
            leds(3,0,0,plane);
            leds(4,0,plane,0);
            test_frame();
            counter++;
        }
        counter = 0;
        while(counter <0x0018){
            leds(0,row0,0,0);
            leds(1,0,row0,0);
            leds(2,0,0,row0);
            leds(3,0,0,row0);
            leds(4,0,row0,0);
            test_frame();
            if(counter >= 0x00F0){
             break;}
            counter++;
        }
    }
}

void Transmit(){
        //PORTB = (1<<PB0);
		transmit_buffer[31]++;
		if (transmit_buffer[31] < ' ' || transmit_buffer[31] > 'z')
		{
			transmit_buffer[31] = ' ';
		}
    	mirf_send(transmit_buffer,32);
        //mirf_send(testbuffer,testbuffersize);
        //
		_delay_ms(10);
}

ISR(TIMER1_OVF_vect){
//
    static uint16_t counter = 0;
    if(counter >= 0x08){
        next = 1;
        counter = 0;
    }

}

void test_xbee_pins(){

    PORTA |= ((1<<PA0)|(1<<PA1));
    _delay_ms(500);
    PORTA |= ((0<<PA0)|(0<<PA1));

}

void probe_test(){
        LED_test(0,4,0);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
        //_delay_ms(500);
}

/***********************************************************************/
//                                main                                 
/***********************************************************************/
int main(){

    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	spi_init();    //initalize SPI port
    //initialize_interrupts();
	// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
    sei();
	mirf_read_register(STATUS, receive_buffer, 1);
	mirf_config();
   
	while(1){
        //test_xbee_pins();
        //Transmit();
        test_pattern();
        //test_pattern1();
        //test_frame();
        //level_test();
        //test_led();
        //probe_test();
        //shift_LED();
        //hall_test();

    }     //empty main while loop
} //main
