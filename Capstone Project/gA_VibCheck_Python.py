## VibCheck_Python.py
## Author: Lisler Thomson Pulikkottil; 
##		Nivedita Rajendran;
##		Shadaab Saiyed
## Description: This program takes the readings from sparkfun pro
##			RF through serial USB port and displays it on a
##			graph. It will also store the readings to a file

import time  
import serial
import matplotlib.pyplot as plt

# Initialising the Gragph
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
xs = range(0,500)
ys = []

# Initialising Serial Port
ser = serial.Serial(
           
    port='/dev/ttyACM0',
    baudrate = 9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
# Storing Sensor data into an array
for t in range(0,500):
    data = ser.readline()
    print(float(data))
    ys.append(float(data))

# PLotting sensor readings to graph
ax.plot(xs,ys)

# Save plot
plt.savefig('datagraph.jpg')

# Show plot
plt.show()

# Open a file and store the sensor data
f = open("test1.txt","a+")
details = input("Enter Details:")
f.write(details)
f.write("\n")
f.write(str(ys))
f.write("\n")
f.close()