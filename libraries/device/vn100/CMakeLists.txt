cmake_minimum_required(VERSION 2.8.12)

message("Build device/vn100")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceVn100 PARENT_SCOPE)

add_library(CosmosDeviceVn100 STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceVn100 CosmosDeviceGeneral)

install(TARGETS CosmosDeviceVn100 DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/vn100)

add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
