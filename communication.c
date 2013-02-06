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
// Frame #    - 9 bits  - 2 byte         Possible Stuffing
// Packet #   - 3 bits  - 
// Last Packet- 1 bit
// DATA       - 80 bits - 10 bytes       Possible to get up to twice the bytes here with stuffing
// CHECKSUM   - 16 bits -  2 bytes       Possible to get up to 4 bytes
// END BYTE   - 8 bits  - 1 byte
//
// This is the uC's communication file
//
// TODO:
//      1. Finish Receive function.
//      2. REDO shift defines
//      4. Finish parse packet.
//      5. TEST Code
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

uint8_t parse_packet(uint32_t packet){
    // check ID otherwise ignore
    // destuff
    // check checksum otherwise nack
    // get subject
    // store who its from incase a response is needed
    // get data
    // return everything
}

// *******************************************************************
//                  create_packets
//  This method creates a full packet (minus start and stop bytes)
//  First it creates data "chunks" which consist of an array of 4 5 byte chunks
//  Then it creates the packets. Since a 15 byte packet cannot be created, a 2D array
//  of containing packets that are broken into 2 8 byte chunks.
//  For example, Packet 1 contains two 8 byte chunks
int8_t create_packets(uint8_t to, uint8_t subject, uint16_t frame,uint8_t *data, uint8_t data_len, uint64_t **packets){
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint64_t data_chunk[4] = {};// max is 4 chunks based on above

    /*
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
    */

    //place data into 5 byte chunks (for our cube this the max should be 2 chunks)
    j=5;
    while(i < data_len){
        data_chunk[k] = (data[i]<<(j*8))
            j--;
        i++;
        if(j==0){
            k++;
            j = 5;
        }

    }

    i = j = k = 0;
    //Create Packets
    while(j <= 4){
        /*
           packet[j][0] = START_BYTE;
           packet[j][1] = to;
           packet[j][2] = ID_SELF;
           packet[j][3] = subject;
           packet[j][4] = data_chunk[j];
           packet[j][5] = data_chunk[j+1];
           packet[j][6] = length;
           packet[j][7] = checksum();
           j++;
           */
        packet[k][0] = (TO<<TO_SHIFT)|(ID_SELF<<FROM_SHIFT)|(subject<<SUBJ_SHIFT)
            |(frame<<FRAME_SHIFT)|(k<<PACKET_SHIFT)|(data_chunk[j]<<DATA_SHIFT1);//add in 1 "chunk" of data
        if((j+1)<=4){
            packet[k][1] |= (data_chunk[j+1]<<DATA_SHIFT2);//add second "chunk" of data
        }
        checksum = checksum(packet[k][0],packet[k][1]) // calculate checksum
            packet[k][1] = (CHECKSUM<<CHECK_SHIFT);
        k++;
    }
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

// ****************************************************************
//                  calculate_checksum
//
//  Calculates the checksum for a given packet.
//  Checksum is calculated by breaking the backet into 16 bytes and
//  XOR all of them together. Finally the answer is truncated to 16
//  bits.
//
uint16_t calculate_checksum(uint64_t *data1, uint64_t *data2){
    uint8_t data_byte[16] = {};
    uint16_t checksum;
    uint32_t sum =0;

    uint8_t k = 0;
    uint8_t l = 7;
    uint8_t m = 0;

    //Convert Data into bytes
    while(k < 16){
        data_byte[k] = ((data[m]>>(l*8))&(0xFF))
            l--;
        k++;
        if(l==0xFF){
            m++;
            l = 7;
        }
    }

    while (k < 16)  {
        sum ^= data_byte[k];
        k++;
    }

    checksum = sum;                    // truncate to 16 bits 
    return checksum;
}

// ******************************************************
//              destuffer
//
// Decodes received data back into its original packet 
// form.
// I.E. It removes all the stuffing for the start, stop 
// and stuff bytes.
//
uint8_t destuffer(uint8_t *data, uint8_t *packet){
    uint8_t i = 0;
    uint8_t j = 0;
    //check each byte for stuffing
     while(i<32){
         if(data[i] == STUFF_BYTE){
             i++
             if(data[i] == STUFF_START){
                packet[j] = START_BYTE;
                j++;
             }
             if(data[i] == STUFF_STOP){
                packet[j] = STOP_BYTE;
                j++;
             }
             if(data[i] == STUFF_STUFF){
                packet[j] = STUFF_BYTE;
                j++
             }
         }
         else{
            packet[j] = data[i];
            j++
         }
         i++;
    }
}


// ***************************************************************
//                      stuffer
//
//  Encodes the packet into a transmittable form. Translates any
//  Start, stop or stuff bytes into stuff bytes.
//
uint8_t stuffer(uint64_t **packets, uint64_t **stuffed_packets){
    uint8_t length = sizeof(packets)/sizeof(packets[0]);
    uint8_t data_byte[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t k = 0;
    uint8_t i = 0;
    uint8_t j = 0;

    //break packet into bytes
     while(k < 32){
        data_byte[k] = ((packets[i]>>(j*8))&(0xFF))
            j--;
        k++;
        if(j==0xFF){
            i++;
            j = 7;
        }
    }

    //check each byte for stuffing
     while(i<16){
         if(data_byte[i] == START_BYTE){
             stuffed_packet[j] = STUFF_BYTE;
             j++;
             stuffed_packet[j] = STUFF_START;
         }
         if(data_byte[i]== STOP_BYTE){
             stuffed_packet[j] = STUFF_BYTE;
             j++;
             stuffed_packet[j] = STUFF_STOP;
         }
         if(data_byte[i] == STUFF_BYTE){
             stuffed_packet[j] = STUFF_BYTE;
             j++;
             stuffed_packet[j] = STUFF_STUFF;
         }
         else{
            stuffed_packet[j] = data_byte[i];
            j++;
         }
         i++;
    }
}

// ***********************************************************
//                      transmit
//
// Transmit sends data to the specified location by first
// creating packets of 15 bytes in length, then stuffing them
// and finally transmitting them (also wrapping them in start
// and stop bytes).
//
// NOTE: MAX data can be is 32 bytes
//
uint8_t transmit(uint8_t id, uint8_t type, uint8_t *data)_{
    uint8_t length = sizeof(data)/sizeof(data[0]);
    if(length > MAX_DATA_BYTES){return -1;}
    uint64_t packets[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
    uint64_t stuffed_packets[5][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}

    // Break data into packets
    // includes checksum but does not include START and END bytes
    create_packets(id, type, frame, data, length, packets)

        // while (there is a packet): 
        //      send packet   - Stuff while sending packet
        //      wait for ack
        //      if ack        if nack
        //      check ack     do nothing
        //      store data
        //      increment packet counter

    //Stuff all packets
    stuffer(packets[][],stuffed_packets[][]);

    //send first packet. Rely on Receive() to send remaining packets and handle errors.
    uint8_t i = 0;
    while(i<16){
       if(i<8){
            TRANSMIT(stuffed_packet[0][i]);
       } 
       else{
            TRANSMIT(stuffed_packet[0][i-8]);
       }
    }
}


//
//
uint8_t receive( void ){
    //check transmit flag
    // parse packet
    parse_packet(uint8_t from, uint8_t)
    // if ACK
    // respond with next packet
    //      if end of packets
    //      transmit last packet
    // if NACK
    // resend previous packet
    // If DATA
    // respond with ACK 
    // If NEXT_FRAME
    // increment frame counter


}







