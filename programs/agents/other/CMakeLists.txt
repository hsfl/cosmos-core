cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})

    get_filename_component(bfile ${file} NAME_WE)

    add_executable(${bfile} ${bfile}.cpp)

#    target_link_libraries(${bfile} CosmosAgent CosmosPhysics CosmosDeviceGeneral)

    install(TARGETS ${bfile} DESTINATION bin)

    if(${DEPLOY_FILES_TO_REMOTE} MATCHES "yes")
        add_deployment_binary(${bfile} "bin")
        add_deployment_source(${bfile}.cpp "src")
    endif()


endforeach()

target_link_libraries(agent_arduino CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_example CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_node CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_orig CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_physics CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_transmitter CosmosAgent CosmosPhysics CosmosDeviceGeneral)
target_link_libraries(agent_transmitter2 CosmosAgent CosmosPhysics CosmosDeviceGeneral)
