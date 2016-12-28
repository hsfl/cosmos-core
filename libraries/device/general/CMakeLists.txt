cmake_minimum_required(VERSION 2.8.12)

message("Building device/general")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceGeneral PARENT_SCOPE)

add_library(CosmosDeviceGeneral STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceGeneral CosmosSupport)

install(TARGETS CosmosDeviceGeneral DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/general)

add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
