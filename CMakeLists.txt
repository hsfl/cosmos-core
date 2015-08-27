#--------------------------------------------------------------------
# Windows/MSVC2013 notes:
# * Near the end of this script are some replacement lines to select the
#   MSVC static runtime. Turn USE_MSVC_STATIC_RUNTIME to OFF to
#   use the default "multithreaded DLL" style of RTL
# * For x64 linkage give the "-A x64" option to the CMAKE command
#--------------------------------------------------------------------
option(USE_MSVC_STATIC_RUNTIME "USE Static Runtime with MSVC" "OFF")

#--------------------------------------------------------------------
# user config variables
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# define the project name
SET(PROJECT_NAME                COSMOS_CORE)

# Type of build
SET(CMAKE_BUILD_TYPE Debug) # Release or Debug

# Type of architecture
add_definitions( -DBUILD_TYPE_${BUILD_TYPE})

# define folder where your programs are going to be installed
#SET(INSTALL_DIR                run)

#--------------------------------------------------------------------
# automatic config

# comment
SET(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 2.6)

project(${PROJECT_NAME})

# define what libraries you want to compile
SET(BUILD_COSMOS_CORE_MATH                 ON)          #ON/OFF
SET(BUILD_COSMOS_CORE_SUPPORT              ON)          #ON/OFF
SET(BUILD_COSMOS_CORE_DEVICES              ON)          #ON/OFF
SET(BUILD_COSMOS_CORE_THIRDPARTY_JPEG      ON)          #ON/OFF
SET(BUILD_COSMOS_CORE_THIRDPARTY_ZLIB      ON)          #ON/OFF
SET(BUILD_COSMOS_PROJECT_USER              OFF)         #ON/OFF
SET(BUILD_COSMOS_CORE_THIRDPARTY_PNG       ON)          #ON/OFF
IF (MSVC)
    SET(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     ON)          #ON/OFF
ELSE()
    SET(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     OFF)
ENDIF()

#TODO: is COSMOS_CXX_FLAGS still being used?
#SET(COSMOS_CXX_FLAGS "-Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")
SET(COSMOS_CXX_FLAGS "")

# Force strict ANSI
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__")

# Debugging?
IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    IF (MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MTd")
    ELSE()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    ENDIF()
    MESSAGE("Building for Debugging")
ENDIF()

Option(BUILD_GPROF "Build for Profiling" OFF)
IF (BUILD_GPROF)
    MESSAGE("Profiling")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ${COSMOS_CXX_FLAGS}")
ELSE ()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COSMOS_CXX_FLAGS}")
ENDIF ()

Option(BUILD_STATIC "Build Static Executables" OFF)
IF (BUILD_STATIC AND NOT MSVC)
    MESSAGE("Building static executables")
    SET(BUILD_SHARED_LIBS OFF)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
ENDIF ()



# SET the COSMOS CORE path
IF (${PROJECT_NAME} MATCHES "COSMOS_CORE")

ELSE()
    SET(COSMOS_CORE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../core)
ENDIF()

# -----------------------------------------------
# Linux specific
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # or just UNIX

    # Configure compiler arguments (GCC)
    IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Set Compiler flags
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        MESSAGE("Building with GCC (MinGw)")

        SET(COMPILER_IS_LINUX_GCC 1)

        # set the COSMOS_LIBS for GCC
        #TODO: remove redundancy libs, this can be resolved when building the libs
        #SET(COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosMath CosmosSupport CosmosDevice CosmosMath zlib)
        SET(COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosSupport CosmosMath zlib)

    ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    #  Option(BUILD_ARM "Build for ARM architecture" OFF)
    IF (${BUILD_TYPE} MATCHES "arm")
        IF("$ENV{COSMOS}" STREQUAL "")
            SET(COSMOS_FOLDER "/usr/local/cosmos/linux_arm")
        ELSE()
            SET(COSMOS_FOLDER "$ENV{COSMOS}/linux_arm")
        ENDIF("$ENV{COSMOS}" STREQUAL "")

        SET(CMAKE_TOOLCHAIN_FILE "${PROJECT_SOURCE_DIR}/build/toolchain_arm")

    ELSE()
        IF("$ENV{COSMOS}" STREQUAL "")
            SET(COSMOS_FOLDER "/usr/local/cosmos/linux")
        ELSE()
            SET(COSMOS_FOLDER "$ENV{COSMOS}/linux")
        ENDIF("$ENV{COSMOS}" STREQUAL "")
    ENDIF()

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


