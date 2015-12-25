cmake_minimum_required(VERSION 2.8.12)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(CosmosSupport STATIC ${SOURCES} ${INCLUDES})

# support depends on math, so link here
target_link_libraries(CosmosSupport CosmosMath CosmosDeviceCpu CosmosDeviceDisk)

install(TARGETS CosmosSupport DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include)
install(FILES ${INCLUDESPP} DESTINATION include)

