cmake_minimum_required(VERSION 2.8.12)

message("Building device/arduino")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceArduino PARENT_SCOPE)

add_library(CosmosDeviceArduino STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceArduino CosmosDeviceGeneral)

install(TARGETS CosmosDeviceArduino DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/arduino)
