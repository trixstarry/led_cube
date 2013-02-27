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
                PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6)) \
                PORTB |= ((0<<PIN4)|(0<<PIN5)|(0<<PIN6))

#define LAYER_2 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6))  \
                PORTB |= ((1<<PIN4)|(0<<PIN5)|(0<<PIN6))

#define LAYER_3 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6))  \
                PORTB |= ((0<<PIN4)|(1<<PIN5)|(0<<PIN6))

#define LAYER_4 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6))  \
                PORTB |= ((1<<PIN4)|(j<<PIN5)|(0<<PIN6))

#define LAYER_5 PORTB &= ~((1<<PIN4)|(1<<PIN5)|(1<<PIN6))  \
                PORTB |= ((0<<PIN4)|(0<<PIN5)|(1<<PIN6))
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
    
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}

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
                transmit1(on,off,off,off,off);
                break;
            case 1:
                level(i);
                transmit1(off,on,off,off,off);
                break;
            case 2:
                level(i);
                transmit1(off,off,on,off,off);
                break;
            case 3:
                level(i);
                transmit1(off,off,off,on,off);
                break;
            case 4:
                level(i);
                transmit1(off,off,off,off,on);
                break;

        }
    //transmit1(data1,data2,data3,data4,data5);
    //level(layer);
    //_delay_ms(500);
    _delay_ms(500);
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

    level(3);

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
    _delay_ms(100);
    //_delay_ms(500);
    }


/***********************************************************************/
//                                main                                 
/***********************************************************************/
int main(){

    DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
	spi_init();    //initalize SPI port
    uint8_t input = 0;
	while(1){
       // patrick_test();
        //level_test();
        //shift_LED();
        transmit1(0,0,0,0,0x0001);

    }     //empty main while loop
} //main
