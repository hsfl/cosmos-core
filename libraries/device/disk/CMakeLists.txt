cmake_minimum_required(VERSION 3.20)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceDisk PARENT_SCOPE)

add_library(CosmosDeviceDisk STATIC ${SOURCES} ${INCLUDES})
#target_link_libraries(CosmosDeviceDisk CosmosDeviceGeneral CosmosSupport ${COSMOS_LIBS})
target_link_libraries(CosmosDeviceDisk CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceDisk DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/disk)


if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()


message("Build ${libraryName} from ${CMAKE_CURRENT_SOURCE_DIR}")
