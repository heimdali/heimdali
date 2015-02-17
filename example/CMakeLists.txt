cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
project(heimdali)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

# Search for ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# TCLAP include
find_path(TCLAP_INCLUDES tclap/CmdLine.h)

# h5unixpipe libraries
find_library(H5UNIXPIPE_LIBRARY NAMES h5unixpipe)
find_library(H5UNIXPIPE_CXX_LIBRARY NAMES h5unixpipe_cxx)
set(H5UNIXPIPE_LIBRARIES
  ${H5UNIXPIPE_LIBRARY}
  ${H5UNIXPIPE_CXX_LIBRARY}
  )

# h5unixpipe include
find_path(H5UNIXPIPE_INCLUDE h5unixpipe.h)

# minimal traceback
find_library(MINIMAL_TRACEBACK_LIBRARIES NAMES minimal_traceback)

# HDF5 libraries
find_library(HDF5_HL_CPP_LIBRARY NAMES hdf5_hl_cpp)
find_library(HDF5_HL_LIBRARY NAMES hdf5_hl)
find_library(HDF5_CPP_LIBRARY NAMES hdf5_cpp)
find_library(HDF5_LIBRARY NAMES hdf5)
set(HDF5_LIBRARIES
  ${HDF5_HL_CPP_LIBRARY}
  ${HDF5_HL_LIBRARY}
  ${HDF5_CPP_LIBRARY}
  ${HDF5_LIBRARY}
  )

# INRimage include.
find_path(INRIMAGE_INCLUDE inrimage/image.h)

# INRimage library.
find_library(INRIMAGE_LIBRARY NAMES inrimage)

# INRImageIO library
find_library(ITKINRIMAGEIO_LIBRARY NAMES itkINRImageIO)
find_path(ITKINRIMAGEIO_INCLUDE itkINRImageIO.h)

# Heimdali library
find_library(HEIMDALI_LIBRARY heimdali)
find_path(HEIMDALI_INCLUDE heimdali/itkhelper.hxx)

include_directories(${HEIMDALI_INCLUDE})
include_directories(${ITKINRIMAGEIO_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})

ADD_EXECUTABLE(createInputImage createInputImage.cxx)
ADD_EXECUTABLE(inrimage_read inrimage_read.cxx)

TARGET_LINK_LIBRARIES(createInputImage 
  ${HEIMDALI_LIBRARY}
  ${ITKINRIMAGEIO_LIBRARY}
  ${INRIMAGE_LIBRARY}
  ${ITK_LIBRARIES}
  ${H5UNIXPIPE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${MINIMAL_TRACEBACK_LIBRARIES}
  )

TARGET_LINK_LIBRARIES(inrimage_read 
  ${HEIMDALI_LIBRARY}
  ${ITKINRIMAGEIO_LIBRARY}
  ${INRIMAGE_LIBRARY}
  ${ITK_LIBRARIES}
  ${H5UNIXPIPE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${MINIMAL_TRACEBACK_LIBRARIES}
  )
