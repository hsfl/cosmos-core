cmake_minimum_required(VERSION 3.20)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# ── K4 ────────────────────────────────────────────────────────────────────────
# task.h depends on datalib.h, timelib.h
add_library(CosmosTask STATIC task.cpp task.h)
install(TARGETS CosmosTask DESTINATION lib)
target_link_libraries(CosmosTask CosmosData CosmosTime CosmosSupport ${COSMOS_LIBS})

# ── S3 ────────────────────────────────────────────────────────────────────────
# event.h depends on jsonlib.h, timelib.h
add_library(CosmosEvent STATIC event.cpp event.h)
install(TARGETS CosmosEvent DESTINATION lib)
target_link_libraries(CosmosEvent CosmosNamespace CosmosTime CosmosSupport ${COSMOS_LIBS})

# ── S2 ────────────────────────────────────────────────────────────────────────
# agentclass.h depends on jsonlib.h, jsonclass.h, logger.h, devicecpu.h,
#   packetcomm.h, channellib.h, beacon.h, task.h
add_library(CosmosAgent STATIC agentclass.cpp agentclass.h)
install(TARGETS CosmosAgent DESTINATION lib)
target_link_libraries(CosmosAgent CosmosLog CosmosTask CosmosChannel CosmosPacket CosmosBeacon CosmosDeviceCpu CosmosJson CosmosNamespace CosmosSupport ${COSMOS_LIBS})

# ── S3 ────────────────────────────────────────────────────────────────────────
# command_queue.h depends on agentclass.h, event.h, task.h
add_library(CosmosCommand STATIC command_queue.cpp command_queue.h)
install(TARGETS CosmosCommand DESTINATION lib)
target_link_libraries(CosmosCommand CosmosAgent CosmosEvent CosmosTask CosmosSupport ${COSMOS_LIBS})

# scheduler.h depends on agentclass.h, event.h
add_library(CosmosScheduler STATIC scheduler.cpp scheduler.h)
install(TARGETS CosmosScheduler DESTINATION lib)
target_link_libraries(CosmosScheduler CosmosAgent CosmosEvent CosmosSupport ${COSMOS_LIBS})

install(FILES ${INCLUDES} DESTINATION include/agent)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
