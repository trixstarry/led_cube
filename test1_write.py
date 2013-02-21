import serial
#import time

print "1"
s = serial.Serial('/dev/ttyUSB0')
#s.write("hello")
print "2"

while True:
    print "3"
	s.write("hello")
    print "hello"
    #time.sleep(1)

