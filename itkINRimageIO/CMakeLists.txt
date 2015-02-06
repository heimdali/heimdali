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

install(TARGETS ${ITKINRIMAGEIO_LIBRARY} DESTINATION lib)
install(FILES
    include/itkINRImageIOFactory.h
    include/itkINRImageIO.h 
    include/itkINRPlugin.h
    DESTINATION include
    )

