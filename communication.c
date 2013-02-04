// Communication.c
//
// Author: Doug Dziggel, Valerie Trang Hoang
// Date: 2/2/13
//
// Communication.c contains the low level communication methods and protocols for communicating between the computer and a micro controller via USART.
//
// Send Data
// Receive ACK/NACK
// ACK: Send next data     NACK: Resend Data
//
// Send Check Command   - checks hall effect sensors
//
// Broadcast to display frame X
//
// PACKET:
// START BYTE - 8 bits  - 1 byte
// TO         - 4 bits
// FROM       - 4 bits  - 1 bytes
// SUBJECT    - 3 bits
// LENGTH     - 4 bits  - 1 bytes        Possible stuffing
// Frame #    - 9 bits  - 1 byte         Possible Stuffing
// Packet #   - 3 bits  - 
// DATA       - 69 bits - 9 bytes       Possible to get up to twice the bytes here with stuffing
// CHECKSUM   - 16 bits -  2 bytes       Possible to get up to 4 bytes
// END BYTE   - 8 bits  - 1 byte
//
// This is the uC's communication file

// NOTE: Protocol and ID defines are randomly generated.
// PROTOCOLS:
#define START_BYTE      0x01
#define END_BYTE        0xFF
#define DATA            0x03
#define ACK             0x04
#define NACK            0x05
#define STUFF_BYTE      0x02

//IDs:
#define ID_SELF         0x03 
#define ID_COMP         0x04
#define ID_CUBE1        0x05
#define ID_CUBE2        0x06
#define ID_CUBE3        0x07
#define ID_CUBE4        0x08
#define BROADCAST       0x09


#define TIMEOUT         40   // in milliseconds

uint8_t parse_packet(uint32_t packet){
    // check ID otherwise ignore
    // destuff
    // check checksum otherwise nack
    // get subject
    // store who its from incase a response is needed
    // get data
    // return everything
}

int8_t create_packet(uint8_t to, uint8_t subject, uint16_t frame,uint64_t *data, uint32_t *output){
    //for data in len of data
    //      break data into 9 byte increments

    //for data in list of data
    //      output[i] = startbyte + all that stuff listted in the header

    // return success otherwise return -1

}

uint8_t calculate_checksum(uint8_t *data, uint8_t len){
    uint8_t ileft = len;
    uint8_t *i = data;
    uint8_t checksum;
    uint16_t sum =0;

    while(ileft > 1){
        sum += *i++;
        ileft -= 2;
    }

    return (checksum);

}

/*
uint8_t calculate_checksum(uint8_t *data, uint8_t len){
    uint8_t ileft = len;
    uint8_t *i = data;
    uint8_t checksum;
    uint16_t sum =0;

    while (ileft > 1)  {
        sum ^= *i++;
        ileft -= 2;
    }

    // mop up an odd byte, if necessary 
    if (ileft == 1){
        sum ^= *(uint8_t *)i;
    }

    checksum = ~sum;                    // truncate to 8 bits 
    return (checksum);
}
*/

uint8_t destuffer(uint8_t data){
    if(data == START_BYTE){
        //do stuff
        //return stuff
    }
    if(data == STOP_BYTE){
        //do stuff
        //return stuff
    }
    if(data == STUFF_BYTE){
        //do stuff 
        //return stuff
    }
}


/* step before transmitting usart
 */
uint8_t stuffer(uint8_t data){
    if(data == START_BYTE){
        //do stuff
        //return stuff
    }
    if(data == STOP_BYTE){
        //do stuff
        //return stuff
    }
    if(data == STUFF_BYTE){
        //do stuff 
        //return stuff
    }
}

//Transmit sends data to the specified location and returns 
// the value of the cubes touching it
uint8_t transmit(uint8_t id, uint8_t type, uint8_t data){
    // Break data into packets

    // load data into buffer of bytes

    // while (there is a packet): 
    //      send packet   - Stuff while sending packet
    //      wait for ack
    //      if ack        if nack
    //      check ack     do nothing
    //      store data
    //      increment packet counter

}


//
//
uint8_t receive( void ){
    // parse packet
    // ACK


}







