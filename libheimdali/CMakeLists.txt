include_directories(${HEIMDALI_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})

add_library(${HEIMDALI_LIBRARY} SHARED
  cli.cxx
  redirect_stdout.cxx
  regionreader.cxx
  )

target_link_libraries(${HEIMDALI_LIBRARY}
  ${ITK_LIBRARIES}
  ${ITK_LIBRARIES}
  ${H5UNIXPIPE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${MINIMAL_TRACEBACK_LIBRARIES}
  )

install(TARGETS ${HEIMDALI_LIBRARY} DESTINATION lib)
install(DIRECTORY heimdali DESTINATION include)
