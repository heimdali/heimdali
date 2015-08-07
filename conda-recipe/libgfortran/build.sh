for f in \
    lib/libgfortran.dylib \
    lib/libgfortran.3.dylib \
    lib/libquadmath.0.dylib \
    lib/libquadmath.dylib \
    lib/libgcc_s.1.dylib
do
    destdir=$PREFIX/$(dirname $f)
    mkdir -p $destdir
    cp $f $destdir
done

