//usart.h
//

#include "mirf.h"
#include "nRF24L01.h"



void usart_put(uint8_t data);
void usart_putframe(uint8_t *str);
char usart_get(void);

void usart_init(void);



void USART_Init( unsigned int baud )
void USART_Transmit( unsigned char data )
unsigned char USART_Receive( void )


