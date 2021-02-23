cmake_minimum_required(VERSION 2.8.12)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

# For running UH's python stuff, for simulation/integration testing purposes
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})
configure_file(testscript.py ${CMAKE_CURRENT_BINARY_DIR}/testscript.py COPYONLY)
# may also need to copy the testscript.py file into DESTINATION bin?

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    target_link_libraries(${bfile} CosmosAgent ${Python3_LIBRARIES})
    install(TARGETS ${bfile} DESTINATION bin)
endforeach()
