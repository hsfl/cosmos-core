cmake_minimum_required(VERSION 3.1)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTChannel ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTChannel CosmosChannel gtest gtest_main)
# gtest_discover_tests(CosmosUTChannel) # Update to CMake 3.10!
add_test(NAME channel_ut COMMAND CosmosUTChannel)

