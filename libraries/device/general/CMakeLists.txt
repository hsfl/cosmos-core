cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceGeneral PARENT_SCOPE)

add_library(CosmosDeviceGeneral STATIC ${SOURCES} ${INCLUDES})
#target_link_libraries(CosmosDeviceGeneral CosmosDeviceSerial CosmosNetwork CosmosMath CosmosSlip CosmosSupport ${COSMOS_LIBS})
target_link_libraries(CosmosDeviceGeneral CosmosData CosmosDeviceSerial CosmosMath CosmosSlip CosmosNetwork CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceGeneral DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/general)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()

message("Build ${libraryName} from ${CMAKE_CURRENT_SOURCE_DIR}")
