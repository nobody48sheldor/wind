import os
from concurrent.futures import ProcessPoolExecutor

os.system("g++ wind-thermo.cpp -o wind-thermo")

core = int(input("core = "))

executor = ProcessPoolExecutor(max_workers=core)

for i in range(int(100/core)):
    for t in range(core):
        executor.submit(os.system, "./wind-thermo {}".format(core*i + t))

os.system("cp time-* time && python3 time/plot.py")
