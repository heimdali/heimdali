include_directories(${HEIMDALI_INCLUDE})
include_directories(${ITKINRIMAGEIO_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})

set(CMD_LIST hconv
             par tpr ical
             so ad di mu bi min max
             sc sd lo ra car exp
             extg raz cco melg fzoom
             sba sha
             )
foreach(cmd ${CMD_LIST})
  ADD_EXECUTABLE(${cmd} ${cmd}.cxx)
  TARGET_LINK_LIBRARIES(${cmd}
    ${HEIMDALI_LIBRARY}
    ${ITKINRIMAGEIO_LIBRARY}
    ${ITK_LIBRARIES}
    ${H5UNIXPIPE_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${MINIMAL_TRACEBACK_LIBRARIES}
    )
endforeach(cmd)

install (TARGETS ${CMD_LIST} DESTINATION bin)
