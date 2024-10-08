cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    target_link_libraries(${bfile} CosmosSupport ${COSMOS_LIBS})
    install(TARGETS ${bfile} DESTINATION bin)

    if(${DEPLOY_BIN_FILES_TO_REMOTE} MATCHES "YES")
        add_deployment_binary(${bfile} "bin")
    endif()

    if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
        add_deployment_source(${bfile}.cpp "src")
    endif()

endforeach()

target_link_libraries(poslvlh CosmosConvert CosmosPhysics)
target_link_libraries(geod2eci2geod CosmosTime CosmosMath CosmosConvert CosmosNamespace)
target_link_libraries(test_coordinate_transformations CosmosTime CosmosMath CosmosConvert gtest)
target_link_libraries(crctest CosmosMath)
target_link_libraries(lsfittest CosmosMath)
target_link_libraries(mathspeed CosmosDeviceCpu CosmosTime)
target_link_libraries(netspeed)
target_link_libraries(netspeedd)
target_link_libraries(objread CosmosCad)
target_link_libraries(test_device_i2c CosmosDeviceI2C)
target_link_libraries(tletest CosmosConvert)
target_link_libraries(tle2orbit CosmosConvert)
target_link_libraries(tledump CosmosConvert)
target_link_libraries(testdata)
target_link_libraries(testmath1 CosmosMath)
target_link_libraries(track_sband CosmosDeviceGeneral CosmosDeviceSerial CosmosAgent CosmosPhysics)
target_link_libraries(serial_setdtr CosmosDeviceSerial CosmosTime)
target_link_libraries(serialPutData CosmosDeviceSerial)
target_link_libraries(serialSendChar CosmosDeviceSerial)
target_link_libraries(agent_simple_request CosmosAgent CosmosLog)
target_link_libraries(errortest CosmosTime CosmosLog)
target_link_libraries(string_test CosmosString CosmosConvert)
target_link_libraries(string_float_test CosmosString)
target_link_libraries(log_move_test CosmosData)

#include(CTest)
#enable_testing()
#add_executable(VecSubTestExecutable testmath1.cpp)
#target_link_libraries(VecSubTestExecutable CosmosSupport ${COSMOS_LIBS})
#add_test (VecSubTest ${EXECUTABLE_OUTPUT_PATH}/VecSubTest 0 0 0 1 1 1 -1 -1 -1)

#add_executable(serialTestExecutable serialSendChar.cpp)
#target_link_libraries(serialTestExecutable CosmosSupport ${COSMOS_LIBS})
#add_test (serialSendChar ${EXECUTABLE_OUTPUT_PATH}/serialTestExecutable)

if(${BUILD_PROGRAMS_TESTS} MATCHES "YES")
    add_subdirectory(unit_tests)
    add_subdirectory(integration_tests)
endif()
