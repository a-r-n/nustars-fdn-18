import matplotlib.pyplot as plt
usb_port = '/dev/cu.usbmodem14301' # different for different computers
from time import sleep
import re
from struct import pack, unpack
filename = "test_launch_1/data1.hex"
f = open(filename)
tag_length = 4
PACKET_SIZE = 19
NUM_INT = 1
NUM_DATA = 3
NUM_LONG = 1
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
        l = f.read(tag_length)
        I.append(counter)
        counter += 1
        print(l)
        data = []
        i = 0
        while i < NUM_LONG:
                l=f.read(4)
                data.append(int(l))
                #data.append(unpack('i', l))
        i = 0
        while i < NUM_DATA:
                d=f.read(4)
                data.append(float(d))
                #data.append(unpack('f', d))
                i += 1
        while i < NUM_INT:
                i=f.read(4)
                data.append(int(i))
                #data.append(unpack('l', i))
        print(data)
        A[0].append(data[1])
        A[1].append(data[2])
        ax1.plot(I, A[0])
        fig1.canvas.draw()
        ax2.plot(I, A[1])
        plt.show()
        sleep(.1)