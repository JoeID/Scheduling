# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mlp

x = []
y1 = []
y2 = []
y3 = []
y4 = []
y_f = []

mlp.style.use('seaborn')

with open("results/times.out") as file:
    i = 0
    for line in file:
        if i%2 == 1:
            a,b,c,d,e = line.split(" ")
            x.append(int((i + 1) / 2))
            y1.append(float(a))
            y2.append(float(b))
            y3.append(float(c))
            y4.append(float(d))
            y_f.append(float(e))
        i += 1
        
    fig, host = plt.subplots(figsize=(15,10))
    p1 = host.twinx();
    
    host.plot(x, y1, 'C1')
    host.plot(x, y2, 'C2')
    host.plot(x, y3, 'C3')
    host.plot(x, y4, 'C4')
    p1.plot(x, y_f, 'C5')
    
    host.legend(["Part I in O(n²), Part II in O(n²)",
                 "Part I in O(n²), Part II in O(nlogn)",
                 "Part I in O(nlogn), Part II in O(n²)",
                 "Part I in O(n²), Part II in O(n²)"],
                 loc="upper left")
    p1.legend(["Failure rate of the algorithms"], loc="upper right")

    plt.title("Average scheduling time depending on the number of tasks")
    host.set_xlabel("Number of tasks")
    host.set_ylabel("Average scheduyling time in milliseconds")
    p1.set_ylabel("Failure rate of the algorithms")
    
    plt.draw()
    plt.savefig("graphs.png")
