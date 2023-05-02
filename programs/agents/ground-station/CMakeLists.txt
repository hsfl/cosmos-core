cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#add_executable(add_radio add_radio.cpp)
#install(TARGETS add_radio DESTINATION bin)

#add_executable(agent_antenna agent_antenna.cpp)
#install(TARGETS agent_antenna DESTINATION bin)

#add_executable(agent_control agent_control.cpp)
#install(TARGETS agent_control DESTINATION bin)

#add_executable(agent_kpc9612p agent_kpc9612p.cpp)
#install(TARGETS agent_kpc9612p DESTINATION bin)

#add_executable(agent_radio agent_radio.cpp)
#install(TARGETS agent_radio DESTINATION bin)

#add_executable(ax25_recv ax25_recv.cpp)
#install(TARGETS ax25_recv DESTINATION bin)

#add_executable(ic9100 ic9100.cpp)
#install(TARGETS ic9100 DESTINATION bin)

#add_executable(kiss_recv kiss_recv.cpp)
#install(TARGETS kiss_recv DESTINATION bin)

#add_executable(kiss_send kiss_send.cpp)
#install(TARGETS kiss_send DESTINATION bin)

#add_executable(kpc9612p_recv kpc9612p_recv.cpp)
#install(TARGETS kpc9612p_recv DESTINATION bin)

#add_executable(kpc9612p_send kpc9612p_send.cpp)
#install(TARGETS kpc9612p_send DESTINATION bin)

#add_executable(monitor_gs monitor_gs.cpp)
#install(TARGETS monitor_gs DESTINATION bin)

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

target_link_libraries(agent_antenna CosmosAgent CosmosDeviceGeneral)
target_link_libraries(agent_control CosmosAgent CosmosPhysics)
target_link_libraries(agent_kpc9612p CosmosAgent CosmosDeviceGeneral)
target_link_libraries(ax25_recv CosmosCad CosmosAgent CosmosDeviceSerial CosmosDeviceGeneral)
target_link_libraries(kiss_recv CosmosString CosmosTime CosmosDeviceGeneral CosmosLog CosmosSlip)
target_link_libraries(kiss_send CosmosDeviceGeneral CosmosSlip)
target_link_libraries(kpc9612p_recv CosmosTime CosmosDeviceGeneral)
target_link_libraries(kpc9612p_send CosmosDeviceGeneral)
target_link_libraries(monitor_gs CosmosAgent CosmosNetwork)
target_link_libraries(monitor_antenna CosmosAgent CosmosNetwork)
