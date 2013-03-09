#!/usr/bin/python

import serial
import os

class Communication():
    def __init__(self):
        if os.name == 'nt':
            port = ['COM1','COM2','COM3','COM4']
        else:
            port = ['/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3']
        try:
            self.s1 = serial.Serial(port[0],baudrate=38400)
        except serial.serialutil.SerialException:
            print "ERROR: Could not open serial Connection on USB0"
            try:
                self.s1 = serial.Serial(port[1],baudrate=38400)
            except serial.serialutil.SerialException:
                print "ERROR: Could not open serial Connection on USB1"
                try:
                    self.s1 = serial.Serial(port[2],baudrate=38400)
                except serial.serialutil.SerialException:
                    print "ERROR: Could not open serial Connection on USB2"
                    try:
                        self.s1 = serial.Serial(port[3],baudrate=38400)
                    except serial.serialutil.SerialException:
                        print "ERROR: Could not open serial Connection on USB3"
                        print "ERROR: Could not find LEDCUBE USB Dongle."


    def Transmit(self,data):
        self.s1.write(data)
        #self.s1.write('&')

    def Receive(self):
        data = ""
        item = ""
        #while True:
        data = self.s1.readline() 
        data = self.s1.readline()
            #if item == '&':
            #    break
            #else:
            #    data += item
        return data

