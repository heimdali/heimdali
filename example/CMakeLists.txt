cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
project(heimdali)

find_package(HEIMDALI REQUIRED)

include_directories(${HEIMDALI_INCLUDE_DIRS})
include(${ITK_USE_FILE})

set(CMD_LIST
    createInputImage 
    inrimage_read
    inrimage_write
)

foreach(cmd ${CMD_LIST})
    add_executable(${cmd} ${cmd}.cxx)
    target_link_libraries(${cmd} ${HEIMDALI_LIBRARIES})
endforeach(cmd)
