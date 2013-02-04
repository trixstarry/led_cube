//USART.c
//
//Doug Dziggel
//11/28/12
//

#include "usart.h"

//**********************************************************
//						usart_put
//
//Takes a character and sends it to USART0
//
void usart_put(uint8_t data){
	while(!(UCSR0A&(1<<UDRE))); // Wait for previous transmission
	UDR0 = data;   // Send data byte
	while (!(UCSR0A&(1<<UDRE))); // Wait for previous transmission
}

//***************************************************************
//						usart_putframe
//Takes a string and sends each character to be sent to USART0
//void usart_puts(unsigned char *str) {
void usart_putframe(uint8_t *data){
	int i = 0;
	//Loop through string, send each char
	while(data[i] != '\0'){usart_put(data[i]); i++;}
}

//**************************************************************
//						usart_get
//Modified to not block indefinately in the case of a lost byte
//
uint8_t usart_get(void){
	uint16_t timer = 0;
	while(!(UCSR0A & (1<<RXC0))){
		timer++;
		if(timer >= 16000) return 0;
	}
	return UDR0;
}

//**************************************************************
//						usart_getframe
//Modified to not block indefinately in the case of a lost byte
//
//NOTE: THIS USES MALLOC. WHEN THE POINTER IS DONE BEING USED
//FREE THE MEMORY!!!!!!!
//
uint8_t *usart_get(void){
	uint8_t byte_count = 0;
    uint8_t *data = malloc(PACKET_SIZE);
	while(timer < PACKET_SIZE){
        data[byte_count] = usart_get();
        if(data[byte_count] == 0){
            return 0;
        }
        byte_count++;
	}
	return data;
}

//**************************************************************
//
//                            usart_init
//
//
//
//RXD0 is PORT E bit 0
//TXD0 is PORT E bit 1
//Jumpers J14 and J16 (mega128.1) OR
//Jumpers  J7 and  J9 (mega128.2)
//must be in place for the MAX232 chip to get data.
//
void usart_init(){

	//	UCSR0A |= (1<<U2X0);
	//rx and tx enable, 8 bit characters

	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);

	//async operation, no parity,  one stop bit, 8-bit characters

	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);

	//set to 9600 baud

	UBRR0H=0x00;

	UBRR0L=0x67;

}
