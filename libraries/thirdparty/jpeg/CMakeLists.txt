#cmake_minimum_required(VERSION 3.5)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(localjpeg STATIC ${SOURCES} ${INCLUDES})

install(TARGETS localjpeg DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/thirdparty/jpeg)


if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
