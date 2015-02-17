export LD_RUN_PATH=""
export INCLUDE_PATH=""
export LIBRARY_PATH=""

mkdir build
cd build

if [ -e "$RECIPE_DIR/travis.txt" ]
then
    CPU_COUNT=2
fi

# CMake detect ITK_DIR as $PREFIX/lib/cmake/ITK-4.7
cmake \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  ..
make -j$CPU_COUNT
make install
