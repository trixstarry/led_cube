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
//oh yea

#define F_CPU 8000000UL	// 1Mhz clock
#define BAUD 250000
#define BUFFER_SIZE 32
//#define MY_UBBR 1 // F_CPU/(16*BAUD)-1
#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400
//#define BAUD 9600
//#define MY_UBBR 51 // F_CPU/(16*BAUD)-1
#define BUSY 0xAB
#define STATUS_REQUEST 0xAC
#define OK  0xAA

//#define SET_U2X 1 // Set U2X in UCSRA (double speed USART)
//#define MY_UBBR 12 // 76800


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "usart.h"


volatile char buffer [32] = {'7','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
                    'q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6'};
volatile uint8_t buffer_index = 0;
volatile uint8_t transmit_flag = 0;
volatile uint8_t timeout_flag = 1;
volatile uint8_t doing_flag = 0;

void Transmit(uint8_t *buffer,uint8_t buffersize){
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

int8_t Receive(uint8_t *buffer,uint8_t buffersize){
    uint32_t i = 0;
//char test [7] = {'d','a','t','a',' ','i','s',' ','\n'};
    while (!mirf_data_ready()){
        _delay_us (100);
        if(i > 50){
            return -1;
        }
        i++;
    }
    //transmit_string("data is ready but i want to see\n");
    //for(i = 0; i < 7;i++){
    //    USART_Transmit(test[i]);
    //}
    //_delay_ms(50);
    mirf_get_data(buffer);
    transmit_string("\r\n");
    for(i = 0; i < (buffersize); i++)
    {
        USART_Transmit(buffer[i]);
    }
    transmit_string("\r\n");
    return 1;
}

void test_Transmit(uint8_t *buffer, uint8_t len){
    buffer[len-1]++;
    if (buffer[len-1] < ' ' || buffer[15] > 'z')
    {
        buffer[len-1] = ' ';
    }
    Transmit(buffer,len);
    
}

void test_protocol(uint8_t *buffer, uint8_t len){
    //transmit_string("txing\r\n");

    //Transmit(buffer,len);
    _delay_ms(5);
    ////transmit_string("Rx_Powerup\r\n");
    //rx_powerup();
    //_delay_ms(1);
    //transmit_string("rxing\r\n");
    if(Receive(buffer,len)==1){
        //transmit_string("rx ok\r\n");
    }
    else{
        //transmit_string("rx bad\r\n");
    }
    //transmit_string ("test_protocol_done\r\n");
}
void USART_Flush( void )
{
    unsigned char dummy;
    while ( UCSRA & (1<<RXC) ) {dummy = UDR;}
}

void test () {
    if(1 == transmit_flag){
        Transmit(buffer,BUFFER_SIZE);
        USART_Transmit(OK);
        transmit_flag = 0;
    }
    else{
        if(Receive(buffer,BUFFER_SIZE)==1){
            transmit_string("k\n");
        }
        else{
            //transmit_string("b\n");
        }
        buffer_index = 0;
    }
}


ISR(USART_RX_vect){
    if(transmit_flag == 1){
        USART_Receive();
        USART_Transmit(BUSY);
        return;
    }
    buffer[buffer_index] = USART_Receive();
    if(buffer_index == 3){
        if(buffer[buffer_index] == '\x00'){
            timeout_flag = 0;
        }
        else{
            timeout_flag = 1;
        }
    }
    if(buffer_index == 31){
        transmit_flag = 1;
        //UCSRB = ~(1<<RXEN);
    }
    buffer_index = ((buffer_index+1)%32);

}

int main (void)
{
	USART_Init(MY_UBBR);
//    while (1)
//    {
//        _delay_ms(100);
//        //transmit_string ("Device Started!\r\n");
//    }
	
	//uint8_t buffersize = BUFFER_SIZE;
	// Initialize AVR for use with mirf

	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
    sei();
	mirf_read_register (STATUS, buffer, 1);
	// Configure mirf
	mirf_config();
	// Test transmitting
	buffer[0] = '7';
    //rx_powerup();
    _delay_ms(100);
    mirf_config_register(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Reset status register
    _delay_ms(50);
	
    transmit_string ("\r\ns");
	while (1)
	{
        //transmit_string ("l");
        //transmit_string ("Device Started!\r\n");
        //test_Transmit(buffer,BUFFER_SIZE);
        
        //Transmit(buffer,BUFFER_SIZE); //_delay_ms(5);
        //rx_powerup();
        //_delay_ms(1);
        ////transmit_string("preSending\n");
        //Receive(buffer,BUFFER_SIZE);
        //test_Transmit(buffer,BUFFER_SIZE);
       //Transmit(buffer,BUFFER_SIZE);
        ////transmit_string("Did i send?\r\n");
        //_delay_ms(100);
        test();
        
        /*
    if(transmit_flag ==1){
        uint8_t status = UCSRB; //save usart status
        UCSRB &= ~(1<<RXCIE);   //turn off receive interrupt enable
        //transmit_string("t");
        uint16_t counter = 0;
        while (mirf_send (buffer, BUFFER_SIZE) && counter < 1000)
        {
            _delay_us(10);
        }
        if (counter >= 1000)
        {
            //transmit_string("e");
        }

        //_delay_ms(50);
        ////transmit_string("Rx_Powerup\r\n");
        //rx_powerup();
        //_delay_ms(50);
        //transmit_string("r");
        if(1 == timeout_flag){
            if(Receive(buffer,BUFFER_SIZE)==1){
                transmit_string("k\n");
            }
            else{
                transmit_string("b\n");
            }
            //transmit_flag = 0;
        }
        transmit_string("n\n");
        transmit_flag = 0;
        for(buffer_index = 0; buffer_index < BUFFER_SIZE;buffer_index++){
            buffer[buffer_index] = 0;
        }
        buffer_index = 0;
        USART_Flush();
        UCSRB = status;//|(1<<RXEN); //if interrupt was enable previously reenable it.
        */
    }

 
//        test_protocol(buffer,BUFFER_SIZE);
//	}
	
}
