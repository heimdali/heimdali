for f in \
    bin/gfortran \
    bin/x86_64-apple-darwin11.4.2-gfortran \
    lib/libgcc_ext.10.5.dylib \
    lib/libgfortran.spec \
    lib/libstdc++.6.dylib \
    lib/gcc/x86_64-apple-darwin11.4.2/4.8.2/libgcc.a \
    libexec/gcc/x86_64-apple-darwin11.4.2/4.8.2/f951 \
    share/man/man1/gfortran.1
do
    destdir=$PREFIX/$(dirname $f)
    mkdir -p $destdir
    cp $f $destdir
done
