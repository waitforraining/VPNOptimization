cmake_minimum_required(VERSION 3.10)

project(VPNOptimization)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR}/data)

add_subdirectory(VPNDLL)
add_subdirectory(VPNRun)
add_subdirectory(VPNTest)
#add_subdirectory(test)