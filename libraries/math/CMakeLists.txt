cmake_minimum_required(VERSION 2.8.12)
#set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++0x -Wno-invalid-offsetof")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(CosmosMath STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosMath DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/math)
install(FILES ${INCLUDESPP} DESTINATION include/math)

