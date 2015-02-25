cmake_minimum_required(VERSION 2.6)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/dirent)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(dirent STATIC ${SOURCES} ${INCLUDES})

install(TARGETS dirent DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/dirent)

