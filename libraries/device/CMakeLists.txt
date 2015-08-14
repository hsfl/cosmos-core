cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../math)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosDevice STATIC ${SOURCES})

# device depends on support and math, so link them here
target_link_libraries(CosmosDevice CosmosSupport)
#target_link_libraries(CosmosDevice CosmosMath) # not sure if I have to do this?

install(TARGETS CosmosDevice DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include)

