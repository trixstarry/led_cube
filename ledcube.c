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

/*
#define MISO PB3 // Master in, Slave out: configure as input (with pullup)
#define MOSI PB2 // Master out, Slave in: configure as output
#define SCK  PB1 // SPI clock: configure as output
#define SS_N PB0 // Slave Select: Configure to output
*/

#define TRUE 1
#define FALSE 0
#define F_CPU 8000000		// 8 MHz cpu speed
#define LEVELS 5			// the LED array has 5 levels
#define PIN_NUM 5*5*3		// each level has 5x5 LEDs with 3 pins per LED
#define HALL_NUM 4			// there are 4 hall effect sensors per cube

#define SS PA6
#define MOSI PA4
#define SCK PA5
//#define CUBE_ID 1			// an ID to identify this cube

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//#include "ledcube.h"

// ************************************************************
//                      ledcube.h
//  This contains most of the led cube defines including all
//  the LED color positions and layer defines
//

#define C1P1     0x0400
#define C1P2     0x0200
#define C1P3     0x0100
#define C1P4     0x2000
#define C1P5     0x1000
#define C1P6     0x0800
#define C1P7     0x0001
#define C1P8     0x8000
#define C1P9     0x4000
#define C1P10    0x0008
#define C1P11    0x0004
#define C1P12    0x0002
#define C1P13    0x0010
#define C1P14    0x0080
#define C1P15    0x0040
#define C1P16    0x0020

#define C2P1     C1P1
#define C2P2     C1P2
#define C2P3     C1P3
#define C2P4     C1P6
#define C2P5     C1P9
#define C2P6     C1P4
#define C2P7     C1P5  //pin 7
#define C2P8     C1P12 //pin 8
#define C2P9     C1P7  //pin 9
#define C2P10    C1P8  //pin 10
#define C2P11    C1P13 
#define C2P12    C1P10
#define C2P13    C1P11 
#define C2P14    C1P14 
#define C2P15    C1P15
#define C2P16    C1P16

#define C3P1     C1P15   // 1 = 5
#define C3P2     C1P2   // 2 = 2
#define C3P3     C1P5   // 3 = 15
#define C3P4     C1P6   // 4 = 8
#define C3P5     C1P1   // 5 = 3
#define C3P6     C1P8   // 6 = 4
#define C3P7     C1P9   // 7 = 11
#define C3P8     C1P4  // 8 = 6
#define C3P9     C1P11   // 9 = 7
#define C3P10    C1P12  //10 = 14
#define C3P11    C1P7  //11 = 9
#define C3P12    C1P16 //12 = 10
#define C3P13    C1P13  //13 = 13
#define C3P14    C1P10  //14 = 16
#define C3P15    C1P3  //15 = 1
#define C3P16    C1P14  //16 = 12

#define C4P1     C1P1   // 1 = 1
#define C4P2     C1P2   // 2 = 2
#define C4P3     C1P3   // 3 = 3
#define C4P4     C1P4   // 4 = 4
#define C4P5     C1P5   // 5 = 5
#define C4P6     C1P6   // 6 = 6
#define C4P7     C1P7   // 7 = 7
#define C4P8     C1P8  // 8 = 8
#define C4P9     C1P9   // 9 = 9
#define C4P10    C1P12  //10 = 11 blue 11
#define C4P11    C1P13  //11 = 12 nc 12
#define C4P12    C1P10 //12 = 10  Green 10
#define C4P13    C1P11  //13 = 14 red 13
#define C4P14    C1P14  //14 = 14
#define C4P15    C1P15  //15 = 15
#define C4P16    C1P16  //16 = 16

