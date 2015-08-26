mkdir build
cd build

if [ -z "$OSX_ARCH" ]
then
   config=config.cmake.linux
else
   config=config.cmake.osx
fi

sed s:CONDA_PREFIX:$PREFIX: $RECIPE_DIR/$config > config.cmake

cmake -C config.cmake ..
VERBOSE=1 make -j$CPU_COUNT
make install
