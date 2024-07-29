cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})

    get_filename_component(bfile ${file} NAME_WE)

    add_executable(${bfile} ${bfile}.cpp)

#    target_link_libraries(${bfile} CosmosSupport ${COSMOS_LIBS})

    install(TARGETS ${bfile} DESTINATION bin)

    if(${DEPLOY_BIN_FILES_TO_REMOTE} MATCHES "YES")
        add_deployment_binary(${bfile} "bin")
    endif()

    if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
        add_deployment_source(${bfile}.cpp "src")
    endif()


endforeach()

target_link_libraries(agent CosmosAgent)
target_link_libraries(agent_monitor CosmosAgent CosmosCommand)
target_link_libraries(agent_time CosmosAgent)
target_link_libraries(agent_tunnel CosmosAgent CosmosDeviceSerial)
target_link_libraries(agent_data CosmosAgent)
target_link_libraries(agent_file CosmosAgent CosmosTransfer CosmosPacketHandler CosmosModule)
target_link_libraries(agent_forward CosmosAgent)
target_link_libraries(agent_route CosmosAgent)
target_link_libraries(agent_tunnel2 CosmosAgent CosmosDeviceSerial)
target_link_libraries(agent_cpu CosmosAgent)
target_link_libraries(agent_exec CosmosEvent CosmosAgent CosmosCommand)
target_link_libraries(agent_propagator CosmosPacketHandler CosmosAgent CosmosPhysics CosmosAgent)

if(${BUILD_AGENTS_TYPE} MATCHES "ALL")
    add_subdirectory(ground-station)
endif()

if(${BUILD_AGENTS_TYPE} MATCHES "ALL")
    add_subdirectory(other)
endif()

# Read the contents of the default propagator setup files and generate a C++ header file
file(READ "${COSMOS_CORE_SOURCE}/data/demo/sats.dat" SAT_FILE)
file(READ "${COSMOS_CORE_SOURCE}/data/demo/targets.dat" TARGET_FILE)
file(READ "${COSMOS_CORE_SOURCE}/data/demo/tle.dat" TLE_FILE)

# Generate header/source file with the file contents as a string literal
# To be used by agent_propagator to write the default files to the realms folder if it doesn't already exist
set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/data/demo/propagate_realm_defaults.h")
file(WRITE ${OUTPUT_FILE} "#pragma once\n\nconst char* sat_default = R\"(${SAT_FILE})\";\nconst char* target_default = R\"(${TARGET_FILE})\";\nconst char* tle_default = R\"(${TLE_FILE})\";\n")
target_include_directories(agent_propagator PRIVATE "${CMAKE_BINARY_DIR}/data/demo")
