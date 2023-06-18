cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    target_link_libraries(${bfile} CosmosAgent)
    install(TARGETS ${bfile} DESTINATION bin)
    add_deployment_binary(${bfile} "bin")
endforeach()


