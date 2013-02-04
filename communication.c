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
// Frame #    - 10 bits  - 1 byte         Possible Stuffing
// Packet #   - 3 bits  - 
// DATA       - 64 bits - 8 bytes       Possible to get up to twice the bytes here with stuffing
// CHECKSUM   - 16 bits -  2 bytes       Possible to get up to 4 bytes
// END BYTE   - 8 bits  - 1 byte
//
// This is the uC's communication file
//
//packet[i] = (START_BYTE<<START_SHIFT)|(TO<<TO_SHIFT)|(ID_SELF<<FROM_SHIFT)|(subject<<SUBJ_SHIFT)
//            |(LENGTH<<LEN_SHIFT)|(frame<<FRAME_SHIFT)|(i<<PACKET_SHIFT)|(data[i]<<DATA_SHIFT)|
//            (checksum<<CHECK_SHIFT)|(END_BYTE);


// NOTE: Protocol and ID defines are randomly generated.
// PROTOCOLS:
#define START_BYTE      0x01
#define END_BYTE        0xFF
#define DATA            0x03
#define ACK             0x04
#define NACK            0x05
#define NEXT_FRAME      0x06
#define STUFF_BYTE      0x02

// SHIFTING DEFINES
#define START_SHIFT     0x18
#define TO_SHIFT        0x14
#define FROM_SHIFT      0x10
#define SUBJ_SHIFT      0x0D
#define LEN_SHIFT       0x09
#define FRAME_SHIFT     0x0
#define PACKET_SHIFT
#define DATA_SHIFT
#define CHECK_SHIFT

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

// PACKET:
// START BYTE - 8 bits  - 1 byte
// TO         - 4 bits
// FROM       - 4 bits  - 1 bytes
// SUBJECT    - 3 bits
// Frame #    - 10 bits  - 1 byte         Possible Stuffing
// Packet #   - 3 bits  - 
// DATA       - 80 bits - 10 bytes       Possible to get up to twice the bytes here with stuffing
// CHECKSUM   - 16 bits -  2 bytes       Possible to get up to 4 bytes
// END BYTE   - 8 bits  - 1 byte
int8_t create_packets(uint8_t to, uint8_t subject, uint16_t frame,uint16_t *data, uint8_t data_len, uint64_t **packets){
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t data_byte_len = 0;
    uint8_t data_byte[32] = {}; //MAX data input is 32 bytes
    uint64_t data_chunk[4] = {};// max is 4 chunks based on above

    //Convert Data into bytes
    while(i < data_len){
        data_byte[k] = (data[i]>>(j*8))
        j++;
        k++;
        if((j%2)==0){
            i++;
            j = 0;
        }
    }

    //Store length of byte data
    data_byte_len = k+1;

    i = j = k = 0;

    //place data into 10 byte chunks (for our cube this the max should be 2 chunks)
    while(i < data_byte_len){
        data_chunk[k] = (data[i]<<(j*8))
        j++;
        k++;
        if((j%5)==0){
            i++;
            j = 0;
        }
    
    }

    //Create Packets
    while(i <= k){
        packet[j][i] = START_BYTE;
        packet[j][i] = to;
        packet[j][i] = ID_SELF;
        packet[j][i] = subject;
        packet[j]
        packet[j][i] = length;
        packet[j][i] = checksum();
                (ID_SELF<<FROM_SHIFT)|(subject<<SUBJ_SHIFT)

            |(LENGTH<<LEN_SHIFT)|(frame<<FRAME_SHIFT)|(i<<PACKET_SHIFT)
            (data_chunk[i]<<DATA_SHIFT)|
            (checksum<<CHECK_SHIFT)|(END_BYTE);
    }
}

    //      break data into 9 byte increments

    //for data in list of data
    //      output[i] = startbyte + all that stuff listted in the header

    // return success otherwise return -1

}

/*
uint16_t calculate_checksum(uint8_t *data, uint8_t len){
    uint8_t ileft = len;
    uint8_t *i = data;
    uint16_t checksum;
    uint16_t sum =0;

    while(ileft > 1){
        sum ^= *i++;
        ileft -= 2;
    }

    return (checksum);

}
*/


uint16_t calculate_checksum(uint8_t *data, uint8_t len){
    uint8_t ileft = len;
    uint8_t *i = data;
    uint16_t checksum;
    uint16_t sum =0;

    while (ileft > 1)  {
        sum ^= *i++;
        ileft -= 2;
    }

    // mop up an odd byte, if necessary 
    if (ileft == 1){
        sum ^= *(uint8_t *)i;
    }

    checksum = sum;                    // truncate to 8 bits 
    return (checksum);
}

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
uint8_t transmit(uint8_t id, uint8_t type, uint32_t *data)_{
    uint8_t length = sizeof(data)/sizeof(data[0]);
    uint32_t packets[5] = {0,0,0,0,0};
    // Break data into packets
    create_packets(id, type, frame, data, length, packets)

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







