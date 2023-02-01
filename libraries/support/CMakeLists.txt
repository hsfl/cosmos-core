cmake_minimum_required(VERSION 3.1)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(CosmosSupport STATIC estimation_lib.cpp cosmos-defs.h estimation_lib.h configCosmos.h enumlib.h enumlib.cpp cosmos-errno.h cosmos-errclass.h cosmos-errclass.cpp)
install(TARGETS CosmosSupport DESTINATION lib)
#target_link_libraries(CosmosSupport CosmosMath)
target_link_libraries(CosmosSupport ${COSMOS_LIBS})

add_library(CosmosSlip STATIC sliplib.cpp sliplib.h)
install(TARGETS CosmosSlip DESTINATION lib)

add_library(CosmosTransfer STATIC transferclass.h transferclass.cpp transferlib.h transferlib.cpp)
install(TARGETS DESTINATION lib)
target_link_libraries(CosmosTransfer CosmosPacket CosmosNamespace CosmosSupport ${COSMOS_LIBS})

add_library(CosmosPrint STATIC print_utils.cpp print_utils.h)
install(TARGETS CosmosPrint DESTINATION lib)
target_link_libraries(CosmosPrint  CosmosSupport ${COSMOS_LIBS})

#add_library(CosmosError STATIC cosmos-errno.h cosmos-errclass.h cosmos-errclass.cpp)
#install(TARGETS CosmosError DESTINATION lib)
#target_link_libraries(CosmosError CosmosString CosmosData CosmosSupport ${COSMOS_LIBS})

add_library(CosmosLog STATIC logger.h logger.cpp)
install(TARGETS CosmosLog DESTINATION lib)
target_link_libraries(CosmosLog CosmosNetwork CosmosNamespace CosmosTime CosmosString CosmosData CosmosSupport ${COSMOS_LIBS})

add_library(CosmosConvert STATIC convertlib.cpp convertlib.h convertdef.h demlib.cpp demlib.h jpleph.cpp jpleph.h geomag.cpp geomag.h ephemlib.cpp ephemlib.h)
install(TARGETS CosmosConvert DESTINATION lib)
target_link_libraries(CosmosConvert CosmosData CosmosTime CosmosMath CosmosSupport ${COSMOS_LIBS})

#add_library(CosmosEphem STATIC ephemlib.cpp ephemlib.h)
#install(TARGETS CosmosEphem DESTINATION lib)
##target_link_libraries(CosmosEphem CosmosTime CosmosMath)
#target_link_libraries(CosmosEphem CosmosConvert CosmosData CosmosSupport ${COSMOS_LIBS})

add_library(CosmosNetwork STATIC socketlib.cpp socketlib.h )
install(TARGETS CosmosNetwork DESTINATION lib)
#target_link_libraries(CosmosNetwork CosmosTime)
target_link_libraries(CosmosNetwork CosmosTime CosmosMath CosmosSupport ${COSMOS_LIBS})

add_library(CosmosNamespace STATIC json11.cpp jsonlib.cpp json11.hpp jsonlib.h jsondef.h)
install(TARGETS CosmosNamespace DESTINATION lib)
target_link_libraries(CosmosNamespace CosmosConvert CosmosCad CosmosJson CosmosDeviceCpu CosmosDeviceDisk CosmosString CosmosMath CosmosSupport ${COSMOS_LIBS})

add_library(CosmosString STATIC stringlib.cpp stringlib.h jsonobject.cpp jsonobject.h jsonvalue.cpp jsonvalue.h)
install(TARGETS CosmosString DESTINATION lib)
target_link_libraries(CosmosString CosmosConvert CosmosMath CosmosSupport ${COSMOS_LIBS})

add_library(CosmosJson STATIC jsonclass.cpp jsonclass.h )
install(TARGETS CosmosJson DESTINATION lib)
target_link_libraries(CosmosJson CosmosSupport ${COSMOS_LIBS})

add_library(CosmosCad STATIC objlib.cpp objlib.h)
install(TARGETS CosmosCad DESTINATION lib)
target_link_libraries(CosmosCad CosmosMath CosmosData CosmosSupport ${COSMOS_LIBS})

add_library(CosmosTime STATIC timelib.cpp timelib.h timeutils.cpp timeutils.h elapsedtime.cpp elapsedtime.h )
install(TARGETS CosmosTime DESTINATION lib)
target_link_libraries(CosmosTime CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(CosmosData STATIC datalib.cpp datalib.h)
install(TARGETS CosmosData DESTINATION lib)
target_link_libraries(CosmosData CosmosTime CosmosString localzlib CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(CosmosPacketHandler STATIC packethandler.cpp packethandler.h)
install(TARGETS CosmosPacketHandler DESTINATION lib)
target_link_libraries(CosmosPacketHandler CosmosBeacon CosmosPacket CosmosAgent CosmosDeviceGeneral CosmosMath CosmosNamespace CosmosSlip CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(CosmosChannel STATIC channellib.cpp channellib.h)
install(TARGETS CosmosChannel DESTINATION lib)
target_link_libraries(CosmosChannel CosmosTime CosmosPacket CosmosAgent CosmosDeviceGeneral CosmosMath CosmosNamespace CosmosSlip CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(CosmosBeacon STATIC beacon.cpp beacon.h)
install(TARGETS CosmosBeacon DESTINATION lib)
target_link_libraries(CosmosBeacon CosmosPacket CosmosAgent CosmosDeviceGeneral CosmosMath CosmosNamespace CosmosSlip CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(CosmosPacket STATIC packetcomm.cpp packetcomm.h ax25class.cpp ax25class.h)
install(TARGETS CosmosPacket DESTINATION lib)
target_link_libraries(CosmosPacket CosmosAgent CosmosMath CosmosNamespace CosmosSlip CosmosLog CosmosSupport ${COSMOS_LIBS})

add_library(Envi STATIC envi.cpp envi.h)
install(TARGETS Envi DESTINATION lib)
#target_link_libraries(Envi CosmosSupport ${COSMOS_LIBS})

install(FILES ${INCLUDES} DESTINATION include/support)
install(FILES ${INCLUDESPP} DESTINATION include/support)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