#define C5P1     C1P1  // 1 = 1
#define C5P2     C1P2   // 2 = 2
#define C5P3     C1P3   // 3 = 3
#define C5P4     C1P4   // 4 = 4
#define C5P5     C1P5   // 5 = 5
#define C5P6     C1P6   // 6 = 6
#define C5P7     C1P7   // 7 = 7
#define C5P8     C1P8   // 8 = 8
#define C5P9     C1P9  // 9 = 9
#define C5P10    C1P10  //10 = 10
#define C5P11    C1P11  //11 = 11
#define C5P12    C1P12  //12 = 12
#define C5P13    C1P15  //13 = 15
#define C5P14    C1P16  //14 = 16
#define C5P15    C1P13   //15 = 13
#define C5P16    C1P14  //16 = 14

//#define R1 {0,C2P8,0,0,0}


uint16_t R[25][5] PROGMEM = {   {0    ,C2P8 ,0    ,0    ,0    }, //1
                        {0    ,C2P7 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P15,0    ,0    }, //3
                        {0    ,0    ,C3P14,0    ,0    }, //4
                        {0    ,0    ,C3P9 ,0    ,0    }, //5
                        {0    ,C2P11,0    ,0    ,0    }, //6
                        {0    ,C2P3 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P3 ,0    ,0    }, //8
                        {0    ,0    ,C3P8 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P7 ,0    }, //10
                        {0    ,C2P14,0    ,0    ,0    },
                        {C1P6 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P11,0    },
                        {0    ,0    ,0    ,C4P6 ,0    },
                        {0    ,0    ,0    ,C4P3 ,0    }, //15
                        {C1P7 ,0    ,0    ,0    ,0    },
                        {C1P3 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P14,0    },
                        {0    ,0    ,0    ,0    ,C5P13},
                        {0    ,0    ,0    ,0    ,C5P12}, //20
                        {C1P10,0    ,0    ,0    ,0    },
                        {C1P16,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P3},
                        {0    ,0    ,0    ,0    ,C5P4},
                        {0    ,0    ,0    ,0    ,C5P9} //25
};
uint16_t G[25][5] PROGMEM = {   {0    ,C2P9 ,0    ,0    ,0    }, //1
                        {0    ,C2P6 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P16,0    ,0    }, //3
                        {0    ,0    ,C3P13,0    ,0    }, //4
                        {0    ,0    ,C3P10,0    ,0    }, //5
                        {0    ,C2P12,0    ,0    ,0    }, //6
                        {0    ,C2P2 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P4 ,0    ,0    }, //8
                        {0    ,0    ,C3P7 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P8 ,0    }, //10
                        {0    ,C2P15,0    ,0    ,0    },
                        {C1P5 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P12,0    },
                        {0    ,0    ,0    ,C4P5 ,0    },
                        {0    ,0    ,0    ,C4P2 ,0    }, //15
                        {C1P8 ,0    ,0    ,0    ,0    },
                        {C1P2 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P15,0    },
                        {0    ,0    ,0    ,0    ,C5P14},
                        {0    ,0    ,0    ,0    ,C5P11}, //20
                        {C1P11,0    ,0    ,0    ,0    },
                        {C1P15,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P2 },
                        {0    ,0    ,0    ,0    ,C5P5 },
                        {0    ,0    ,0    ,0    ,C5P8 } //25
};
uint16_t B[25][5] PROGMEM = {   {0    ,C2P10,0    ,0    ,0    }, //1
                        {0    ,C2P5 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P1 ,0    ,0    }, //3
                        {0    ,0    ,C3P12,0    ,0    }, //4
                        {0    ,0    ,C3P11,0    ,0    }, //5
                        {0    ,C2P13,0    ,0    ,0    }, //6
                        {0    ,C2P1 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P5 ,0    ,0    }, //8
                        {0    ,0    ,C3P6 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P9 ,0    }, //10
                        {0    ,C2P16,0    ,0    ,0    },
                        {C1P4 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P13,0    },
                        {0    ,0    ,0    ,C4P4 ,0    },
                        {0    ,0    ,0    ,C4P1 ,0    }, //15
                        {C1P9 ,0    ,0    ,0    ,0    },
                        {C1P1 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P16,0    },
                        {0    ,0    ,0    ,0    ,C5P15},
                        {0    ,0    ,0    ,0    ,C5P10}, //20
                        {C1P12,0    ,0    ,0    ,0    },
                        {C1P14,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P1 },
                        {0    ,0    ,0    ,0    ,C5P6 },
                        {0    ,0    ,0    ,0    ,C5P7 } //25
};

