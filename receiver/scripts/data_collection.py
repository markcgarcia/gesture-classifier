import serial 
import serial.tools.list_ports
import time
import csv
import numpy as np
import matplotlib.pyplot as plt

# List out ports to connect to
ports = serial.tools.list_ports.comports()
print("List of ports: =====================")
for port in ports:
    print(port)

# Connect to respective port or throw error message
ser = serial.Serial('COM6', 9600)

# Wait for initialization
time.sleep(2)

# Initialize the CSV file
header = "AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ"
filepath = r"C:\Users\garci\Documents\Projects\26Su_Project\gesture-classifier\receiver\scripts"
i = 0

# Countdown to data collection
print("3")
time.sleep(1)
print("2")
time.sleep(1)
print("1")
time.sleep(1)
print("Reading start!")

# Do 2s (200 sample) data collection
with open("out.csv", "w", newline="", encoding="utf-8") as file:
    # main loop
    try:
        file.write(header)
        file.write("\n")
        while i < 200:
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
                    file.write(reading)
                    file.write("\n")
                    i += 1
        print("Reading done!")

    except KeyboardInterrupt:
        ser.close()

# Plot onto six different Matplotlib windows