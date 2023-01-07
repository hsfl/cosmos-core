cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../../build/linux/googletest-src/googletest/include/)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")

add_executable(test_all test_all.cpp)
install(TARGETS test_all DESTINATION bin/test)

add_subdirectory(transfer)

target_link_libraries(test_all CosmosUTTransfer CosmosNamespace gtest gtest_main)