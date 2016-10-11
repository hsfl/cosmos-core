cmake_minimum_required(VERSION 2.8.12)

message("Building device/astrodev")

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# set the variable "libraryName" with the name of the library
# and make sure it's propagated to the parent scope
# for using it in the libraryList
set(libraryName CosmosDeviceAstrodev PARENT_SCOPE)

add_library(CosmosDeviceAstrodev STATIC ${SOURCES} ${INCLUDES})

install(TARGETS CosmosDeviceAstrodev DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/astrodev)
