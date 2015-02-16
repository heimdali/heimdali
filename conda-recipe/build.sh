export LD_RUN_PATH=""
export INCLUDE_PATH=""
export LIBRARY_PATH=""

mkdir build
cd build

if [ "$TRAVIS" == "true" ]
then
    MAKE_JOBS=2
else
    MAKE_JOBS=$CPU_COUNT
fi

echo $MAKE_JOBS

# CMake detect ITK_DIR as $PREFIX/lib/cmake/ITK-4.7
cmake \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  ..
make -j$MAKE_JOBS
make install
