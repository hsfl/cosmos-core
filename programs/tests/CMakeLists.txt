cmake_minimum_required(VERSION 2.8.12)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    target_link_libraries(${bfile} ${COSMOS_LIBS})
    install(TARGETS ${bfile} DESTINATION bin)

#    if(${DEPLOY_FILES_TO_REMOTE} MATCHES "yes")
#        add_deployment_binary(${bfile} "bin")
#        add_deployment_source(${bfile}.cpp "src")
#    endif()


endforeach()

#include(CTest)
#enable_testing()
#add_executable(VecSubTestExecutable testmath1.cpp)
#target_link_libraries(VecSubTestExecutable ${COSMOS_LIBS})
#add_test (VecSubTest ${EXECUTABLE_OUTPUT_PATH}/VecSubTest 0 0 0 1 1 1 -1 -1 -1)

#add_executable(serialTestExecutable serialSendChar.cpp)
#target_link_libraries(serialTestExecutable ${COSMOS_LIBS})
#add_test (serialSendChar ${EXECUTABLE_OUTPUT_PATH}/serialTestExecutable)
