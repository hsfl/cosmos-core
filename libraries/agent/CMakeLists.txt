cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosAgent STATIC ${SOURCES} ${INCLUDES})
#target_link_libraries(CosmosAgent CosmosNetwork CosmosString CosmosDeviceCpu CosmosTime CosmosError CosmosJson localzlib)
target_link_libraries(CosmosAgent CosmosError CosmosString CosmosTime CosmosNetwork CosmosJson CosmosCad CosmosNamespace CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosAgent DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/agent)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