# -----------------------------------------------
# Windows specific
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows") #WIN32

    # Configure compiler arguments GCC (MinGw)
    IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Set Compiler flags
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
        MESSAGE("Building with G++ (MinGw)")
        set (COMPILER_IS_WINDOWS_MINGW 1)

        # set the COSMOS_LIBS for MinGW
        #TODO: remove redundancy libs, this can be resolved when building the libs
        #set (COSMOS_LIBS ${COSMOS_LIBS} CosmosMath CosmosDevice CosmosMath CosmosSupport CosmosMath CosmosDevice CosmosMath zlib wsock32 winmm ws2_32 iphlpapi)
        set (COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosSupport CosmosMath zlib wsock32 winmm ws2_32 iphlpapi)

    ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") #or just MSVC
        MESSAGE("Building with MSVC")

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")

        SET(COMPILER_IS_WINDOWS_MSVC 1)

        # set the COSMOS_LIBS for MSVC
        #TODO: remove redundancy libs, this can be resolved when building the libs
        #SET(COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosMath CosmosSupport CosmosDevice zlib dirent wsock32 winmm ws2_32 iphlpapi)
        SET(COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosSupport CosmosMath zlib dirent wsock32 winmm ws2_32 iphlpapi)

    ENDIF()

    IF (${BUILD_TYPE} MATCHES "arm")
        SET(COSMOS_FOLDER "c:/cosmos/windows_arm")
        SET(CMAKE_TOOLCHAIN_FILE "${PROJECT_SOURCE_DIR}/cmake/toolchain_arm_win")
    ELSE()
        IF("$ENV{COSMOS}" STREQUAL "")
            SET(COSMOS_FOLDER "c:/cosmos/windows")
        ELSE()
            SET(COSMOS_FOLDER "$ENV{COSMOS}/windows")
        ENDIF("$ENV{COSMOS}" STREQUAL "")
    ENDIF()

    IF(MSVC AND ((${USE_MSVC_STATIC_RUNTIME} MATCHES "ON")))
        MESSAGE("Building MSVC Static")
        # Below foreach is to remove the /MD from the linker settings to use a static runtime link
        # referenced from https://www.mail-archive.com/cmake@cmake.org/msg02225.html
        FOREACH (MODE "_DEBUG" "_MINSIZEREL" "_RELEASE" "_RELWITHDEBINFO")
            STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_C_FLAGS${MODE}}")
            SET(CMAKE_C_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
            #MESSAGE(STATUS "C_${MODE}=${CMAKE_C_FLAGS${MODE}}")
            STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_CXX_FLAGS${MODE}}")
            SET(CMAKE_CXX_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
            #MESSAGE(STATUS "CXX_${MODE}=${CMAKE_CXX_FLAGS${MODE}}")
        ENDFOREACH (MODE)

        MESSAGE("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")

    ELSEIF(MSVC)
        MESSAGE("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")

    ENDIF()

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# -----------------------------------------------
# MacOS specific
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    # Configure compiler arguments (GCC)
    IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Set Compiler flags
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        MESSAGE("Building with GCC")

        SET(COMPILER_IS_MACOS_GCC 1)

        # set the COSMOS_LIBS for GCC
        SET(COSMOS_LIBS ${COSMOS_LIBS} CosmosDevice CosmosMath CosmosSupport CosmosDevice CosmosMath zlib)

    ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    IF("$ENV{COSMOS}" STREQUAL "")
        SET(COSMOS_FOLDER "/Applications/cosmos")
    ELSE()
        SET(COSMOS_FOLDER "$ENV{COSMOS}/macos")
    ENDIF("$ENV{COSMOS}" STREQUAL "")

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


# -----------------------------------------------
MESSAGE(COSMOS_FOLDER: ${COSMOS_FOLDER})

IF (${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
    SET(CMAKE_INSTALL_PREFIX "${COSMOS_FOLDER}" CACHE PATH "default install path" FORCE )
ENDIF()

get_filename_component(OS_FOLDER ${CMAKE_BINARY_DIR} NAME)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)

# -----------------------------------------------
# Build Math Library
IF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")
    MESSAGE("Building Math Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/math ${CMAKE_BINARY_DIR}/libraries/math)
    SET(LINK_LIBRARY_COSMOS_CORE_MATH CosmosMath)
ENDIF(${BUILD_COSMOS_CORE_MATH} MATCHES "ON")

# -----------------------------------------------
# Build Support Library
IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
    MESSAGE("Building Support Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/support ${CMAKE_BINARY_DIR}/libraries/support)
    SET(LINK_LIBRARY_COSMOS_CORE_SUPPORT CosmosSupport)
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

# -----------------------------------------------
# Build Devices Library
IF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")
    MESSAGE("Building Device Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/device ${CMAKE_BINARY_DIR}/libraries/device)
    SET(LINK_LIBRARY_COSMOS_DEVICE CosmosDevice)
ENDIF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-zlib Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")
    MESSAGE("Building zlib Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/thirdparty/zlib ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib)
    SET(LINK_LIBRARY_COSMOS_CORE_ZLIB zlib )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")

# -----------------------------------------------
# Build Thirdparty-jpeg Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")
    MESSAGE("Building jpeg Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/thirdparty/jpeg ${CMAKE_BINARY_DIR}/libraries/thirdparty/jpeg)
    SET(LINK_LIBRARY_COSMOS_CORE_JPEG jpeg )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")

# -----------------------------------------------
# Build user Library
IF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")
    MESSAGE("Building user Library")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
    SET(LINK_LIBRARY_COSMOS_PROJECT_USER CosmosUser)
ENDIF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")

# -----------------------------------------------
# Build png Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")
    MESSAGE("Building png")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/thirdparty/png ${CMAKE_BINARY_DIR}/libraries/thirdparty/png)
    SET(LINK_LIBRARY_COSMOS_CORE_PNG png )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")

# -----------------------------------------------
# Build dirent Library
IF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")
    MESSAGE("Building dirent")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/thirdparty/dirent ${CMAKE_BINARY_DIR}/libraries/thirdparty/dirent)
    SET(LINK_LIBRARY_COSMOS_CORE_DIRENT dirent )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")


# -----------------------------------------------
MESSAGE("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# -----------------------------------------------
# now that the libraries have been compiled let's add the cosmos-core programs
add_subdirectory(${PROJECT_SOURCE_DIR}/programs)
