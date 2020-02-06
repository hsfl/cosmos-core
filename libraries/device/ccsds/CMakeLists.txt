cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceCcsds PARENT_SCOPE)

add_library(CosmosDeviceCcsds STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceCcsds)

install(TARGETS CosmosDeviceCcsds DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/ccsds)

if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()

message("Build ${libraryName} from ${CMAKE_CURRENT_SOURCE_DIR}")
