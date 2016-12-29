cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(zlib STATIC ${SOURCES} ${INCLUDES})

install(TARGETS zlib DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/thirdparty/zlib)


add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
