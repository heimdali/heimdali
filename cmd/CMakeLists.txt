set(CMD_LIST par)

foreach(cmd ${CMD_LIST})
  ADD_EXECUTABLE(${cmd} ${cmd}.cxx cli.cxx redirect_stdout.cxx)
  TARGET_LINK_LIBRARIES(${cmd} ${ITK_LIBRARIES} ${H5UNIXPIPE_LIBRARIES} ${HDF5_LIBRARIES})
endforeach(cmd)

install (TARGETS ${CMD_LIST} DESTINATION bin)

