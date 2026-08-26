import serial 
import serial.tools.list_ports
import time
import csv
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# List out ports to connect to
ports = serial.tools.list_ports.comports()
print("List of ports: =====================")
for port in ports:
    print(port)

# Connect to respective port or throw error message
ser = serial.Serial('COM6', 460800)

# Initialize the CSV file
header = "timestamp,rxacclX,rxacclY,rxacclZ,rxgyroX,rxgyroY,rxgyroZ,txacclX,txacclY,txacclZ,txgyroX,txgyroY,txgyroZ"
filepath = r"C:\Users\garci\Documents\Projects\26Su_Project\gesture-classifier\receiver\scripts\captures2"
i = 0

# Make sure that this new capture is unique
gesture = "leftjab"
count = 1
filename = f"{gesture}.{count}.csv"

new_file = os.path.join(filepath, f"{gesture}.{count}.csv")

while os.path.exists(new_file):
    count += 1
    new_file = os.path.join(filepath, f"{gesture}.{count}.csv")

new_file = os.path.join(filepath, f"{gesture}.{count}.csv")

# Countdown to data collection
# print("3")
# time.sleep(1)
print("2")
time.sleep(1)
print("1")
time.sleep(1)
print("Reading start!")

# Do 2s (200 sample) data collection
with open(new_file, "w", newline="", encoding="utf-8") as file:
    # main loop
    try:
        file.write(header)
        file.write("\n")
        while i < 1000:
                if ser.in_waiting > 0:
                    val = None
                    # Only read the latest line
                    while ser.in_waiting > 0:
                        try:
                            reading = ser.readline().decode('utf-8').strip()
                            if reading:
                                val = reading
                        except:
                            pass

                    # Catch errors (skip broken lines)
                    if val is None:
                        continue
                    
                    # Write data to CSV
                    file.write(f"{i * 10},")
                    file.write(reading)
                    file.write("\n")
                    i += 1
        print("Reading done!")

    except KeyboardInterrupt:
        ser.close()

# TODO: Plot onto six different Matplotlib windows
# Use Pandas to read in CSV that we took
df = pd.read_csv(new_file)

fig, ax = plt.subplots(3, 4)

x1 = df['rxacclX']
plt.subplot(3, 4, 1)
plt.plot(x1)

x2 = df['rxacclY']
plt.subplot(3, 4, 5)
plt.plot(x2)

x3 = df['rxacclZ']
plt.subplot(3, 4, 9)
plt.plot(x3)

x4 = df['rxgyroX']
plt.subplot(3, 4, 2)
plt.plot(x4)

x5 = df['rxgyroY']
plt.subplot(3, 4, 6)
plt.plot(x5)

x6 = df['rxgyroZ']
plt.subplot(3, 4, 10)
plt.plot(x6)

x7 = df['txacclX']
plt.subplot(3, 4, 3)
plt.plot(x7)

x8 = df['txacclY']
plt.subplot(3, 4, 7)
plt.plot(x8)

x9 = df['txacclZ']
plt.subplot(3, 4, 11)
plt.plot(x9)

x10 = df['txgyroX']
plt.subplot(3, 4, 4)
plt.plot(x10)

x11 = df['txgyroY']
plt.subplot(3, 4, 8)
plt.plot(x11)

x12 = df['txgyroZ']
plt.subplot(3, 4, 12)
plt.plot(x12)


# Set titles for all subplots
ax[0, 0].set_title("rx acclX (m/s^2)")
ax[1, 0].set_title("rx acclY (m/s^2)")
ax[2, 0].set_title("rx acclZ (m/s^2)")

ax[0, 1].set_title("rx gyroX (deg/s)")
ax[1, 1].set_title("rx gyroY (deg/s)")
ax[2, 1].set_title("rx gyroZ (deg/s)")

ax[0, 2].set_title("tx acclX (m/s^2)")
ax[1, 2].set_title("tx acclY (m/s^2)")
ax[2, 2].set_title("tx acclZ (m/s^2)")

ax[0, 3].set_title("tx gyroX (deg/s)")
ax[1, 3].set_title("tx gyroY (deg/s)")
ax[2, 3].set_title("tx gyroZ (deg/s)")

# Set all subplots y-axes to be normalized with each other.
# Make sure accel measurements have a range DIFFERENT than gyroscope.
ax[1, 0].sharey(ax[0, 0])
ax[2, 0].sharey(ax[0, 0])

ax[0, 1].sharey(ax[0, 1])
ax[1, 1].sharey(ax[0, 1])
ax[2, 1].sharey(ax[0, 1])

ax[0, 2].sharey(ax[0, 0])
ax[1, 2].sharey(ax[0, 0])
ax[2, 2].sharey(ax[0, 0])

ax[0, 3].sharey(ax[0, 1])
ax[1, 3].sharey(ax[0, 1])
ax[2, 3].sharey(ax[0, 1])

plt.subplots_adjust(hspace=0.6) 
plt.show()

