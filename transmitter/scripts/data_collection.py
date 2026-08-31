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
header = "timestamp,acclX,acclY,acclZ,gyroX,gyroY,gyroZ"
filepath = r"C:\Users\garci\Documents\Projects\26Su_Project\gesture-classifier\receiver\scripts\captures"
i = 0

# Make sure that this new capture is unique
gesture = "chop"
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

fig, ax = plt.subplots(3, 2)

x1 = df['acclX']
plt.subplot(3, 2, 1)
plt.plot(x1)

x2 = df['acclY']
plt.subplot(3, 2, 3)
plt.plot(x2)

x3 = df['acclZ']
plt.subplot(3, 2, 5)
plt.plot(x3)

x4 = df['gyroX']
plt.subplot(3, 2, 2)
plt.plot(x4)

x5 = df['gyroY']
plt.subplot(3, 2, 4)
plt.plot(x5)

x6 = df['gyroZ']
plt.subplot(3, 2, 6)
plt.plot(x6)


# titles and settings
ax[0, 0].set_title("acclX (m/s^2)")
ax[1, 0].set_title("acclY (m/s^2)")
ax[2, 0].set_title("acclZ (m/s^2)")

ax[0, 1].set_title("gyroX (deg/s)")
ax[1, 1].set_title("gyroY (deg/s)")
ax[2, 1].set_title("gyroZ (deg/s)")

ax[1, 0].sharey(ax[0, 0])
ax[2, 0].sharey(ax[0, 0])

ax[1, 1].sharey(ax[0, 1])
ax[2, 1].sharey(ax[0, 1])


plt.subplots_adjust(hspace=0.6) 

plt.show()

