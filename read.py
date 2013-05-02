#!/usr/bin/python

import serial

s = serial.Serial('/dev/ttyUSB0',baudrate=38400)
'''
data = ''.join(("\x01",'\x3c'))
for i in range(31):
    data = ''.join((data,'\x00'))
while (True):
    s.write(data)
    rx = s.readline()
    if (rx == 'a\n'):
        break; 
    if (rx != 'b\n'):
        print ("unexpected byte from serial: [" + rx + "]")
        break;
        '''
while(1):
    s.readline()

