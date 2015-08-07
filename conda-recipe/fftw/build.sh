# double
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-fortran \
    --enable-threads
make
make install

# float
make clean
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-fortran \
    --enable-threads \
    --enable-float
make
make install
