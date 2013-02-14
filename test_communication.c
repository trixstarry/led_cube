//This file tests communication.c's methods
//
//
//
//

#include "communication.c"
#include "avr/io.h"





extern int8_t create_packets(uint8_t to, uint8_t subject, uint16_t frame, uint8_t *data, uint8_t data_len, uint64_t **packets);

test_create_packets(){
    uint8_t data[10] = {0,0,0,0,0,0,0,0,0,0};

    //check length algorithm
    length = sizeof(data)/sizeof(data[0]);


    //test

}

uint8_t main(){
    while(1){
        test_create_packets();
    }
}
