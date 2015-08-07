#!/bin/bash

# Note:
#  requires gfortran


tar xvzf $RECIPE_DIR/inrimage-4.6.5-fonts.tar.gz -C ..

if [ -z "$OSX_ARCH" ]
then
    ./configure \
        --disable-build-fonts \
        --prefix=$PREFIX \
        F77=/usr/bin/gfortran \
        LDFLAGS=-L$PREFIX/lib
else
    ./configure \
        --disable-build-fonts \
        --prefix=$PREFIX \
        F77=$PREFIX/bin/gfortran \
        CFLAGS='-Wno-return-type -std=c89'
fi

make # Note: -j make the build failed.
make install

mkdir -p $PREFIX/include/inrimage
cd $SRC_DIR/src/h/inrimage
cp image *.h $PREFIX/include/inrimage

# Remove this broken symbolic link.
rm $PREFIX/share/inrimage/WWW

# sur mac os x, compile error:

# iouvct.c:205:1: error: conflicting types for 'c_fermct'
# ../h/inrimage/contour.h:189:17: note: previous declaration is here
# void            c_fermct(struct fc_cont *nfc);

