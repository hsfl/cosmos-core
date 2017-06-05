#cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(jpeg STATIC ${SOURCES} ${INCLUDES})

install(TARGETS jpeg DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/thirdparty/jpeg)


if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
