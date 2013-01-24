import serial

s = serial.Serial('/dev/ttyUSB0')
s.write("hello")

#while True:
#	s.write("hello")

