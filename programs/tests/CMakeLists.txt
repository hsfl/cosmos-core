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

target_link_libraries(crctest CosmosSupport)
target_link_libraries(lsfittest CosmosSupport)
target_link_libraries(mathspeed CosmosSupport CosmosTime)
target_link_libraries(netspeed CosmosSupport)
target_link_libraries(netspeedd CosmosSupport)
target_link_libraries(objread CosmosSupport CosmosCad)
target_link_libraries(test_device_i2c CosmosDeviceI2C)
target_link_libraries(testdata CosmosSupport)
target_link_libraries(testmath1 CosmosSupport)
target_link_libraries(track_sband CosmosSupport CosmosDeviceGeneral CosmosDeviceSerial CosmosAgent CosmosPhysics)
target_link_libraries(serial_setdtr CosmosDeviceSerial CosmosSupport)
target_link_libraries(serialPutData CosmosDeviceSerial CosmosSupport)
target_link_libraries(serialSendChar CosmosDeviceSerial CosmosSupport)
target_link_libraries(agent_simple_request CosmosSupport CosmosAgent CosmosError)

#include(CTest)
#enable_testing()
#add_executable(VecSubTestExecutable testmath1.cpp)
#target_link_libraries(VecSubTestExecutable ${COSMOS_LIBS})
#add_test (VecSubTest ${EXECUTABLE_OUTPUT_PATH}/VecSubTest 0 0 0 1 1 1 -1 -1 -1)

#add_executable(serialTestExecutable serialSendChar.cpp)
#target_link_libraries(serialTestExecutable ${COSMOS_LIBS})
#add_test (serialSendChar ${EXECUTABLE_OUTPUT_PATH}/serialTestExecutable)
