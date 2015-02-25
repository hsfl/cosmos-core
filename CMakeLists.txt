#--------------------------------------------------------------------
# Windows/MSVC2013 notes:
# * Near the end of this script is replacement lines to select the
#   MSVC static runtime. Turn USE_MSVC_STATIC_RUNTIME to OFF to
#   use the default "multithreaded DLL" style of RTL
# * For x64 linkage give the "-A x64" option to the CMAKE command
#--------------------------------------------------------------------
option(USE_MSVC_STATIC_RUNTIME "USE Static Runtime with MSVC" "ON")

#--------------------------------------------------------------------
# user config variables
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# define the project name
set(PROJECT_NAME                COSMOS_CORE)

# define what libraries you want to compile
set(BUILD_COSMOS_CORE_SUPPORT              ON)          #ON/OFF
set(BUILD_COSMOS_CORE_DEVICES              ON)         #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_JPEG     ON)          #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_ZLIB     ON)          #ON/OFF
set(BUILD_COSMOS_PROJECT_USER              OFF)         #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_PNG     ON)          #ON/OFF
if (WIN32)
    set(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     ON)          #ON/OFF
else()
    set(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     OFF)
endif()

# Type of build
set(CMAKE_BUILD_TYPE Debug) # Release or Debug

# define folder where your programs are going to be installed
#set(INSTALL_DIR                run)

#--------------------------------------------------------------------
# automatic config

# comment
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 2.6)

project(${PROJECT_NAME})

if (UNIX)
#set(COSMOS_CXX_FLAGS "-Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")
set(COSMOS_CXX_FLAGS "")
elseif (MSVC)
set(COSMOS_CXX_FLAGS "")
endif ()

# Debugging?
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MTd")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()
    message("Building for Debugging")
endif()

Option(BUILD_GPROF "Build for Profiling" OFF)
if (BUILD_GPROF)
message("Profiling")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ${COSMOS_CXX_FLAGS}")
else ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COSMOS_CXX_FLAGS}")
endif ()

Option(BUILD_STATIC "Build Static Executables" OFF)
if (BUILD_STATIC)
        message("Building static executables")
        set(BUILD_SHARED_LIBS OFF)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif ()

# Type of compiler
string(FIND ${CMAKE_CXX_COMPILER} "cl.exe" posc)
if (posc GREATER -1)
Option(BUILD_MSVC "Build With MSVC" ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")
message("Building with MSVC")
endif()

string(FIND ${CMAKE_CXX_COMPILER} "++" posc)
message("Find ++: '${posc}'")
if (posc GREATER -1)
Option(BUILD_G++ "Build With G++" ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
message("Building with G++")
endif()


# set the COSMOS CORE path
if (${PROJECT_NAME} MATCHES "COSMOS_CORE")
else()
set(COSMOS_CORE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../core)
endif()

# make the assumption that 'core' is where the CMakeLists.txt (this) file is located
set(CORE ${CMAKE_CURRENT_SOURCE_DIR})
set(COSMOS_CORE_THIRDPARTY ${CORE}/libraries/thirdparty)
set(COSMOS_CORE_DEVICE ${CORE}/libraries/device)
set(COSMOS_CORE_SUPPORT ${CORE}/libraries/support)
set(COSMOS_PROJECT_USER ${CMAKE_SOURCE_DIR})

if (MSVC)
include_directories("${COSMOS_CORE_THIRDPARTY}/softagalleria_dirent")
endif ()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    SET(OperatingSystem "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        Option(BUILD_ARM "Build for ARM architecture" OFF)
        if (BUILD_ARM)
                SET(OS_FOLDER "arm")
                set(CMAKE_TOOLCHAIN_FILE "${COSMOS_CORE_SOURCE_DIR}/cmake/toolchain_arm")
        else()
                SET(OS_FOLDER "linux")
                if (${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
                    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}" CACHE PATH "default install path" FORCE )
                endif()
        endif()
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # Mac OS X specific code
   SET(OperatingSystem "Mac OS X")
   SET(OS_FOLDER "macos")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   # Windows 7 specific code
   SET(OperatingSystem "Windows 7")
   SET(OS_FOLDER "windows")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    if (${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
        set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/mysoftware" CACHE PATH "default install path" FORCE )
    endif()
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

get_filename_component(OS_FOLDER ${CMAKE_BINARY_DIR} NAME)

IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
        message("Building support")
    add_subdirectory(${CORE}/libraries/support ${CORE}/build/${OS_FOLDER}/libraries/support)
#    add_subdirectory(${CORE}/libraries/support ${CMAKE_BINARY_DIR}/libraries/support)
    set(LINK_LIBRARY_COSMOS_CORE_SUPPORT CosmosSupport)
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")
        message("Building device")
    add_subdirectory(${CORE}/libraries/device ${CORE}/build/${OS_FOLDER}/libraries/device)
#    add_subdirectory(${CORE}/libraries/device ${CMAKE_BINARY_DIR}/libraries/device)
    set(LINK_LIBRARY_COSMOS_DEVICE CosmosDevice)
ENDIF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")
        message("Building zlib")
    add_subdirectory(${CORE}/libraries/thirdparty/zlib ${CORE}/build/${OS_FOLDER}/libraries/thirdparty/zlib)
#    add_subdirectory(${CORE}/libraries/thirdparty/zlib ${CMAKE_BINARY_DIR}/libraries/thirdparty/zlib)
    set(LINK_LIBRARY_COSMOS_CORE_ZLIB zlib )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")
        message("Building jpeg")
    add_subdirectory(${CORE}/libraries/thirdparty/jpeg ${CORE}/build/${OS_FOLDER}/libraries/thirdparty/jpeg)
#    add_subdirectory(${CORE}/libraries/thirdparty/jpeg ${CMAKE_BINARY_DIR}/libraries/thirdparty/jpeg)
    set(LINK_LIBRARY_COSMOS_CORE_JPEG jpeg )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")

IF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")
        message("Building user")
    add_subdirectory(${COSMOS_PROJECT_USER}/libraries)
    set(LINK_LIBRARY_COSMOS_PROJECT_USER CosmosUser)
ENDIF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")
        message("Building png")
    add_subdirectory(${CORE}/libraries/thirdparty/png ${CMAKE_BINARY_DIR}/libraries/thirdparty/png)
    set(LINK_LIBRARY_COSMOS_CORE_PNG png )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_PNG} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")
        message("Building dirent")
    add_subdirectory(${CORE}/libraries/thirdparty/dirent ${CMAKE_BINARY_DIR}/libraries/thirdparty/dirent)
    set(LINK_LIBRARY_COSMOS_CORE_DIRENT dirent )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_DIRENT} MATCHES "ON")

if(MSVC AND ((${USE_MSVC_STATIC_RUNTIME} MATCHES "ON")))
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
        message("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
        message("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")
endif()

message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/programs)
