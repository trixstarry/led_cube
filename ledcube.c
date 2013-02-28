// bar_graph_demo_skel.c 
// R. Traylor
// 10.20.09
// demos interrups, counter timer and SPI

// This code implements a timer interrupt to update the bar graph display
// at 0.5 second intervals. Every half second, a new data value to the
// bargraph vis SPI.  The value displayed is a single led on in a climbing
// pattern.

// Expected Connections:
// LED Driver Pinout    LED board or mega128
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
//#define CUBE_ID 1			// an ID to identify this cube

#include <avr/io.h>
#include <util/delay.h>


#define SS PA6
#define MOSI PA4
#define SCK PA5

// *********************************
// LAYER DEFINITIONS
// *********************************
#define LAYER_1() \
                PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6)); \
                PORTB |= ((0<<PIN4)|(0<<PIN5)|(0<<PIN6));

#define LAYER_2 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6));  \
                PORTB |= ((1<<PIN4)|(0<<PIN5)|(0<<PIN6));

#define LAYER_3 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6));  \
                PORTB |= ((0<<PIN4)|(1<<PIN5)|(0<<PIN6));

#define LAYER_4 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6));  \
                PORTB |= ((1<<PIN4)|(j<<PIN5)|(0<<PIN6))

#define LAYER_5 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6))  \
                PORTB |= ((0<<PIN4)|(0<<PIN5)|(1<<PIN6))

#define CP7     0x0001
#define CP12     0x0002
#define CP11     0x0004
#define CP10     0x0008
#define CP13     0x0010
#define CP16     0x0020
#define CP15     0x0040
#define CP14     0x0080
#define CP3    0x0100
#define CP2    0x0200
#define CP1    0x0400
#define CP6    0x0800
#define CP5    0x1000
#define CP4    0x2000
#define CP9    0x4000
#define CP8    0x8000

//global variables
//uint16_t layer0 = 0x0000;
//uint16_t layer1 = 0x0000;
//uint16_t layer2 = 0x0000;
//uint16_t layer3 = 0x0000;
//uint16_t layer4 = 0x0000;
//uint8_t frame[5] = {layer0,layer1,layer2,layer3,layer4};
uint16_t frame[10][5];

// *********************************

//#include "controllerTest.h"



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
	//Bit 3 = CPOL = Clock polarity (0 means low polarity)
	//Bit 2 = CPHA = Clock phase
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

    _delay_us(100);
    
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

void blue(uint8_t position){
    switch(position){
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = CP8;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 2:
            frame[0][0] = 0x0000;
            frame[0][1] = CP5;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 3:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = CP15;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 4:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = CP14;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 5:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = CP9;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 6:
            frame[0][0] = 0x0000;
            frame[0][1] = CP11;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
            /*
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
        case 1:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0008;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;
            */
        default:
            frame[0][0] = 0x0000;
            frame[0][1] = 0x0000;
            frame[0][2] = 0x0000;
            frame[0][3] = 0x0000;
            frame[0][4] = 0x0000;
            break;

    }


}

void red(uint8_t position){
//
}

void green(uint8_t position){
//
}

// NOTE: Colors are
//      0  Blue
//      1  Red
//      2  Green
// Layers can be 0 - 4
// positions can be 1 - 25
void LED_test(uint8_t color, uint8_t position,uint8_t layer){
        level(layer);
        switch(color){
            case 0:
                blue(position);
                break;
            case 1:
                red(position);
                break;
            case 2:
                green(position);
                break;
            default:
                break;
        }
        transmit1(frame[0][4],frame[0][3],frame[0][2],frame[0][1],frame[0][0]);

}

