#!/usr/bin/python3
#
# This script imports a CSV with raw capture data
# from an oscilloscope inspecting gate signals
# in an attempt to yield a variety of gate signal properties.
#

import sys, os

if len(sys.argv) < 2:
    print("Usage: script.py <filename.csv>")
    sys.exit()

filename = sys.argv[1]

if not os.path.exists(filename):
    print("File not found: {:s}".format(filename))
    sys.exit(1)

f = open(filename, "r")

# Skip the header
line = ""
while line.find("Second") < 0:
    line = f.readline()
    if not line:
        f.close()
        print("Error: Unexpected end of file")
        sys.exit(1)

# Load the data
time = []
value = []
fieldSeparator = ","
line = f.readline()
while line:
    s = line.strip().split(fieldSeparator)
    time += [float(s[0])]
    value += [float(s[1])]
    line = f.readline()

f.close()

#
# Find gate voltages
#
print("Extracting minimum and maximum gate voltages:")
min = None
max = None
for i in range(len(value)):
    v = value[i]
    if (min is None) or (v < min):
        min = v
        print(".", end="")
        sys.stdout.flush()
    if (max is None) or (v > max):
        max = v
        print(".", end="")
        sys.stdout.flush()

print("")
print("Minimum voltage: {:f}".format(min))
print("Maximum voltage: {:f}".format(max))

triggerLevel = min + (max - min)/2.0
print("Setting trigger level to {:f} V.".format(triggerLevel))

#
# Find switching frequency
#
print("Attempting to find the switching frequency...")
f = open("switching_frequency.csv", "w")
g = open("dutycycles.csv", "w")
state = (value[0] > triggerLevel)
lastTime = 0.0
period = []
sum = 0.0
dutyStart = 0.0
dutyStop = 0.0
for i in range(len(value)):
    newState = (value[i] > triggerLevel)
    if (state == False) and (newState == True):
        # Rising edge
        p = time[i] - lastTime
        period += [p]
        sum += p
        f.write("{:d}{:s}{:.12f}\n".format(i, fieldSeparator, p))
        if p != 0.0:
            dutycycle = (dutyStop - dutyStart) / p
            if dutycycle < 1.0:
                g.write("{:d}{:s}{:.12f}\n".format(i, fieldSeparator, dutycycle))
        lastTime = time[i]
        dutyStart = time[i]
    elif (state == True) and (newState == False):
        # Falling edge
        dutyStop = time[i]
    state = newState

f.close()
g.close()

meanPeriod = sum/len(period)
print("Mean period/s: {:.12f}".format(meanPeriod))
meanFrequency = 1.0/meanPeriod
print("Mean frequency/Hz: {:.2f}".format(meanFrequency))

#
# Extract the development of the dutycycle over time
#
