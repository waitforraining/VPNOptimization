project(VPNRun)

add_executable(VPNRun RoomScan.cpp)

include_directories(${PROJECT_TOP_DIR}/VPNDLL
    ${CMAKE_SOURCE_DIR}/thirdParty/gdal/dll
)

target_link_libraries(VPNRun VPNDLL)