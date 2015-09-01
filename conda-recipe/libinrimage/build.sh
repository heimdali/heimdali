#!/bin/bash

if [ -z "$OSX_ARCH" ]
then
    ./configure \
        F77="$PREFIX/bin/gfortran" \
        LDFLAGS="-L$PREFIX/lib" \
        --disable-build-fonts \
        --disable-libtiff \
        --disable-netpbm \
        --disable-libjpeg \
        --disable-libpng \
        --disable-Xaw3d \
        --enable-shared=yes \
        --enable-static=no \
        --prefix=$PREFIX
else
    ./configure \
        F77="$PREFIX/bin/gfortran" \
        CFLAGS="-Wno-return-type" \
        --disable-build-fonts \
        --disable-libtiff \
        --disable-netpbm \
        --disable-libjpeg \
        --disable-libpng \
        --disable-Xaw3d \
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
