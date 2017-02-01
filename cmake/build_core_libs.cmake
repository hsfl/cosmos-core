# -----------------------------------------------
# Build Agent Library
IF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")
    MESSAGE("Building Agent Library")
        add_subdirectory(${COSMOS_SOURCE}/core/libraries/agent ${CMAKE_BINARY_DIR}/libraries/agent)
        SET(LINK_LIBRARY_COSMOS_CORE_AGENT CosmosAgent)
ENDIF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")

# Build Physics Library
IF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")
    MESSAGE("Building Physics Library")
        add_subdirectory(${COSMOS_SOURCE}/core/libraries/physics ${CMAKE_BINARY_DIR}/libraries/physics)
        SET(LINK_LIBRARY_COSMOS_CORE_PHYSICS CosmosPhysics)
ENDIF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")


# Build Math Library
IF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")
    MESSAGE("Building Math Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/math ${CMAKE_BINARY_DIR}/libraries/math)
    SET(LINK_LIBRARY_COSMOS_CORE_MATH CosmosMath)
ENDIF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")

# -----------------------------------------------
# Build Support Library
IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
    MESSAGE("Building Support Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/support ${CMAKE_BINARY_DIR}/libraries/support)
    SET(LINK_LIBRARY_COSMOS_CORE_SUPPORT CosmosSupport)
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

# -----------------------------------------------
# Build Devices Library
IF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")
    MESSAGE("Building Device Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/device ${CMAKE_BINARY_DIR}/libraries/device)
    SET(LINK_LIBRARY_COSMOS_DEVICE CosmosDevice)
ENDIF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-Eigen Library
#IF(${BUILD_COSMOS_CORE_THIRDPARTY_EIGEN} MATCHES "ON")
#    MESSAGE("Building Eigen Library")
#    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/Eigen ${CMAKE_BINARY_DIR}/libraries/thirdparty/Eigen)
#    SET(LINK_LIBRARY_COSMOS_CORE_EIGEN Eigen )
#ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_EIGEN} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-zlib Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")
    MESSAGE("Building zlib Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/zlib ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib)
    SET(LINK_LIBRARY_COSMOS_CORE_ZLIB zlib )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-jpeg Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")
    MESSAGE("Building jpeg Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/jpeg ${CMAKE_BINARY_DIR}/libraries/thirdparty/jpeg)
    SET(LINK_LIBRARY_COSMOS_CORE_JPEG jpeg )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")

# -----------------------------------------------
# Build user Library
IF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")
    MESSAGE("Building user Library")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries)
    SET(LINK_LIBRARY_COSMOS_PROJECT_USER CosmosUser)
ENDIF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")

# -----------------------------------------------
# Build png Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")
    MESSAGE("Building png")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/png ${CMAKE_BINARY_DIR}/libraries/thirdparty/png)
    SET(LINK_LIBRARY_COSMOS_CORE_PNG png )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")

# -----------------------------------------------
# Build dirent Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")
    MESSAGE("Building dirent")
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/dirent ${CMAKE_BINARY_DIR}/libraries/thirdparty/dirent)
    SET(LINK_LIBRARY_COSMOS_CORE_DIRENT dirent )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")


# -----------------------------------------------
# Build other devices libraries
#add_subdirectory(${COSMOS_SOURCE}/core/libraries/device/arduino ${CMAKE_BINARY_DIR}/libraries/device/arduino)
