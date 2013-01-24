import serial

s = serial.Serial('/dev/ttyUSB1')
while True:
	s.read()

