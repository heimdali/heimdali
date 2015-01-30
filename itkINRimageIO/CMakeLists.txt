include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INRIMAGE_INCLUDE})

add_library(itkINRImageIO SHARED
  src/itkINRImageIO.cxx
  src/itkINRImageIOFactory.cxx
  )

target_link_libraries(itkINRImageIO
    ${ITK_LIBRARIES}
    ${INRIMAGE_LIBRARY}
)
