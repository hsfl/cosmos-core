cmake_minimum_required(VERSION 3.5)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosMath STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosMath CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosMath DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/math)


if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
