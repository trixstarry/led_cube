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
            self.s1 = serial.Serial(port[0],baudrate=38400,timeout=.15)
        except serial.serialutil.SerialException:
            print "ERROR: Could not open serial Connection on USB0"
            try:
                self.s1 = serial.Serial(port[1],baudrate=38400,timeout=.15)
            except serial.serialutil.SerialException:
                print "ERROR: Could not open serial Connection on USB1"
                try:
                    self.s1 = serial.Serial(port[2],baudrate=38400,timeout=.15)
                except serial.serialutil.SerialException:
                    print "ERROR: Could not open serial Connection on USB2"
                    try:
                        self.s1 = serial.Serial(port[3],baudrate=38400,timeout=.15)
                    except serial.serialutil.SerialException:
                        print "ERROR: Could not open serial Connection on USB3"
                        print "ERROR: Could not find LEDCUBE USB Dongle."


    def Transmit(self,data):
        self.s1.write(data)
        #self.s1.write('&')

    def Receive(self):
        data = "" first = True
        while(True):
            data = self.s1.readline()
            #print data
            if not data:
                if first == True:
                    return -2
                first = False

            #print "*********************"
            #print [int(x.encode('hex'),16) for x in data]
            try:
                if data == 'b\n':
                    #print data
                    return 0
                if ((data == 'a\n')):
                    while(1):
                        data = self.s1.readline() 
                        if data == "c\n":
                            data = [int(x.encode('hex'),16) for x in self.s1.readline()] 
                        #print "sensor input is:"
                        #print data[2]
                            return data[2]
            except:
                #data = []
                data = [-1,-1,-1]
            #print data

        '''data = self.s1.readline()
        data = [int(x.encode('hex'),16) for x in data]
        print data

        junk = self.s1.readline()
        print [int(x.encode('hex'),16) for x in junk]
        '''
        #return data

