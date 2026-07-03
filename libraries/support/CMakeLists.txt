cmake_minimum_required(VERSION 3.20)

FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB INCLUDESPP ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# ── K0 ────────────────────────────────────────────────────────────────────────
# CosmosSupport: configCosmos, cosmos-errno, cosmos-errclass, enumlib
# enumlib is S0 conceptually but bundled here for now; future: split it out
add_library(CosmosSupport STATIC cosmos-defs.h configCosmos.h enumlib.h enumlib.cpp cosmos-errno.h cosmos-errclass.h cosmos-errclass.cpp)
install(TARGETS CosmosSupport DESTINATION lib)
target_link_libraries(CosmosSupport ${COSMOS_LIBS})

add_library(CosmosSlip STATIC sliplib.cpp sliplib.h)
install(TARGETS CosmosSlip DESTINATION lib)
target_link_libraries(CosmosSlip ${COSMOS_LIBS})

add_library(CosmosPrint STATIC print_utils.cpp print_utils.h)
install(TARGETS CosmosPrint DESTINATION lib)
target_link_libraries(CosmosPrint CosmosSupport ${COSMOS_LIBS})

# json11.cpp lives here at K0 so CosmosConvert can link it without a cycle
add_library(CosmosString STATIC stringlib.cpp stringlib.h json11.cpp json11.hpp jsonobject.cpp jsonobject.h jsonvalue.cpp jsonvalue.h)
install(TARGETS CosmosString DESTINATION lib)
target_link_libraries(CosmosString CosmosMath CosmosSupport ${COSMOS_LIBS})

add_library(CosmosJson STATIC jsonclass.cpp jsonclass.h)
install(TARGETS CosmosJson DESTINATION lib)
target_link_libraries(CosmosJson CosmosSupport ${COSMOS_LIBS})

add_library(CosmosTime STATIC timelib.cpp timelib.h timeutils.cpp timeutils.h elapsedtime.cpp elapsedtime.h)
install(TARGETS CosmosTime DESTINATION lib)
target_link_libraries(CosmosTime CosmosMath CosmosString CosmosSupport ${COSMOS_LIBS})

# ── K1 ────────────────────────────────────────────────────────────────────────
# datalib.h only includes configCosmos.h + zlib; impl uses CosmosTime, CosmosString
add_library(CosmosData STATIC datalib.cpp datalib.h)
install(TARGETS CosmosData DESTINATION lib)
target_link_libraries(CosmosData CosmosTime CosmosString localzlib CosmosSupport ${COSMOS_LIBS})

# logger.h includes socketlib.h, elapsedtime.h, timelib.h
add_library(CosmosLog STATIC check.h check.cpp logger.h logger.cpp)
install(TARGETS CosmosLog DESTINATION lib)
target_link_libraries(CosmosLog CosmosNetwork CosmosData CosmosTime CosmosString CosmosJson CosmosSupport ${COSMOS_LIBS})

add_library(CosmosNetwork STATIC socketlib.cpp socketlib.h)
install(TARGETS CosmosNetwork DESTINATION lib)
target_link_libraries(CosmosNetwork CosmosTime CosmosMath CosmosSupport ${COSMOS_LIBS})

# ── K2 ────────────────────────────────────────────────────────────────────────
add_library(CosmosCad STATIC objlib.cpp objlib.h)
install(TARGETS CosmosCad DESTINATION lib)
target_link_libraries(CosmosCad CosmosData CosmosMath CosmosSupport ${COSMOS_LIBS})

# ── K3 ────────────────────────────────────────────────────────────────────────
# channellib.h depends on packetcomm.h, timelib.h, elapsedtime.h
add_library(CosmosChannel STATIC channellib.cpp channellib.h)
install(TARGETS CosmosChannel DESTINATION lib)
target_link_libraries(CosmosChannel CosmosPacket CosmosTime CosmosSupport ${COSMOS_LIBS})

# transferlib.h depends on datalib.h, packetcomm.h, elapsedtime.h (all K0-K1)
# transferclass.h depends on agentclass.h → CosmosTransfer is effectively S2/S3
add_library(CosmosTransfer STATIC transferclass.h transferclass.cpp transferlib.h transferlib.cpp)
install(TARGETS CosmosTransfer DESTINATION lib)
target_link_libraries(CosmosTransfer CosmosAgent CosmosPacket CosmosData CosmosSupport ${COSMOS_LIBS})

# ── K4 ────────────────────────────────────────────────────────────────────────
# packetcomm.h depends on ax25class.h, crclib.h, sliplib.h, timelib.h
add_library(CosmosPacket STATIC packetcomm.cpp packetcomm.h ax25class.cpp ax25class.h)
install(TARGETS CosmosPacket DESTINATION lib)
target_link_libraries(CosmosPacket CosmosMath CosmosSlip CosmosTime CosmosSupport ${COSMOS_LIBS})

add_subdirectory(filesenderimpl)

# ── S0 ────────────────────────────────────────────────────────────────────────
add_library(CosmosConvert STATIC convertlib.cpp convertlib.h convertdef.h demlib.cpp demlib.h jpleph.cpp jpleph.h geomag.cpp geomag.h ephemlib.cpp ephemlib.h)
install(TARGETS CosmosConvert DESTINATION lib)
target_link_libraries(CosmosConvert CosmosData CosmosTime CosmosString CosmosMath CosmosSupport ${COSMOS_LIBS})

# jsondef is K2 but bundled here with jsonlib (S0) for now
add_library(CosmosNamespace STATIC jsonlib.cpp jsonlib.h jsondef.h jsondef.cpp)
install(TARGETS CosmosNamespace DESTINATION lib)
target_link_libraries(CosmosNamespace CosmosConvert CosmosCad CosmosJson CosmosString CosmosMath CosmosSupport ${COSMOS_LIBS})

add_library(Envi STATIC envi.cpp envi.h)
install(TARGETS Envi DESTINATION lib)
target_link_libraries(Envi CosmosData CosmosSupport ${COSMOS_LIBS})

# ── S3 ────────────────────────────────────────────────────────────────────────
# beacon.h depends on jsonlib.h, enumlib.h, packetcomm.h
add_library(CosmosBeacon STATIC beacon.cpp beacon.h)
install(TARGETS CosmosBeacon DESTINATION lib)
target_link_libraries(CosmosBeacon CosmosPacket CosmosNamespace CosmosMath CosmosSupport ${COSMOS_LIBS})

# packethandler.h depends on agentclass.h, convertlib.h, beacon.h, packetcomm.h
add_library(CosmosPacketHandler STATIC packethandler.cpp packethandler.h)
install(TARGETS CosmosPacketHandler DESTINATION lib)
target_link_libraries(CosmosPacketHandler CosmosBeacon CosmosPacket CosmosConvert CosmosAgent CosmosNamespace CosmosSupport ${COSMOS_LIBS})

install(FILES ${INCLUDES} DESTINATION include/support)
install(FILES ${INCLUDESPP} DESTINATION include/support)

if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()
