cmake_minimum_required(VERSION 3.1)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTPacketHandler ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTPacketHandler CosmosPacketHandler gtest gtest_main)
add_test(NAME packethandler_ut COMMAND CosmosUTPacketHandler)