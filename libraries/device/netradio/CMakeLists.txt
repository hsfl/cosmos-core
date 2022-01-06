cmake_minimum_required(VERSION 3.5)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(libraryName CosmosDeviceNetRadio PARENT_SCOPE)

add_library(CosmosDeviceNetRadio STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceNetRadio CosmosPacket CosmosNetwork CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceNetRadio DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/netradio)

message("Build ${libraryName} from ${CMAKE_CURRENT_SOURCE_DIR}")
