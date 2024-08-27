cmake_minimum_required(VERSION 3.1)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTLog ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTLog CosmosLog gtest gtest_main)
add_test(NAME log_ut COMMAND CosmosUTLog)