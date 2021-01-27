#cmake_minimum_required(VERSION 2.8.12)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
#FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(CosmosSupport STATIC
    estimation_lib.cpp
    cosmos-defs.h estimation_lib.h
    configCosmos.h)
install(TARGETS CosmosSupport DESTINATION lib)
target_link_libraries(CosmosSupport CosmosMath)

add_library(CosmosSlip STATIC sliplib.cpp sliplib.h)
install(TARGETS CosmosSlip DESTINATION lib)

add_library(CosmosTransfer STATIC transferlib.cpp transferlib.h)
install(TARGETS CosmosTransfer DESTINATION lib)

add_library(CosmosPrint STATIC print_utils.cpp print_utils.h)
install(TARGETS CosmosPrint DESTINATION lib)

add_library(CosmosError STATIC cosmos-errno.cpp cosmos-errno.h )
install(TARGETS CosmosError DESTINATION lib)

add_library(CosmosConvert STATIC convertlib.cpp convertlib.h convertdef.h demlib.cpp demlib.h jpleph.cpp jpleph.h geomag.cpp geomag.h)
install(TARGETS CosmosConvert DESTINATION lib)
target_link_libraries(CosmosConvert CosmosTime CosmosMath CosmosData CosmosEphem)

add_library(CosmosEphem STATIC ephemlib.cpp ephemlib.h)
install(TARGETS CosmosEphem DESTINATION lib)
target_link_libraries(CosmosEphem CosmosTime CosmosMath)

add_library(CosmosNetwork STATIC socketlib.cpp socketlib.h )
install(TARGETS CosmosNetwork DESTINATION lib)
target_link_libraries(CosmosNetwork CosmosTime)

add_library(CosmosNamespace STATIC json11.cpp jsonlib.cpp jsonlib.h jsondef.h)
install(TARGETS CosmosNamespace DESTINATION lib)
target_link_libraries(CosmosNamespace CosmosConvert CosmosCad CosmosTime CosmosDeviceCpu)

add_library(CosmosString STATIC stringlib.cpp stringlib.h jsonobject.cpp jsonobject.h jsonvalue.cpp jsonvalue.h)
install(TARGETS CosmosString DESTINATION lib)
#target_link_libraries(CosmosString CosmosSupport)

add_library(CosmosJson STATIC jsonclass.cpp jsonclass.h )
install(TARGETS CosmosJson DESTINATION lib)
target_link_libraries(CosmosJson CosmosString)

add_library(CosmosCad STATIC objlib.cpp objlib.h)
install(TARGETS CosmosCad DESTINATION lib)
target_link_libraries(CosmosCad CosmosMath CosmosData)

add_library(CosmosTime STATIC timelib.cpp timelib.h timeutils.cpp timeutils.h elapsedtime.cpp elapsedtime.h )
install(TARGETS CosmosTime DESTINATION lib)
target_link_libraries(CosmosTime CosmosData CosmosEphem)

add_library(CosmosData STATIC datalib.cpp datalib.h)
install(TARGETS CosmosData DESTINATION lib)
target_link_libraries(CosmosData CosmosTime CosmosString CosmosNamespace CosmosConvert localzlib)

install(FILES ${INCLUDES} DESTINATION include/support)
install(FILES ${INCLUDESPP} DESTINATION include/support)

if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
