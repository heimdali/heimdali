#!/bin/bash


if [ -z "$OSX_ARCH" ]
then
    ./configure \
        CFLAGS="-g -O0" \
        FFLAGS="-g -O0" \
        LDFLAGS"=-L$PREFIX/lib" \
        F77"=/usr/bin/gfortran" \
        --disable-build-fonts \
        --disable-libtiff \
        --disable-netpbm \
        --disable-libjpeg \
        --disable-libpng \
        --enable-shared=yes \
        --enable-static=no \
        --prefix=$PREFIX
else
    ./configure \
        CFLAGS="-g -O0 -Wno-return-type" \
        FFLAGS="-g -O0" \
        F77="$PREFIX/bin/gfortran" \
        --disable-build-fonts \
        --disable-libtiff \
        --disable-netpbm \
        --disable-libjpeg \
        --disable-libpng \
        --enable-shared=yes \
        --enable-static=no \
        --prefix=$PREFIX
fi

cd src/inrimage
make
make install

mkdir -p $PREFIX/include/inrimage
cd $SRC_DIR/src/h/inrimage
cp image *.h $PREFIX/include/inrimage
