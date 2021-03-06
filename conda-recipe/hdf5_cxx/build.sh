#!/bin/bash

./configure \
    --prefix=$PREFIX \
    --enable-cxx \
    --enable-linux-lfs \
    --with-zlib=$PREFIX \
    --with-ssl
make
make install

rm -rf $PREFIX/share/hdf5_examples
