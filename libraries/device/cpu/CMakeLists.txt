cmake_minimum_required(VERSION 3.5)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceCpu PARENT_SCOPE)

add_library(CosmosDeviceCpu STATIC ${SOURCES} ${INCLUDES})
#target_link_libraries(CosmosDeviceCpu CosmosData CosmosTime CosmosSupport ${COSMOS_LIBS})
target_link_libraries(CosmosDeviceCpu CosmosString CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceCpu DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/cpu)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()

message("Build ${libraryName} from ${CMAKE_CURRENT_SOURCE_DIR}")
