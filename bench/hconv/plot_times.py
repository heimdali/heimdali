#!/usr/bin/env python

import os
import re
import matplotlib.pyplot as p
p.switch_backend('agg')
from pylab import *

def extract_dims_from_filename(filename):
    s = "imtest_z(?P<z>\d+)_y(?P<y>\d+)_x(?P<x>\d+)_c(?P<c>\d+)\.*"
    regexp = re.compile(s)
    d = re.search(regexp, filename).groupdict()
    nz,ny,nx,nc = [int(d[name]) for name in 'z y x c'.split()]
    return nz,ny,nx,nc

def extract_dims_from_cmd(line):
    exe, input_filename, output_filename = line.split()
    nz,ny,nx,nc = extract_dims_from_filename(input_filename)
    return nz,ny,nx,nc

def extract_times(line):
    s = "real: 0:(?P<real>.*) user: (?P<user>.+?) sys: (?P<sys>.+?)$"
    regexp = re.compile(s)
    d = re.search(regexp, line.strip()).groupdict()
    return [float(d[name]) for name in "real user sys".split()]

symbols = ['o', 'x']

for cmd,symbol in zip(['h52inr', 'hconv'], symbols):

    filename = "%s_h5_inr.txt" % cmd

    # Read result file.
    lines = open(filename).readlines()
    cmd_lines = lines[::2]
    time_lines = lines[1::2]

    # Extract image sizes.
    sizes = []
    for line in cmd_lines:
        nz,ny,nx,nc = extract_dims_from_cmd(line)
        size = nz*ny*nx
        sizes.append(size)

    # Extract times
    reals, users, syss = [], [], []
    for line in time_lines:
        real,user,sys = extract_times(line)
        reals.append(real)
        users.append(user)
        syss.append(sys)

    # Plot real time
    plot(sizes, reals, 'b'+symbol, label = cmd + ' real')
    plot(sizes, users, 'g'+symbol, label = cmd + ' user')
    plot(sizes, syss, 'r'+symbol, label = cmd + ' sys')
    print "plot"

xlabel('Number of pixels (each has 3 components)')
ylabel('Time in seconds')
legend()

savefig("time.png")
print 'time.png'
