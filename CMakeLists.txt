cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
project(heimdali)

SET(CMAKE_BUILD_TYPE debug)
enable_testing()

# Search for ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# Search TCLAP
find_path(TCLAP_INCLUDES tclap/CmdLine.h)

# Search for H5UNIXPIPE
find_library(H5UNIXPIPE_LIBRARIES NAMES h5unixpipe h5unixpipe_cxx)
if(H5UNIXPIPE_LIBRARIES)
    MESSAGE("h5unxipipe found.")
else(H5UNIXPIPE_LIBRARIES)
    MESSAGE(FATAL_ERROR "h5unxipipe not found.")
endif(H5UNIXPIPE_LIBRARIES)

# Search for HDF5
find_library(HDF5_LIBRARIES NAMES hdf5 hdf5_hl)

add_subdirectory(cmd)
