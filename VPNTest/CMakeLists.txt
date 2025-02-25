project(VPNTest)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdParty/googletest ${CMAKE_BINARY_DIR}/googletest_build)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(VPNTest ioTest.cpp funcTest.cpp)
target_include_directories(
    VPNTest
    PUBLIC
    ${PROJECT_TOP_DIR}/VPNDLL
)
link_directories(${CMAKE_SOURCE_DIR}/dll)

target_link_libraries(VPNTest   
    GTest::gtest
    GTest::gtest_main
    VPNDLL)

enable_testing()

add_test(NAME VPNTest COMMAND VPNTest)