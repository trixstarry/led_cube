//This file tests communication.c's methods
//
//

#include "communication.h"

//#include "mirf.h"
//#include "nRF24L01.h"
//#include "avr/io.h"
//
//adsf


/*
int8_t test_create_packets( void ){
    uint8_t data[10] = {0,0,0,0,0,0,0,0,0,0};
    uint8_t length = 0;
    //check length algorithm
    length = sizeof(data)/sizeof(data[0]);
    //test
    return 1;
}
*/

//
uint8_t main()
{
    uint8_t data[10] = {0,0,0,0,0,0,0,0,0,0};
    uint8_t subject = DATA;
    uint8_t id = ID_CUBE1;
    uint8_t i = 0;

    while(1){
        i = transmit(id,subject,data);
    }
    return 1;
}
