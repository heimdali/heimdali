include_directories(${HEIMDALI_INCLUDE})
include_directories(${H5UNIXPIPE_INCLUDE})
include_directories(${ITKINRIMAGEIO_INCLUDE})

add_library(
    ${HEIMDALI_LIBRARY} SHARED
    cli.cxx
    redirect_stdout.cxx
    regionreader.cxx
    itkhelper.cxx
    cmdhelper.cxx
    util.cxx
)

target_link_libraries(
    ${HEIMDALI_LIBRARY}
    ${ITK_LIBRARIES}
    ${ITK_LIBRARIES}
    ${H5UNIXPIPE_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${MINIMAL_TRACEBACK_LIBRARIES}
)

install(
    TARGETS ${HEIMDALI_LIBRARY} 
    DESTINATION lib
    EXPORT HEIMDALI_LIBRARY
)
install(
    DIRECTORY heimdali
    DESTINATION include
)

# Install exports.
install(
    EXPORT HEIMDALI_LIBRARY
    DESTINATION ${HEIMDALI_CONFIGDIR_FROM_PREFIX}
)

# Make config file available in develop mode (build but no install)
export(
    TARGETS ${HEIMDALI_LIBRARY}
    FILE ${HEIMDALI_BINARY_DIR}/HEIMDALI_LIBRARY.cmake
)
