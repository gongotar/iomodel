#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May 23 22:29:40 2022

@author: gongotar
"""

# import matplotlib.pyplot as plt
from scipy import stats

# x = [89,43,36,36,95,10,66,34,38,20,26,29,48,64,6,5,36,66,72,40]
# y = [21,46,3,35,67,95,53,72,58,10,26,34,90,33,38,20,56,2,47,15]

file = "tmp_data"


x = [float(t.split('\t')[0]) for t in open(file).readlines()]
y = [float(t.split('\t')[1]) for t in open(file).readlines()]


slope, intercept, r, p, std_err = stats.linregress(x, y)

# # slope, intercept = 3.892174691830716e-09, 0.00023141935483870965

# def myfunc(x):
#   return slope * x + intercept

# mymodel = list(map(myfunc, x))

# plt.scatter(x, y)
# plt.plot(x, mymodel,  color="red")
# plt.show() 

# print (1.0/(slope*1024*1024*1024), intercept)

with open("py_out", "w") as outfile:
    # Writing data to a file
    outfile.write(str(slope) + " " + str(intercept))