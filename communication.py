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


    #NOTE: must input channel as a hex char like so: '\x01' from 1 to 84 '\0x54'
    def set_channel(self,channel):
        data = ''.join(("\x01",channel))
        for i in range(31):
            data = ''.join((data,'\x00'))
        self.Transmit(data)

#    def Transmit(self,data):
#        self.s1.write(data)
#        #self.s1.write('&')

    def Transmit (self, data):
        if (len (data) != 33):
            print ("Cannot send data: length is not 33:" + data)
            return -1

        while (True):
            self.s1.write(data)
            rx = self.s1.readline()
            if (rx == 'a\n'):
                return 1
            if (rx != 'b\n'):
                print ("unexpected byte from serial: [" + rx + "]")
                return -2


    def Receive(self):
        data = self.s1.readline() 
        if data == "c\n":
            data = [int(x.encode('hex'),16) for x in self.s1.read(32)]
            #print "data = "
            print data
            print "Reading leftovers: " + self.s1.readline()
            print "Done!"
        else:
            data = ''.join([int(x.encode('hex'),16) for x in self.s1.read(32)])
            print ("Data was not expected: [" + data + "]")
            return -1
        return data[2]

        data = ""
        first = True
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
                            data = [int(x.encode('hex'),16) for x in self.s1.read(32)]
                            print data
                            print "Reading leftovers: " + self.s1.readline()
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

