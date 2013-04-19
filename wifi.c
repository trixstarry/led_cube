
// wifi.c
//
//  Project: LED Cube
//  Hardware: LED Cube USB Dongle
// 	Author:	Doug Dziggel
// 	Date:	1.16.13
//
// Description:
// This program runs on the LED Cube usb dongle. It takes the incoming data
// from the computer sends it wirelessly to the LED Cubes. It also passes
// information from the cubes to the computer.
//
// It runs on an Atmega2313 that has the following:
// Flash: 2K
// EEPROM: 128 Bytes
// SRAM: 128 Bytes
//
// Atmega2313 Pinout:
//****************************************************************
// Atmega2313       NRF24L01        FTDI        PROGRAMMER HEADER
// ***************************************************************
// VCC = 3.3V       VCC = 3.3V      VCC = 3.3V  VCC = 3.3V
// GND =            GND             GND         VCC
// PA2 =  ------------------------------------  RESET
// PB2 =  -------  CSN
// PB3 =  -------  CE
// PB4 =  -------  IRQ
// PB5 =  -------  MI  -----------------------  MOSI
// PB6 =  -------  MO  -----------------------  MISO
// PB7 =  -------  SCK -----------------------  SCK
// PD1 =  ------------------------  RX
// PD2 =  ------------------------  TX
//
// NOTE: If you edit this code or the hardware keep the above in mind.
//************************************************************************//

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


volatile uint8_t instr = 0;
volatile char buffer [32] = {'7','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
                    'q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6'};
volatile char buffer_in_use = 0;

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
//    transmit_string("\r\n");
//    for(i = 0; i < (buffersize); i++)
//    {
//        USART_Transmit(buffer[i]);
//    }
//    transmit_string("\r\n");
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
    if (1 == Receive(buffer, BUFFER_SIZE))
    {
        USART_Transmit('c');
        USART_Transmit('\n');
        char i = 0;
        for(i = 0; i < BUFFER_SIZE; i++)
        {
            USART_Transmit(buffer[i]);
        }
        USART_Transmit('\n');
    }
    if(1 == buffer_in_use)
    {
        // do something with the data in buffer...
        Transmit(buffer, BUFFER_SIZE);
        buffer_in_use = 0;
    }
}

void set_channel(uint8_t channel)
{
    mirf_config_register(RF_CH,channel); // Set the channel
}

void set_TX_addr(uint8_t *addr){
    mirf_set_TADDR(addr);
}

void set_RX_addr(uint8_t pipe,uint8_t *addr)
{
    switch(pipe){
        case 0:
            mirf_set_RADDR(addr);
            break;
        case 1:
            mirf_set_RADDR_P1(addr);
            break;
        case 2:
            mirf_set_RADDR_P2(addr); 
            break;
        case 3:
            mirf_set_RADDR_P3(addr);
            break;
        case 4:
            mirf_set_RADDR_P4(addr); 
            break;
        case 5:
            mirf_set_RADDR_P5(addr); 
            break;
        default:
            break;
    }
}


ISR(USART_RX_vect){

    uint8_t data = USART_Receive();
    if ((0 == buffer_index) && (0 == buffer_in_use))
    {
        transmit_flag = 0;
    }
    if (0 == transmit_flag)
    {
        if(0 == buffer_index)
        {
            instr = data;
        }
        else
        {
            buffer[buffer_index] = data;
        }
    }

    if(buffer_index == 32){
        if (1 == transmit_flag)
        {
            USART_Transmit('b');
            USART_Transmit('\n');
        }
        else
        {
            transmit_flag = 1;
            buffer_in_use = 1;
            USART_Transmit('a');
            USART_Transmit('\n');
        }
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
    uint64_t addr;
	
    transmit_string ("s\r\n");
    while (1)
    {
        //test();
        if (1 == Receive(buffer, BUFFER_SIZE))
        {
            USART_Transmit('c');
            USART_Transmit('\n');
            char i = 0;
            for(i = 0; i < BUFFER_SIZE; i++)
            {
                USART_Transmit(buffer[i]);
            }
            USART_Transmit('\n');
        }
        if(1 == buffer_in_use)
        {
            // do something with the data in buffer...
            switch(instr){
                case 0:
                    Transmit(buffer, BUFFER_SIZE);
                    break;
                case 1:
                    set_channel(buffer[0]);
                case 2:
                    addr = (buffer[2]<<0)|(buffer[3]<<8)|(buffer[4]<<24)|
                           (buffer[5]<<32)|(buffer[6]<<40);
                    if(1 == buffer[0]){
                        set_TX_addr(addr);
                    }
                    else{
                        set_RX_addr(buffer[1],addr);
                    }
            }
            //Transmit(buffer, BUFFER_SIZE);
            buffer_in_use = 0;
        }
    }
}
