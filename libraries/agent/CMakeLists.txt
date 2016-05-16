cmake_minimum_required(VERSION 2.8.12)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosAgent STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosAgent DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/agent)

