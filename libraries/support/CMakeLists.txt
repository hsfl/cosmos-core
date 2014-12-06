cmake_minimum_required(VERSION 2.6)
#set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++0x -Wno-invalid-offsetof")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosSupport STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosSupport DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include)

