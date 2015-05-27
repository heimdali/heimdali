include_directories(${ITKINRIMAGEIO_INCLUDE})
include_directories(${INRIMAGE_INCLUDE})

add_library(${ITKINRIMAGEIO_LIBRARY} SHARED
  src/itkINRImageIO.cxx
  src/itkINRImageIOFactory.cxx
  )

target_link_libraries(${ITKINRIMAGEIO_LIBRARY}
  ${ITK_LIBRARIES}
  ${INRIMAGE_LIBRARY}
)

install(TARGETS ${ITKINRIMAGEIO_LIBRARY} 
    DESTINATION lib
    EXPORT ITKINRIMAGEIO_LIBRARY
)

install(FILES
    include/itkINRImageIOFactory.h
    include/itkINRImageIO.h 
    include/itkINRPlugin.h
    DESTINATION include
    )

# Install exports.
install(
    EXPORT ITKINRIMAGEIO_LIBRARY
    DESTINATION ${HEIMDALI_CONFIGDIR_FROM_PREFIX}
)

# Make config file available in develop mode (build but no install)
export(
    TARGETS ${ITKINRIMAGEIO_LIBRARY}
    FILE ${HEIMDALI_BINARY_DIR}/ITKINRIMAGEIO_LIBRARY.cmake
)
