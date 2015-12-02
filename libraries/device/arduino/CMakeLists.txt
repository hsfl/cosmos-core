#cmake_minimum_required(VERSION 2.6)

message("Building device/arduino")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../math)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceArduino PARENT_SCOPE)

add_library(CosmosDeviceArduino STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosDeviceArduino DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device)
