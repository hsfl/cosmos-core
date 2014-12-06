cmake_minimum_required(VERSION 2.6)
#set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++0x -Wno-invalid-offsetof")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/jpeg)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(jpeg STATIC ${SOURCES} ${INCLUDES})

install(TARGETS jpeg DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include)

