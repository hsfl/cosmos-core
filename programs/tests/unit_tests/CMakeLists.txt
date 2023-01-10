cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../../build/linux/googletest-src/googletest/include/)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")

add_executable(test_all test_all.cpp)
install(TARGETS test_all DESTINATION bin)

add_subdirectory(channel)
add_subdirectory(packetcomm)
add_subdirectory(transfer)

target_link_libraries(
    # Our test program
    test_all
    # Test libraries
    CosmosUTChannel
    CosmosUTTransfer
    # Additional libraries
    CosmosNamespace
    # Google Test library
    gtest
    gtest_main
)