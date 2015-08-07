#!/bin/bash

if [ -z "$OSX_ARCH" ]
then
  FLAGS="LDFLAGS=-L$PREFIX/lib F77=/usr/bin/gfortran"
else
  FLAGS="CFLAGS=-Wno-return-type F77=$PREFIX/bin/gfortran"
fi

./configure \
    --disable-build-fonts \
    --disable-libtiff \
    --disable-netpbm \
    --disable-libjpeg \
    --disable-libpng \
    --enable-shared=yes \
    --enable-static=no \
    --prefix=$PREFIX \
    $FLAGS
cd src/inrimage
make
make install

mkdir -p $PREFIX/include/inrimage
cd $SRC_DIR/src/h/inrimage
cp image *.h $PREFIX/include/inrimage
