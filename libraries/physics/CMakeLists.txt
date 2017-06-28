cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
add_library(CosmosPhysics STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosPhysics DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/physics)
install(FILES ${INCLUDESPP} DESTINATION include/physics)


if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
