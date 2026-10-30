message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(CosmosUTModule ${INCLUDES} ${SOURCES})
target_link_libraries(CosmosUTModule CosmosModule CosmosCommand CosmosEvent CosmosAgent gtest gtest_main)
add_test(NAME module_ut COMMAND CosmosUTModule)
