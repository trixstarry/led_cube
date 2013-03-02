// header file
//
//

#include <avr/io.h>
#include <stdlib.h>
#include "mirf.h"
#include "nRF24L01.h"


//
// NOTE: Protocol and ID defines are randomly generated.
// Subjects:
#define DATA            0x03
#define ACK             0x04
#define NACK            0x05
#define NEXT_FRAME      0x06
#define END_DATA        0x07

// Control:
#define START_BYTE      0x01
#define END_BYTE        0xFF
#define STUFF_BYTE      0x02
#define STUFF_STUFF     0x03 //if stuff byte is found in data stuff it to 0x03
#define STUFF_START     0x04 // same
#define STUFF_STOP      0x05 // same

// SHIFTING DEFINES
#define TO_SHIFT        0x3C  //shift 60 bits
#define FROM_SHIFT      0x38  //shift 56 bits
#define SUBJ_SHIFT      0x35  //shift 53 bits
#define FRAME_SHIFT     0x2B  //shift 43 bits
#define PACKET_SHIFT    0x28  //shift 40 bits
#define DATA_SHIFT1     0x00  //shift none -- end of packet part
#define DATA_SHIFT2     0x18  //shift 24 bits
#define CHECK_SHIFT     0x08  //shift 8  bits
                            //NOTE: packet part contains an extra byte.
                            //      Be sure to remove before transmission.
//IDs:
#define ID_SELF         0x03 
#define ID_COMP         0x04
#define ID_CUBE1        0x05
#define ID_CUBE2        0x06
#define ID_CUBE3        0x07
#define ID_CUBE4        0x08
#define BROADCAST       0x09


#define TIMEOUT         40   // in milliseconds



uint8_t parse_packet(uint8_t *to, uint8_t *from, uint8_t *subject, uint16_t *frame_num, uint8_t *packet_num,uint8_t *last_packet, uint16_t *checksum,uint8_t *data,uint8_t *packet);
int8_t create_packets(uint8_t to, uint8_t subject, uint16_t frame,uint8_t *data, uint8_t data_len,uint16_t checksum, uint64_t packets[5][2]);
uint16_t calculate_checksum(uint64_t packet[5][2],uint8_t index);
uint8_t destuffer(uint8_t *data, uint8_t *packet);
uint8_t stuffer(uint64_t packets[5][2], uint64_t stuffed_packets[5][4]);
uint8_t receive( void );
uint8_t transmit(uint8_t id, uint8_t type, uint8_t *data)
