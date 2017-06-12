cmake_minimum_required(VERSION 2.8.12)

message("Build device/disk")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceDisk PARENT_SCOPE)

add_library(CosmosDeviceDisk STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceDisk CosmosDeviceGeneral)

install(TARGETS CosmosDeviceDisk DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/disk)


if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()

