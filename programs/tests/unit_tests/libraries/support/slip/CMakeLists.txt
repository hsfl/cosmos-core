cmake_minimum_required(VERSION 3.1)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTSlip ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTSlip CosmosSlip gtest gtest_main)
add_test(NAME slip_ut COMMAND CosmosUTSlip)