void patrick_test(void){
    uint16_t data1 = 0x00FF;
    uint16_t data2 = 0x00FF;
    uint16_t data3 = 0x00FF;
    uint16_t data4 = 0x00FF;
    uint16_t data5 = 0x00FF;

    transmit1(data1,data2,data3,data4,data5);
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
    //transmit1(data1,data2,data3,data4,data5);
    //level(layer);
    //_delay_ms(3);
    //_delay_ms(500);
    }
    /*
    layer = 6;

    data1 = ~0x00FF;
    data2 = ~0x00FF;
    data3 = ~0x00FF;
    data4 = ~0x00FF;
    data5 = ~0x00FF;
    transmit1(data1,data2,data3,data4,data5);
    level(layer);
    _delay_ms(500);
    _delay_ms(500);
    */
}

void on(void){
    uint16_t output5 = -1;

    transmit1(-1,-1,-1,-1,-1);
	}

void off(void){
    uint16_t output = 0; //All on
    uint16_t output2 = 0;
    uint16_t output3 = 0;
    uint16_t output4 = 0;
    uint16_t output5 = 0;

    transmit1(output,output2,output3,output4,output5);
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
    //_delay_ms(500);
    }

void hall_test(void){
    uint8_t input = 0;
    while(1){
        input = (~PINB & ((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)));
        //transmit1(CP3,0,0,0,0);
        //level(1);
        switch(input){
            case 0:
                level(0);
                transmit1(0,0,CP3,CP2,0);
                break;
            case 1:
                level(1);
                transmit1(0,0,CP3,CP2,0);
                break;
            case 2:
                level(2);
                transmit1(0,0,CP3,CP2,0);
                break;
            case 4:
                level(3);
                transmit1(0,0,CP3,CP2,0);
                break;
            case 8:
                level(4);
                transmit1(0,0,CP3,CP2,0);
                break;
            default:
                level(0);
                transmit1(CP2,0,0,0,0);
                
    }

}
}

void PIN_Test(void){
    static uint8_t pin = 0;
    uint8_t input = (~PINB & (1<<PB0));

    if(input == 1){
        pin++;
        pin = pin % 25;
    }
    

    switch(pin){
        case 1:
            transmit1(CP1,CP1,CP1,CP1,CP1);
            break;
        case 2:
            transmit1(CP2,CP2,CP2,CP2,CP2);
            break;
        case 3:
            transmit1(CP3,CP3,CP3,CP3,CP3);
            break;
        case 4:
            transmit1(CP4,CP4,CP4,CP4,CP4);
            break;
        case 5:
            transmit1(CP5,CP5,CP5,CP5,CP5);
            break;
        case 6:
            transmit1(CP6,CP6,CP6,CP6,CP6);
            break;
        case 7:
            transmit1(CP7,CP7,CP7,CP7,CP7);
            break;
        case 8:
            transmit1(CP8,CP8,CP8,CP8,CP8);
            break;
        case 9:
            transmit1(CP9,CP9,CP9,CP9,CP9);
            break;
        case 10:
            transmit1(CP10,CP10,CP10,CP10,CP10);
            break;
        case 11:
            transmit1(CP11,CP11,CP11,CP11,CP11);
            break;
        case 12:
            transmit1(CP12,CP12,CP12,CP12,CP12);
            break;
        case 13:
            transmit1(CP13,CP13,CP13,CP13,CP13);
            break;
        case 14:
            transmit1(CP14,CP14,CP14,CP14,CP14);
            break;
        case 15:
            transmit1(CP15,CP15,CP15,CP15,CP15);
            break;
        case 16:
            transmit1(CP16,CP16,CP16,CP16,CP16);
            break;
        default:
            off();
    }
    _delay_ms(500);
    _delay_ms(500);
    
}

/***********************************************************************/
//                                main                                 
/***********************************************************************/
int main(){

    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	spi_init();    //initalize SPI port
    uint8_t input = 0;
	while(1){
       // patrick_test();
        //level_test();
        //level(2);
        shift_LED();
        //transmit1(0,0,0,CP3,0);
        //level(4);
        //transmit1(0,0,0,0,CP1);
        //off();
        //transmit1(on,on,on,on,on);
        //LED_test(0,2,4);
        //hall_test();
        //PIN_Test();

    }     //empty main while loop
} //main
