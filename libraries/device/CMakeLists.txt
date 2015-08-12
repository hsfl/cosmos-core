cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../math)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosDevice STATIC ${SOURCES})

install(TARGETS CosmosDevice DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include)

