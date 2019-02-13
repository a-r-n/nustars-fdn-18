import serial
from time import sleep
usb_port = 'COM15'
ser = serial.Serial(usb_port, 9600)
counter = 32
while True:
    counter += 1
    ser.write(counter)
	print("%i\n" % counter)
	print(ser.readline())
	sleep(.1)
	if counter == 255:
		counter = 32