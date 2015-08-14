
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/thirdparty)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/math)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/support)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/device)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# run for each cpp file in this directory
foreach(file ${files})

    get_filename_component(bfile ${file} NAME_WE)

    add_executable(${bfile} ${bfile}.cpp)

    target_link_libraries(${bfile} ${COSMOS_LIBS})

    install(TARGETS ${bfile} DESTINATION bin)

endforeach()
