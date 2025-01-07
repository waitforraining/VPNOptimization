project(VPNRoomScan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(Boost_DIR "D:/code/bycode/boost_1_71_0/build/lib/cmake/Boost-1.71.0")
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

set(GDAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdParty/gdal/include")
set(GDAL_LIBRARY "${CMAKE_SOURCE_DIR}/thirdParty/gdal/lib/gdal_i.lib")

find_package(GDAL 3.0)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE HEADERS
    ${CMAKE_SOURCE_DIR}/include/*.h
)
set(
    VPN_TARGET_LIBRARIES
    ${OpenCV_LIBS} 
    ${Boost_LIBRARIES} 
    ${GDAL_LIBRARIES}
)

link_directories(
    ${CMAKE_SOURCE_DIR}/dll
)

include_directories(
    ${GDAL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

add_executable( ${PROJECT_NAME} RoomScan.cpp ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${VPN_TARGET_LIBRARIES})
