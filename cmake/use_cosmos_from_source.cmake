if(${USE_COSMOS_FROM} MATCHES "SOURCE")
    message("using COSMOS from source folder " ${COSMOS_SOURCE})

    # -----------------------------------------------
    # add the cosmos libraries
    include_directories(${COSMOS_SOURCE}/core/libraries/)
#    include_directories(${COSMOS_SOURCE}/core/libraries/support)

    include_directories(${COSMOS_SOURCE}/core/libraries/thirdparty)

    add_subdirectory(${COSMOS_SOURCE}/core/libraries/agent     ${CMAKE_BINARY_DIR}/libraries/agent/)
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/support   ${CMAKE_BINARY_DIR}/libraries/support/)
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/math      ${CMAKE_BINARY_DIR}/libraries/math/)
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/physics   ${CMAKE_BINARY_DIR}/libraries/physics/)


    # -----------------------------------------------
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/png     ${CMAKE_BINARY_DIR}/libraries/png/)
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/jpeg     ${CMAKE_BINARY_DIR}/libraries/jpeg/)
    add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/zlib     ${CMAKE_BINARY_DIR}/libraries/zlib/)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC") #or just MSVC
        add_subdirectory(${COSMOS_SOURCE}/core/libraries/thirdparty/dirent     ${CMAKE_BINARY_DIR}/libraries/dirent/)
    endif()

    # -----------------------------------------------
    # import devices
    include_directories(${COSMOS_SOURCE}/core/libraries/device)
    include_directories(${COSMOS_SOURCE}/core/libraries/device/general)

    add_subdirectory(${COSMOS_SOURCE}/core/libraries/device/general      ${CMAKE_BINARY_DIR}/libraries/device/general)

    # -----------------------------------------------
    # import device disk
    include_directories(${COSMOS_SOURCE}/core/libraries/device/disk)
    add_subdirectory(   ${COSMOS_SOURCE}/core/libraries/device/disk      ${CMAKE_BINARY_DIR}/libraries/device/disk)

    # -----------------------------------------------
    # import device cpu
    include_directories(${COSMOS_SOURCE}/core/libraries/device/cpu)
    add_subdirectory(   ${COSMOS_SOURCE}/core/libraries/device/cpu      ${CMAKE_BINARY_DIR}/libraries/device/cpu)

    # -----------------------------------------------
    # import device i2c
    include_directories(${COSMOS_SOURCE}/core/libraries/device/i2c)
    add_subdirectory(   ${COSMOS_SOURCE}/core/libraries/device/i2c      ${CMAKE_BINARY_DIR}/libraries/device/i2c)

    # -----------------------------------------------
    # import device serial
    include_directories(${COSMOS_SOURCE}/core/libraries/device/serial)
    add_subdirectory(   ${COSMOS_SOURCE}/core/libraries/device/serial      ${CMAKE_BINARY_DIR}/libraries/device/serial)

#    # -----------------------------------------------
#    # import device vn100
#    include_directories(${COSMOS_SOURCE}/core/libraries/device/vn100)
#    add_subdirectory(   ${COSMOS_SOURCE}/core/libraries/device/vn100      ${CMAKE_BINARY_DIR}/libraries/device/vn100)

    #add_library(localzlib STATIC IMPORTED)
    #set_property(TARGET localzlib PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib/libzlib.a)

endif()
