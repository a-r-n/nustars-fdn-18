import matplotlib.pyplot as plt
import serial
usb_port = '/dev/cu.usbmodem14301'
ser = serial.Serial(usb_port, 9600)
from time import sleep

counter = 0
l = ser.readline()

while l != b'BEGIN\r\n':
	l = ser.readline()
l = ser.readline()


A = [[],[]]
I = []
plt.ion()
fig1 = plt.figure()
ax = fig1.add_subplot(111)

while counter < 10:
	I.append(counter)
	counter += 1
	print(l)
	data = l.split();
	A[0].append(int(data[1]))
	A[1].append(int(data[2]))
	ax.plot(I, A[0])
	fig1.canvas.draw()

	l = ser.readline()
	sleep(.1)
ser.close()