#!/bin/bash

# Notes for OS X:
#   0- XQuartz must be installed
#   1- https://conda.binstar.org/asmeurer is required for pkg-config

PKG_CONFIG_PATH=/usr/X11/lib/pkgconfig/ \
  ./configure \
  --prefix=$PREFIX

make
make install
