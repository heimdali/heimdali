include_directories(${ITKINRIMAGEIO_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})

set(CMD_LIST_0 par ical)
foreach(cmd ${CMD_LIST_0})
  ADD_EXECUTABLE(${cmd} ${cmd}.cxx cli.cxx redirect_stdout.cxx)
  TARGET_LINK_LIBRARIES(${cmd} 
    ${ITKINRIMAGEIO_LIBRARY}
    ${ITK_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
endforeach(cmd)

set(CMD_LIST_1 so inrtoh5 h5toinr)
foreach(cmd ${CMD_LIST_1})
  ADD_EXECUTABLE(${cmd} ${cmd}.cxx cli.cxx regionreader.cxx)
  TARGET_LINK_LIBRARIES(${cmd}
    ${ITKINRIMAGEIO_LIBRARY}
    ${ITK_LIBRARIES}
    ${H5UNIXPIPE_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${MINIMAL_TRACEBACK_LIBRARIES}
    )
endforeach(cmd)

install (TARGETS ${CMD_LIST_0} ${CMD_LIST_1} DESTINATION bin)
