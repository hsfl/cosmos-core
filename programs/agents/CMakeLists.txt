cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})

    get_filename_component(bfile ${file} NAME_WE)

    add_executable(${bfile} ${bfile}.cpp)

#    target_link_libraries(${bfile} ${COSMOS_LIBS})

    install(TARGETS ${bfile} DESTINATION bin)

    if(${DEPLOY_FILES_TO_REMOTE} MATCHES "yes")
        add_deployment_binary(${bfile} "bin")
        add_deployment_source(${bfile}.cpp "src")
    endif()

endforeach()

target_link_libraries(agent CosmosAgent)
target_link_libraries(agent_monitor CosmosAgent)
target_link_libraries(agent_time CosmosAgent)
target_link_libraries(agent_tunnel CosmosAgent CosmosDeviceSerial)
target_link_libraries(agent_data CosmosAgent)
target_link_libraries(agent_file CosmosAgent CosmosTransfer CosmosSlip)
target_link_libraries(agent_file2 CosmosAgent CosmosTransfer CosmosSlip)
target_link_libraries(agent_file3 CosmosAgent CosmosTransfer CosmosSlip)
target_link_libraries(agent_file4 CosmosAgent CosmosTransfer CosmosSlip)
target_link_libraries(agent_forward CosmosAgent)
target_link_libraries(agent_route CosmosAgent)
target_link_libraries(agent_tunnel2 CosmosAgent CosmosDeviceSerial)
target_link_libraries(cosmos CosmosAgent)
target_link_libraries(agent_cpu CosmosAgent CosmosDeviceDisk CosmosPrint)
target_link_libraries(agent_exec CosmosAgent)

if(${BUILD_AGENTS_TYPE} MATCHES "ALL")
    add_subdirectory(ground-station)
endif()

if(${BUILD_AGENTS_TYPE} MATCHES "ALL")
    add_subdirectory(other)
endif()
