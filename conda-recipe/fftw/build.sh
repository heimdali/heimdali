# float
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-fortran \
    --enable-threads \
    --enable-float
make
make install

# double
make clean
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-fortran \
    --enable-threads \
    --enable-double
make
make install
