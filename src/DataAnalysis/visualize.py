import matplotlib.pyplot as plt
import numpy as np
import csv
from sys import argv

if (len(argv) < 2):
    print(f"Usage: {argv[0]} <merged.csv>")
    exit(1)

csv_reader = csv.DictReader(open(argv[1], "r"), ["t", "U", "I", *(f"throttle{i}" for i in range(4))])

csv_reader.__next__() # discard headers

t  = []
U  = []
I  = []
t0 = []
t1 = []
t2 = []
t3 = []

time0 = 0;

for i, d in enumerate(csv_reader):
    if (i == 0):
        time0 = float(d["t"])

    t.append (float(d["t"]) - time0)
    U.append (float(d["U"]))
    I.append (float(d["I"]))
    t0.append(int(d["throttle0"]))
    t1.append(int(d["throttle1"]))
    t2.append(int(d["throttle2"]))
    t3.append(int(d["throttle3"]))

# plot
red   = "red"
black = "black"

tcolors = ["blue", "green", "orange", "yellow"]

fig, ax_U = plt.subplots(figsize=(1920/96, 1080/96), dpi=96)

ax_U.set_xlabel("time (s)")
ax_U.set_ylabel("U (V)", color=red)
ax_U.plot(np.array(t), np.array(U), 'o-', linewidth=2, color=red)

ax_U.set_xlim(0, max(t))
ax_U.tick_params(axis="y", labelcolor=red)

ax_I = ax_U.twinx()
ax_I.set_ylabel("I (A)", color=black)
ax_I.plot(np.array(t), np.array(I), 'o-', linewidth=2, color=black)

ax_I.tick_params(axis="y", labelcolor=black)

for i, tlist in enumerate([t0, t1, t2, t3]):
    ax = ax_U.twinx()
#    ax.set_ylabel(f"throttle{i} (0 - 255)", color=tcolors[i])
    ax.plot(np.array(t), np.array(tlist), 'o-', linewidth=2, color=tcolors[i])

    ax.tick_params(axis="y", length=0, labelsize=0)
    ax.set_ylim(0, 255)

plt.savefig("diagram.png")
plt.show()