//global variables
//uint16_t layer0 = 0x0000;
//uint16_t layer1 = 0x0000;
//uint16_t layer2 = 0x0000;
//uint16_t layer3 = 0x0000;
//uint16_t layer4 = 0x0000;
//uint8_t frame[5] = {layer0,layer1,layer2,layer3,layer4};


uint16_t frame[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; //5 layers and 5 led drivers

/***********************************************************************/
//                            spi_init                               
//Initalizes the SPI port on the mega128. Does not do any further   
//external device specific initalizations.  Sets up SPI to be:                        
//master mode, clock=clk/2, cycle half phase, low polarity, MSB first
//interrupts disabled, poll SPIF bit in SPSR to check xmit completion
/***********************************************************************/
void spi_init(void){
    /*
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
    */

    //ATtiny167 SPI INIT
	DDRA |= (1<<SS)|(1<<MOSI)|(1<<SCK);
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR |= (1<<SPI2X);

}//spi_init


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
	//break the data up into 4 bytes
	
	uint8_t temp = 0;
    uint8_t i = 0;
    for(i=0;i<5;i++){
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
    }

	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}

void level(uint8_t layer){
    switch(layer){
        case 2:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(0<<PB5)|(0<<PB6));
            return;
        case 1:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((1<<PB4)|(0<<PB5)|(0<<PB6));
            return;
        case 0:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(1<<PB5)|(0<<PB6));
            return;
        case 4:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((1<<PB4)|(1<<PB5)|(0<<PB6));
            return;
        case 3:
            PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6));
            PORTB |= ((0<<PB4)|(0<<PB5)|(1<<PB6));
            return;
        default:
            PORTB |= ((1<<PB4)|(1<<PB5)|(1<<PB6));
            }
}


void level_test(void){
    uint16_t data1 = 0x00FF;
    uint16_t data2 = 0x00FF;
    uint16_t data3 = 0x00FF;
    uint16_t data4 = 0x00FF;
    uint16_t data5 = 0x00FF;
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
                frame[layer][i] |= pgm_read_byte(&(G[index][i]));
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
        transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        level(i);
        _delay_ms(500);
        _delay_ms(500);
        _delay_ms(500);
        _delay_ms(500);
        _delay_ms(3);
        _delay_us(300);
    }
}

/***********************************************************************/
//                                main                                 
/***********************************************************************/
int main(){

    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	spi_init();    //initalize SPI port
    uint8_t input = 0;
    uint8_t i = 0;
    uint32_t color_on = 0x1FFFFFF;
   
    //initialize the pattern
    leds(0,0,color_on,0);
    leds(1,0,color_on,0);
    leds(2,0,0,color_on);
    leds(3,color_on,0,0);
    leds(4,0,color_on,0);

	while(1){

        //test_frame();
        //level(i);
        //transmit1(frame[i][4],frame[i][3],frame[i][2],frame[i][1],frame[i][0]);
        level(4);
        transmit1(pgm_read_word(&(G[i][4])),pgm_read_word(&(G[i][3])),pgm_read_word(&(G[i][2])),pgm_read_word(&(G[i][1])),pgm_read_word(&(G[i][0])));
        i = (i+1) % 25;
        _delay_ms(500);
        //level_test();
        //shift_LED();
        //level(0);
        //transmit1(0xFF00,0xFF00,0xFF00,0xFF00,0xFF00);
        /*
        SPDR = 0x00;
        while(bit_is_clear(SPSR,SPIF)){};
        SPDR = 0x88;
        while(bit_is_clear(SPSR,SPIF)){};
        */
        //hall_test();

    }     //empty main while loop
} //main
