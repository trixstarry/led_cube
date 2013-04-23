//usart.h
//

#include "mirf.h"
#include "nRF24L01.h"

#define SET_U2X 0
#define MY_UBBR 12 // BAUD = 38400



//void usart_put(uint8_t data);
//void usart_putframe(uint8_t *str);
//char usart_get(void);

void usart_init(void);



extern void USART_Init( unsigned int baud );
extern void USART_Transmit( unsigned char data );
extern unsigned char USART_Receive( void );
extern void transmit_string(char *ptr);


