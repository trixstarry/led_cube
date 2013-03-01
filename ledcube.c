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
#include <util/delay.h>
#include "ledcube.h"


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

void PIN_Test(void){
    static uint8_t pin = 0;
    uint8_t input = (~PINB & (1<<PB0));

    if(input == 1){
        pin++;
        pin = pin % 17;
    }
    

    switch(pin){
        case 1:
            transmit1(C5P1,C4P1,C3P1,C2P1,C1P1);
            break;
        case 2:
            transmit1(C5P2,C4P2,C3P2,C2P2,C1P2);
            break;
        case 3:
            transmit1(C5P3,C4P3,C3P3,C2P3,C1P3);
            break;
        case 4:
            transmit1(C5P4,C4P4,C3P4,C2P4,C1P4);
            break;
        case 5:
            transmit1(C5P5,C4P5,C3P5,C2P5,C1P5);
            break;
        case 6:
            transmit1(C5P6,C4P6,C3P6,C2P6,C1P6);
            break;
        case 7:
            transmit1(C5P7,C4P7,C3P7,C2P7,C1P7);
            break;
        case 8:
            transmit1(C5P8,C4P8,C3P8,C2P8,C1P8);
            break;
        case 9:
            transmit1(C5P9,C4P9,C3P9,C2P9,C1P9);
            break;
        case 10:
            transmit1(C5P10,C4P10,C3P10,C2P10,C1P10);
            break;
        case 11:
            transmit1(C5P11,C4P11,C3P11,C2P11,C1P11);
            break;
        case 12:
            transmit1(C5P12,C4P12,C3P12,C2P12,C1P12);
            break;
        case 13:
            transmit1(C5P13,C4P13,C3P13,C2P13,C1P13);
            break;
        case 14:
            transmit1(C5P14,C4P14,C3P14,C2P14,C1P14);
            break;
        case 15:
            transmit1(C5P15,C4P15,C3P15,C2P15,C1P15);
            break;
        case 16:
            transmit1(C5P16,C4P16,C3P16,C2P16,C1P16);
            break;
        default:
            off();
    }
    _delay_ms(500);
    _delay_ms(500);
    
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
        state = ((red>>index)&(1));
        if(state){
            for(i=0;i<5;i++){
                frame[0][i] |= R[index][i];
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
                frame[0][i] |= B[index][i];
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
                frame[0][i] |= G[index][i];
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
        transmit2(i);
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
	while(1){

        test_frame();
       // patrick_test();
        //level_test();
        //level(2);
        //shift_LED();
        //transmit1(0,0,0,C1P3,0);
        //level(4);
        //transmit1(0,0,0,0,C1P1);
        //off();
        //transmit1(R1[4],R1[3],R1[2],R1[1],R1[0]);
        //test_led();
        //LED_test(0,5,4);
        //hall_test();
        //PIN_Test();

    }     //empty main while loop
} //main
