import matplotlib.pyplot as plt
import numpy as np
from matplotlib import style
from matplotlib import cm
from mpl_toolkits.mplot3d import axes3d
import time

style.use('dark_background')

n = 40 

X = []
Y = []

with open("data.txt", 'r') as file:
    data = file.readlines()

data = data[0].split("/",n*n)

for i in range(len(data)-1):
    data[i] = float(data[i])

for y in range(n):
    X = []
    for x in range(n):
        X.append(data[int(y*n + x)])
    Y.append(X)

array = np.array(Y)

X = np.linspace(-5, 5, n)
Y = np.linspace(-5, 5, n)

x, y = np.meshgrid(X, Y)



#plt.savefig("{}".format(time))

#plt.imshow(Y)

fig = plt.figure()
plt.clf()
ax = fig.add_subplot(111, projection = '3d')

ax.plot_surface(x, y, array, cmap = cm.plasma, linewidth=0, antialiased=True)

ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')

ax.axes.set_xlim3d(left=-5, right=5)
ax.axes.set_ylim3d(bottom=-5, top=5)
ax.axes.set_zlim3d(bottom=-1.2, top=1.2)

plt.title('Time = 0 secondes')

plt.savefig("renders/{}.png".format(time.time()))
