cmake_minimum_required(VERSION 3.20)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTAgent ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTAgent CosmosAgent gtest gtest_main)
add_test(NAME agent_ut COMMAND CosmosUTAgent)