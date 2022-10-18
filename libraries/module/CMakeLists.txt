cmake_minimum_required(VERSION 3.5)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosModule STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosModule CosmosSupport CosmosTransfer ${COSMOS_LIBS})

install(TARGETS CosmosModule DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/module)


if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
