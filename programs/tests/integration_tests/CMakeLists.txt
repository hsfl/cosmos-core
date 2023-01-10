cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../../build/linux/googletest-src/googletest/include/)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")

add_executable(integration_tests integration_tests.cpp)
install(TARGETS integration_tests DESTINATION bin)

add_subdirectory(file)

target_link_libraries(integration_tests CosmosTestFile gtest gtest_main)
