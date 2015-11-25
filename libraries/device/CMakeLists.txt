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

# -----------------------------------------------
#list(APPEND libraryList "")

#add_subdirectory(${COSMOS_CORE}/libraries/device/arduino ${CMAKE_BINARY_DIR}/libraries/device/arduino)
#target_link_libraries(CosmosDevice CosmosDeviceArduino)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# go into every folder to find more cmakelists
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

message("-------------------------")
#message(${SUBDIRS})

FOREACH(subdir ${SUBDIRS})
    message(${subdir})
    add_subdirectory(${COSMOS_FOLDER}/core/libraries/device/${subdir} ${CMAKE_BINARY_DIR}/libraries/device/${subdir})
    list(APPEND libraryList ${libraryName})
ENDFOREACH()

# -----------------------------------------------
# add each device library into the main CosmosDevice lib
# basic example:
#     target_link_libraries(CosmosDevice CosmosDeviceArduino)
message("-------------------------")
message("add each device library into the main CosmosDevice lib:")

# iterate trough the libraryList
foreach(libraryName ${libraryList})
    message("adding " ${libraryName})
    target_link_libraries(CosmosDevice ${libraryName})
endforeach()
