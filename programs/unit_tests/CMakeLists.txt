cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../../build/linux/googletest-src/googletest/include/)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
#    target_link_libraries(${bfile} ${COSMOS_LIBS} gtest gtest_main)
    install(TARGETS ${bfile} DESTINATION bin)
endforeach()

target_link_libraries(test_all SPHSim CosmosNamespace gtest gtest_main)
