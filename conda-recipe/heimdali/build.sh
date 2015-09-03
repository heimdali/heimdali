export LD_RUN_PATH=""
export INCLUDE_PATH=""
export LIBRARY_PATH=""

mkdir build
cd build

if [ "$TRAVIS" == "true" ]
then
    echo "On Travis, set CPU_COUNT to 2 cores"
    CPU_COUNT=2
fi

# CMake detect ITK_DIR as $PREFIX/lib/cmake/ITK-4.7
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  ..

# Build
LD_LIBRARY_PATH=$PREFIX/lib VERBOSE=1 make -j$CPU_COUNT

make install

# Fix absolute path to system libaries, that are different for example on
# Ubuntu.
if [ -z "$OSX_ARCH" ]
then
    for config in $(find $PREFIX -name '*.cmake')
    do
        sed -i s:/usr/lib64/libm.so:libm.so:g   $config
        sed -i s:/usr/lib64/libdl.so:libdl.so:g $config
        sed -i s:/usr/lib64/librt.so:librt.so:g $config
    done
fi
