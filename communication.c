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
#define BROADCAST       0xF6


#define TIMEOUT         5   // in milliseconds

uint8_t parse_packet(uint64_t packet){
    static uint8_t FROM = 0; 
    static uint8_t TO = 0;
    static uint8_t SUBJ = 0;
    static uint8_t packet = 0;
    static uint32_t msg = 0;

    FROM = ((header & 0xE0)>>5);
    TO = ((header & 0x1C)>>2);
    SUBJ = (header & 0x03)
}

uint8_t calculate_checksum(uint8_t *data, uint8_t len){
    uint8_t ileft = len;
    uint8_t *i = data;
    uint8_t checksum;
    uint16_t sum =0;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum),
     * we add sequential 8 bit words to it, and at the end, fold
     * back all the carry bits from the top 8 bits into the lower
     * 8 bits.
     *
     */
    while (ileft > 1)  {
        sum += *i++;
        ileft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (ileft == 1)
        sum += *(uint8_t *)i;

    /*
     * add back carry outs from top 8 bits to low 8 bits
     *
     */
    sum = (sum >> 8) + (sum & 0xffff);  /* add hi 8 to low 8 */
    sum += (sum >> 8);                  /* add carry */
    checksum = ~sum;                    /* truncate to 8 bits */
    return (checksum);
}

uint8_t check_receive()


    uint8_t transmit(uint8_t id, uint8_t type, uint8_t data){
        USART(id)
            send(TRANSMIT_READY)


    }







