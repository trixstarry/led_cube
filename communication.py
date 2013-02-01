#!/usr/bin/python

import serial

class Communication():
    def __init__(self):
        self.s1 = serial.Serial('/dev/ttyUSB0')
        self.s2 = serial.Serial('/dev/ttyUSB1')

    def Transmit(self,data):
        self.s1.write(data)
        self.s1.write('&')

    def Receive(self):
        data = ""
        item = ""
        while True:
            item = self.s2.read() 
            if item == '&':
                break
            else:
                data += item
        return data

