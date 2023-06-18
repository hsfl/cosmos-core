cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosTask STATIC task.cpp task.h )
install(TARGETS CosmosTask DESTINATION lib)
target_link_libraries(CosmosTask CosmosTime CosmosData CosmosSupport ${COSMOS_LIBS})

add_library(CosmosEvent STATIC event.cpp event.h )
install(TARGETS CosmosEvent DESTINATION lib)
target_link_libraries(CosmosEvent CosmosTime CosmosNamespace CosmosSupport ${COSMOS_LIBS})

add_library(CosmosAgent STATIC agentclass.cpp agentclass.h)
install(TARGETS CosmosAgent DESTINATION lib)
target_link_libraries(CosmosAgent CosmosLog CosmosTask CosmosChannel CosmosPacket CosmosBeacon CosmosDeviceCpu CosmosLog CosmosString CosmosTime CosmosData CosmosNetwork CosmosJson CosmosCad CosmosNamespace CosmosSupport ${COSMOS_LIBS})

add_library(CosmosCommand STATIC command_queue.cpp command_queue.h )
install(TARGETS CosmosCommand DESTINATION lib)
target_link_libraries(CosmosCommand CosmosString CosmosEvent CosmosAgent CosmosNamespace CosmosSupport ${COSMOS_LIBS})

add_library(CosmosScheduler STATIC scheduler.cpp scheduler.h )
install(TARGETS CosmosScheduler DESTINATION lib)
target_link_libraries(CosmosScheduler CosmosEvent CosmosAgent CosmosSupport ${COSMOS_LIBS})

install(FILES ${INCLUDES} DESTINATION include/agent)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
