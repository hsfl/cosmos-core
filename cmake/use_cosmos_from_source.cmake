if(${USE_COSMOS_FROM} MATCHES "SOURCE")
    if(NOT DEFINED COSMOS_SOURCE_CORE)
        if(NOT DEFINED COSMOS_SOURCE)
            message(FATAL_ERROR "Variable COSMOS_SOURCE is not defined!")
        endif()
        set(COSMOS_SOURCE_CORE ${COSMOS_SOURCE}/core)
    endif()
    message("using COSMOS from source folder " ${COSMOS_SOURCE_CORE})

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
    set(CMAKE_CXX_EXTENSIONS OFF)
    message("Setting Standard to c++11")

    # -----------------------------------------------
    # add the cosmos libraries
    include_directories(${COSMOS_SOURCE_CORE}/libraries/)
#    include_directories(${COSMOS_SOURCE_CORE}/libraries/support)

    include_directories(${COSMOS_SOURCE_CORE}/libraries/thirdparty)

    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/agent     ${CMAKE_BINARY_DIR}/libraries/agent/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/support   ${CMAKE_BINARY_DIR}/libraries/support/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/math      ${CMAKE_BINARY_DIR}/libraries/math/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/module    ${CMAKE_BINARY_DIR}/libraries/module/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/physics   ${CMAKE_BINARY_DIR}/libraries/physics/)


    # -----------------------------------------------
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/thirdparty/png     ${CMAKE_BINARY_DIR}/libraries/png/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/thirdparty/jpeg     ${CMAKE_BINARY_DIR}/libraries/jpeg/)
    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/thirdparty/zlib     ${CMAKE_BINARY_DIR}/libraries/zlib/)
#    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC") #or just MSVC
#        add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/thirdparty/dirent     ${CMAKE_BINARY_DIR}/libraries/dirent/)
#    endif()

    # -----------------------------------------------
    # import devices
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device)
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/general)

    add_subdirectory(${COSMOS_SOURCE_CORE}/libraries/device/general      ${CMAKE_BINARY_DIR}/libraries/device/general)

    # -----------------------------------------------
    # import device disk
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/disk)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/disk      ${CMAKE_BINARY_DIR}/libraries/device/disk)

    # -----------------------------------------------
    # import device cpu
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/cpu)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/cpu      ${CMAKE_BINARY_DIR}/libraries/device/cpu)

    # -----------------------------------------------
    # import device i2c
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/i2c)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/i2c      ${CMAKE_BINARY_DIR}/libraries/device/i2c)

    # -----------------------------------------------
    # import device serial
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/serial)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/serial      ${CMAKE_BINARY_DIR}/libraries/device/serial)

    # -----------------------------------------------
    # import device ccsds
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/ccsds)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/ccsds      ${CMAKE_BINARY_DIR}/libraries/device/ccsds)
    message("Added Ccsds")

    # -----------------------------------------------
    # import device netradio
    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/netradio)
    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/netradio      ${CMAKE_BINARY_DIR}/libraries/device/netradio)
    message("Added NetRadio")

#    # -----------------------------------------------
#    # import device vn100
#    include_directories(${COSMOS_SOURCE_CORE}/libraries/device/vn100)
#    add_subdirectory(   ${COSMOS_SOURCE_CORE}/libraries/device/vn100      ${CMAKE_BINARY_DIR}/libraries/device/vn100)

    #add_library(localzlib STATIC IMPORTED)
    #set_property(TARGET localzlib PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib/libzlib.a)

endif()
