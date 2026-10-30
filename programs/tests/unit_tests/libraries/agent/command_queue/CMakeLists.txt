cmake_minimum_required(VERSION 3.20)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTCommandQueue ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTCommandQueue CosmosCommand CosmosEvent CosmosAgent gtest gtest_main)
add_test(NAME command_queue_ut COMMAND CosmosUTCommandQueue)
