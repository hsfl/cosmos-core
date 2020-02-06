MESSAGE("")
MESSAGE("Libraries:")
#set(COSMOS_LIBS  CosmosAgent CosmosPhysics CosmosSupport CosmosMath localzlib ${COSMOS_LIBS})
IF(${BUILD_COSMOS_CORE_ZLIB} MATCHES "ON")
    set(COSMOS_LIBS localzlib ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_ZLIB} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")
    set(COSMOS_LIBS CosmosMath ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
    set(COSMOS_LIBS CosmosSupport ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")
    set(COSMOS_LIBS CosmosPhysics ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")
    set(COSMOS_LIBS CosmosAgent ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")

# -----------------------------------------------
# Build Agent Library
IF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")
    MESSAGE("- Agent Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/agent ${CMAKE_BINARY_DIR}/libraries/agent)
    SET(LINK_LIBRARY_COSMOS_CORE_AGENT CosmosAgent)
#    set(COSMOS_LIBS  CosmosAgent ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_AGENT} MATCHES "ON")

# Build Physics Library
IF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")
    MESSAGE("- Physics Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/physics ${CMAKE_BINARY_DIR}/libraries/physics)
    SET(LINK_LIBRARY_COSMOS_CORE_PHYSICS CosmosPhysics)
#    set(COSMOS_LIBS  CosmosPhysics ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_PHYSICS} MATCHES "ON")


# Build Math Library
IF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")
    MESSAGE("- Math Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/math ${CMAKE_BINARY_DIR}/libraries/math)
    SET(LINK_LIBRARY_COSMOS_CORE_MATH CosmosMath)
#    set(COSMOS_LIBS CosmosMath ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")

# -----------------------------------------------
# Build Support Library
IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
    MESSAGE("- Support Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/support ${CMAKE_BINARY_DIR}/libraries/support)
    SET(LINK_LIBRARY_COSMOS_CORE_SUPPORT CosmosSupport)
#    set(COSMOS_LIBS CosmosSupport ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

# -----------------------------------------------
# Build Devices Library
IF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")
    MESSAGE("= Device Libraries")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/device ${CMAKE_BINARY_DIR}/libraries/device)
    SET(LINK_LIBRARY_COSMOS_DEVICE CosmosDevice)

    #if(${BUILD_LIBRARIES_DEVICES} MATCHES "all")
        # Device includes
        # macro to find sub-directories
        macro(SUBDIRlist result curdir)
          file(GLOB children RELATIVE ${curdir} ${curdir}/*)
          set(dirlist "")
          foreach(child ${children})
            if(IS_DIRECTORY ${curdir}/${child})
                list(APPEND dirlist ${child})
            endif()
          endforeach()
          set(${result} ${dirlist})
        endmacro()

        # go into every device folder to find cmakelists
        subdirlist(DEVICEDIRS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/device)

        set(COSMOS_LIBS CosmosDeviceGeneral ${COSMOS_LIBS})

        #cmake_policy(SET CMP0057 NEW)
        #message("libs:" "${BUILD_LIBRARIES_DEVICES}")

        foreach(subdir ${DEVICEDIRS})
            #message("${subdir}")
            #if(${subdir} IN_LIST BUILD_LIBRARIES_DEVICES OR BUILD_LIBRARIES_DEVICES MATCHES "all") # requires cmake 3.3
                #message("${subdir}")
                add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/device/${subdir} ${CMAKE_BINARY_DIR}/libraries/device/${subdir})
                list(APPEND librarylist ${libraryName})
                if(${subdir} MATCHES "general")
                else()
                    set(COSMOS_LIBS ${libraryName} ${COSMOS_LIBS})
                    message("Setting " ${libraryName})
                endif()
            #endif()
        endforeach()
    #endif()

    # Make sure includes in all folders are available
    #foreach(subdir ${DEVICEDIRS})
    #    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/device/${subdir})
    #endforeach()

ENDIF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-Eigen Library
#IF(${BUILD_COSMOS_CORE_THIRDPARTY_EIGEN} MATCHES "ON")
#    MESSAGE("- Eigen Library")
#    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/thirdparty/Eigen ${CMAKE_BINARY_DIR}/libraries/thirdparty/Eigen)
#    SET(LINK_LIBRARY_COSMOS_CORE_EIGEN Eigen )
#ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_EIGEN} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-zlib Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")
    MESSAGE("- local zlib Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/thirdparty/zlib ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib)
    SET(LINK_LIBRARY_COSMOS_CORE_ZLIB localzlib )
#    set(COSMOS_LIBS localzlib ${COSMOS_LIBS})
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-jpeg Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")
    MESSAGE("- jpeg Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/thirdparty/jpeg ${CMAKE_BINARY_DIR}/libraries/thirdparty/jpeg)
    SET(LINK_LIBRARY_COSMOS_CORE_JPEG jpeg )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")

# -----------------------------------------------
# Build user Library
IF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")
    MESSAGE("- user Library")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries)
    SET(LINK_LIBRARY_COSMOS_PROJECT_USER CosmosUser)
ENDIF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")

# -----------------------------------------------
# Build png Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")
    MESSAGE("- png")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/thirdparty/png ${CMAKE_BINARY_DIR}/libraries/thirdparty/png)
    SET(LINK_LIBRARY_COSMOS_CORE_PNG png )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")

# -----------------------------------------------
# Build dirent Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")
    MESSAGE("- dirent")
    add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/thirdparty/dirent ${CMAKE_BINARY_DIR}/libraries/thirdparty/dirent)
    SET(LINK_LIBRARY_COSMOS_CORE_DIRENT dirent )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")

# -----------------------------------------------
# Build other devices libraries
#add_subdirectory(${COSMOS_CORE_SOURCE}/libraries/device/arduino ${CMAKE_BINARY_DIR}/libraries/device/arduino)

MESSAGE("")
