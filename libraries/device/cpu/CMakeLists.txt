cmake_minimum_required(VERSION 2.8.12)

message("Build device/cpu")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceCpu PARENT_SCOPE)

add_library(CosmosDeviceCpu STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceCpu CosmosDeviceGeneral)

install(TARGETS CosmosDeviceCpu DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/cpu)

add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
