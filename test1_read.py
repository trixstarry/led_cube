import serial

s = serial.Serial('/dev/ttyUSB0')
while True:
	s.read()

