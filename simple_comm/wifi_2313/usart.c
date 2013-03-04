//USART.c
//
//Doug Dziggel
//11/28/12
//

#include "usart.h"

/*

// **********************************************************
//						usart_put
//
//Takes a character and sends it to USART0
//
void usart_put(uint8_t data){
	while(!(UCSR0A&(1<<UDRE))); // Wait for previous transmission
	UDR0 = data;   // Send data byte
	while (!(UCSR0A&(1<<UDRE))); // Wait for previous transmission
}

// ***************************************************************
//						usart_putframe
//Takes a string and sends each character to be sent to USART0
//void usart_puts(unsigned char *str) {
void usart_putframe(uint8_t *data){
	int i = 0;
	//Loop through string, send each char
	while(data[i] != '\0'){usart_put(data[i]); i++;}
}

// **************************************************************
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

// **************************************************************
//						usart_getframe
//Modified to not block indefinately in the case of a lost byte
//
//NOTE: THIS USES MALLOC. WHEN THE POINTER IS DONE BEING USED
//FREE THE MEMORY!!!!!!!
//
void usart_get(uint8_t received[32]){
	uint8_t byte_count = 0;
	while(timer < PACKET_SIZE){
        data[byte_count] = usart_get();
        if(data[byte_count] == 0){
            return 0;
        }
        byte_count++;
	}
	return data;
}
*/

// **************************************************************
//
//                            usart_init

void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	if (SET_U2X)
	{
		UCSRA |= (1<<U2X);
	}
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);
}


void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}

void transmit_string(char *ptr)
{
	while(*ptr)
	{
		USART_Transmit(*ptr);
		ptr++;
	}
}
