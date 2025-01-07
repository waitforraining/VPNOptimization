
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(Boost_DIR "D:/code/bycode/boost_1_71_0/build/lib/cmake/Boost-1.71.0")
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

set(GDAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdParty/gdal/include")
set(GDAL_LIBRARY "${CMAKE_SOURCE_DIR}/thirdParty/gdal/lib/gdal_i.lib")

find_package(GDAL 3.0)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

add_subdirectory(googletest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(
    TEST_TARGET_LIBRARIES
    GTest::gtest
    GTest::gtest_main
    ${OpenCV_LIBS} 
    ${Boost_LIBRARIES} 
    ${GDAL_LIBRARIES}
)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE HEADERS
    ${CMAKE_SOURCE_DIR}/include/*.h
)

add_executable(IO_test ioTest.cpp ${SOURCES} ${HEADERS})


target_include_directories(
    IO_test
    PUBLIC
    ${GDAL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
target_sources(IO_test PRIVATE ${SOURCES})
target_link_libraries(IO_test ${TEST_TARGET_LIBRARIES})

enable_testing()
add_test(NAME IO_test COMMAND IO_test)