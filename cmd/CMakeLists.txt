include_directories(${HEIMDALI_INCLUDE})
include_directories(${ITKINRIMAGEIO_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})

set(CMD_LIST hconv
             par tpr ical itest
             so ad di mu bi min max
             sc sd lo ra car exp
             extg raz cco melg fzoom izoom
             sba sha vb vh mb mh muls norma
             mo carflo fixflo flofix logic
             cim asm rot
             detc
             med rdf ddf idf ri ma conv fmoy ce
             regionreader
             heiminfo inrinfo
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

add_custom_command(TARGET rdf POST_BUILD COMMAND ln -s rdf rmo)
add_custom_command(TARGET ddf POST_BUILD COMMAND ln -s ddf dmo)
add_custom_command(TARGET idf POST_BUILD COMMAND ln -s idf imo)

install(TARGETS ${CMD_LIST} DESTINATION bin)
install(PROGRAMS rmo dmo imo DESTINATION bin)
