# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mlp

x = []
y1 = []
y2 = []
y3 = []
y4 = []
y5 = []
y_f = []

mlp.style.use('seaborn')

with open("results/times.out") as file:
    i = 0
    add_c = fact_r = D = 0
    for line in file:
        if i == 0:
            D, add_c, fact_r = line.split(" ")
            D = int(D)
            add_c = int(add_c)
            fact_r = float(fact_r)
        elif i%2 == 0:
            a,b,c,d,e,f = line.split(" ")
            x.append(int((i + 1) / 2))
            y1.append(float(a))
            y2.append(float(b))
            y3.append(float(c))
            y4.append(float(d))
            y5.append(float(e))
            y_f.append(float(f))
        i += 1
        
    fig, host = plt.subplots(figsize=(15,10))
    p1 = host.twinx();
    
    host.plot(x, y1, 'C1')
    host.plot(x, y2, 'C2')
    #host.plot(x, y3, 'C3')
    #host.plot(x, y4, 'C4')
    host.plot(x, y5, 'C5')
    p1.plot(x, y_f, 'C6')
    
    host.legend(["Part I in O(n²), Part II in O(n²)",
                 "Part I in O(n²), Part II in O(nlogn)",
                 #"Part I in O(nlogn), Part II in O(n²)",
                 #"Part I in O(nlogn), Part II in O(nlogn)",
                 "Mael's algorithm"],
                 loc="upper left")
    p1.legend(["Failure rate of the algorithms"], loc="upper right")
    
    host.text(900, -1.7, f"Duration of a task : D = {D}\nDeadline max : dmax = (n + {add_c}) * D\nRelease time max : {fact_r} * dmax")

    plt.title("Average scheduling time depending on the number of tasks")
    host.set_xlabel("Number of tasks")
    host.set_ylabel("Average scheduling time in milliseconds")
    host.set_ylim(0)
    p1.set_ylabel("Failure rate of the algorithms (proportion of times where a schedule is not possible)")
    p1.set_ylim(0)
    
    plt.draw()
    plt.savefig("graphs.svg")
