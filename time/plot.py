import matplotlib.pyplot as plt
import numpy as np
from matplotlib import style
from matplotlib import cm
from mpl_toolkits.mplot3d import axes3d
import time
import os

style.use('dark_background')

n = 40 

time = np.zeros(101)
speed = np.zeros(101)

for i in os.listdir("."):
    if i != "plot.py":
        with open(i) as file:
            data = file.read()
            s = int(data.split("/",2)[-1])
            t = float(data.split("/",2)[0])
            print(t, "    ", s)
            time[s] = 1/t
            speed[s] = s


print(time)
print(speed)

plt.scatter(speed, time)
plt.show()
plt.plot(speed, time)
plt.show()
