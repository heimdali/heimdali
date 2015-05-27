cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
project(heimdali)

find_package(HEIMDALI REQUIRED)

include_directories(${HEIMDALI_INCLUDE_DIRS})
include(${ITK_USE_FILE})

add_executable(main main.cxx)
target_link_libraries(main ${HEIMDALI_LIBRARIES})
