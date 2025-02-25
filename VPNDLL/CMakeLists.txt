project(VPNDLL)

find_package(OpenCV REQUIRED)

set(Boost_DIR "D:/code/bycode/boost_1_71_0/build/lib/cmake/Boost-1.71.0")
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

set(GDAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdParty/gdal/include")
set(GDAL_LIBRARY "${CMAKE_SOURCE_DIR}/thirdParty/gdal/lib/gdal_i.lib")

find_package(GDAL 3.0)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library(VPNDLL SHARED
    ${SOURCES} 
    ${HEADERS}
)
set_target_properties(VPNDLL PROPERTIES
    OUTPUT_NAME "VPNDLL"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dll  
)

target_compile_definitions(VPNDLL PRIVATE VPN_DLL_EXPORTS)  # µ¼³ö·ûºÅ
target_include_directories(VPNDLL PUBLIC 
    ${GDAL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    include)

target_link_libraries(VPNDLL 
    ${OpenCV_LIBS} 
    ${Boost_LIBRARIES} 
    ${GDAL_LIBRARIES})

