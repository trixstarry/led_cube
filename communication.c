// Communication.c
//
// Author: Doug Dziggel, Valerie Trang Hoang
// Date: 2/2/13
//
// Communication.c contains the low level communication methods and protocols for communicating between the computer and a micro controller via USART.
//
// This is the uC's communication file

// NOTE: Protocol and ID defines are randomly generated.
// PROTOCOLS:
#define RECEIVE_READY   0x26
#define TRANSMIT_READY  0x9E
#define ACK             0xEA
#define NACK            0x85
#define TRANSMIT_DONE   0xFB

//IDs:
#define ID_SELF         0xE3 
#define ID_COMP         0x03
#define ID_CUBE1        0x54
#define ID_CUBE2        0x17
#define ID_CUBE3        0x23
#define ID_CUBE4        0xDB


#define TIMEOUT         5   // in milliseconds

uint8_t parse_header(uint8_t header){
    static uint8_t FROM = 0; 
    static uint8_t TO = 0;
    static uint8_t SUBJ = 0;

    FROM = ((header & 0xE0)>>5);
    TO = ((header & 0x1C)>>2);
    SUBJ = (header & 0x03)
}

uint8_t check_receive()


uint8_t transmit(uint8_t id, uint8_t type, uint8_t data){
    USART(id)
    send(TRANSMIT_READY)


}







