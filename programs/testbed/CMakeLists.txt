
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/thirdparty)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/support)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/math)
include_directories(${COSMOS_CORE_SOURCE_DIR}/libraries/device)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})

    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)

    if (UNIX)
        target_link_libraries(${bfile} CosmosDevice CosmosSupport CosmosDevice CosmosMath zlib)
    else()

        if (MSVC)
            target_link_libraries(${bfile} CosmosDevice CosmosSupport CosmosDevice CosmosMath zlib dirent wsock32 winmm ws2_32 iphlpapi)
        else()
            target_link_libraries(${bfile} CosmosDevice CosmosSupport CosmosDevice CosmosMath zlib wsock32 winmm ws2_32 iphlpapi)
        endif()

    endif()

    install(TARGETS ${bfile} DESTINATION bin)
endforeach()

