from bisect import bisect
import csv
import sys

if (len(sys.argv) != 3):
    print(f"USAGE: {sys.argv[0]} <korad-log> <drone-log>")
    exit(1)

koradLog   = open(sys.argv[1],  "r")
droneLog   = open(sys.argv[2],  "r")
outputFile = open("output.csv", "w")

csv_writer = csv.writer(outputFile)
csv_writer.writerow(["t", "U", "I", *(f"throttle{i}" for i in range(4))])

throttleData  = []
for line in droneLog:
    t, throttle0, throttle1, throttle2, throttle3 = line.rstrip().split("|")
    throttleData.append([float(t), [throttle0, throttle1, throttle2, throttle3]])

def throttlesAt(t):
    i = max(0, bisect(throttleData, t, key=lambda v: v[0]) - 1)
    return throttleData[i][1]

for line in koradLog:
    if (line.startswith("#")):
        continue

    t, U, I = line.rstrip().split(" ") # rstrip to remove the trailing newline

    throttles = throttlesAt(float(t))
    csv_writer.writerow([t, U, I, *throttles])
