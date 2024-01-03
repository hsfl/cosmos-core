cmake_minimum_required(VERSION 3.1)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# Not including the ${SOURCES} into add_library here, since otherwise the sources will build twice when making a change
add_library(CosmosUTChannel STATIC ${INCLUDES})
target_link_libraries(CosmosUTChannel CosmosChannel gtest)
target_sources(test_all PRIVATE ${SOURCES})
