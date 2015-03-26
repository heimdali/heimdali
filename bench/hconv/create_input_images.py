#!/usr/bin/env python
import sys

sys.path.append("../../tests/features")
import terrain

from subprocess import check_call
import os
from numpy import *

k = 1000
nv = 3

first_size = 100
last_size = 50*k*k
number_of_images = 20

data_dir = terrain.get_data_dir()
exe = os.path.join(data_dir, "build", "WriteHDF5Imtest")

def compare(expected,actual):
    print expected, actual, percent(expected,actual)

def percent(expected,actual):
    return (float(actual) / expected) * 100

for nzyxv_approx in linspace(first_size, last_size, number_of_images):
    nzyxv = int(nzyxv_approx)
    nzyx = nzyxv / nv
    nz = ny = nx = int(nzyx**(1./3.))
    #compare(nzyxv_approx, nz*ny*nx*nv)
    cmd = "%s %i %i %i %i" % (exe, nz, ny, nx, nv)
    check_call(cmd.split())
