import matplotlib.pyplot as plt

A = []
I = []
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
for i in range(10):
    I.append(i)
    A.append(i**2);
    print(i)
    ax.plot(I, A)
    fig.canvas.draw()