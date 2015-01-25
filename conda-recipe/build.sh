mkdir build
cd build

# CMake detect ITK_DIR as $PREFIX/lib/cmake/ITK-4.7
cmake \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  ..
make -j$CPU_COUNT
make install
