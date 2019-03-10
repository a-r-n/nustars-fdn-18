import matplotlib.pyplot as plt
import serial
usb_port = '/dev/cu.usbmodem14301' # different for different computers
ser = serial.Serial(usb_port, 9600)
from time import sleep
import re
from struct import pack, unpack
tag_length = 4
PACKET_SIZE = 19
NUM_DATA = 3
A = []
for i in range(NUM_DATA):
        A.append([])
I = []
plt.ion()
fig1 = plt.figure()
ax1 = fig1.add_subplot(111)
fig2 = plt.figure()
ax2 = fig2.add_subplot(111)
counter = 0
while counter < 1000:
        # search for tag
        l = ser.read(size=tag_length)
        I.append(counter)
        counter += 1
        print(l)
        data = []
        i = 0
        while i < NUM_DATA*4:
                d=ser.read(size=4)
                data.append(unpack('f', d))
                i += 4
        print(data)
        A[0].append(data[1])
        A[1].append(data[2])
        ax1.plot(I, A[0])
        fig1.canvas.draw()
        ax2.plot(I, A[1])
        plt.show()
        sleep(.1)
ser.close()
