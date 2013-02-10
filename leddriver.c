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

void transmit(uint32_t data){
	//break the data up into 4 bytes
	
	uint8_t temp = 0;

	temp = (data);
	//load first byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data>>8);
	//load second byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data>>16);
	//load third byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	temp = (data>>24);
	//load fourth byte
	SPDR = temp;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};
	
	//Toggle latch
	PORTA |= (1<<SS);
	PORTA &= ~(1<<SS);
}

void test_run(void){
	//break the data up into 4 bytes
	
	//load first byte
	SPDR = 0xff;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};
	/*
	//Toggle latch
	PORTB |= 0x80;
	PORTB &= ~(0x80);
	*/

	//load second byte
	SPDR = 0xff;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};

	/*
	//Toggle latch
	PORTB |= 0x00;
	PORTB &= ~(0x80);
	*/
	
	//load third byte
	SPDR = 0xff;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};
	/*
	//Toggle latch
	PORTB |= 0x00;
	PORTB &= ~(0x80);
	*/
	


	//load fourth byte
	SPDR = 0xff;
	//temp = 0x0F & display_count;
	while(bit_is_clear(SPSR,SPIF)){};
	
	//Toggle latch
	PORTB |= 0x80;
	PORTB &= ~(0x80);
}

void on(void){
    uint8_t output = -1; //All on
    transmit(output);
	}

void off(void){
    uint32_t output = 0;
    transmit(output);
}

void LED1on(void){
    uint32_t output = 0x01000000;
    transmit(output);
}

void LED17on(void){
    uint32_t output = 0x00000100;
    transmit(output);
}

void driver1_count(void){
    static uint16_t count = 0;
    transmit((count<<16));
    }

void driver2_count(void){
    static uint16_t count = 0;
    transmit(count);
    }

void shift_LED(void){
    static uint32_t LED = 0x00000001;
    if(LED == 0){
        LED = 0x00000001;
    }
    transmit(LED);
    LED = LED << 1;
    }

void test1(void){
    static uint32_t output = -1;
    transmit(output);
}

void test2p1(void){
    uint32_t output = 0x00010000;
    transmit(output);
}

void test2p2(void){
    uint32_t output = 0x00000001;
    transmit(output);
}

void test3(void){
    static uint8_t i = 0;
    static uint32_t output1 = 0x00010000;
    static uint32_t output2 = 0x00000001;
    switch(i){
        case 0:
            transmit(output1);
            break;
        case 1:
            transmit(output2);
            break;
        case 2:
            transmit(0);
            break;
        case 3:
            transmit(0);
            break;
        case 4:
            transmit(0);
            break;
    }
    i++;
    i = i%5;
    /*
    if((i%5)==0){
    i = 0;
    transmit(output1);
    }
    else{transmit(output2);}
    i++;
    */
}
/*************************************************************************/
//                           timer/counter0 ISR                          
//When the TCNT0 overflow interrupt occurs, the count_7ms variable is    
//incremented.  Every 7680 interrupts the minutes counter is incremented.
//tcnt0 interrupts come at 7.8125ms internals.
// 1/32768         = 30.517578uS
//(1/32768)*256    = 7.8125ms
//(1/32768)*256*64 = 500mS
/*************************************************************************/
/*
ISR(TIMER0_OVF_vect){
	static uint8_t count_7ms = 0;        //holds 7ms tick count in binary
	static uint32_t display_count = 0x00000001; //holds count for display 
	uint8_t temp = 0;
	count_7ms++;                //increment count every 7.8125 ms 
	if ((count_7ms % 1)==0){ //?? interrupts equals one half second 
	//	if((temp % 64) == 0){
			transmit(display_count);
			//transmit(0xFFFFFFFF);
			display_count++;
			//display_count = (display_count << 1); //shift display bit for next time 
			temp++;
//		}
	}
	//if (display_count == 0x00){display_count=0x01;} //display back to 1st positon
}
*/

/***********************************************************************/
//                                main                                 
/***********************************************************************/
int main(){

	//port initialization
	//DDRA &= ~(0x03);  //set port B bits 7,6,5,4,2,1,0  as outputs
    //PORTA |= (0x03);
    //DDRB = 0xFF;
    //PORTB = 0x01;
	//tcnt0_init();  //initalize counter timer zero
    //PORTA |= 0x02;
	spi_init();    //initalize SPI port
    //PORTB = 0x02;
	//sei();         //enable interrupts before entering loop
    uint8_t input = 0;
	while(1){
        input = 3;
        if(input == 0){
            off();
        }
        if(input == 1){
            test1();
        }
        if(input == 2){
            shift_LED();
            _delay_ms(250);
        }
        //test2p2();
        if(input == 3){
            test3();
        }
        //shift_LED();
        //_delay_ms(250);
    }     //empty main while loop
} //main
