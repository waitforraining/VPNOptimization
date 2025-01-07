cmake_minimum_required(VERSION 3.10)

project(VPNOptimization)


set(CMAKE_BUILD_TYPE Release Debug CACHE STRING "Choose the type of build." FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(run)
add_subdirectory(test)