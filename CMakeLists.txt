#--------------------------------------------------------------------
# user config variables
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}" CACHE PATH "Linux install path")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# define the project name
set(PROJECT_NAME                COSMOS_CORE)

# define what libraries you want to compile
set(BUILD_COSMOS_CORE_SUPPORT              ON)          #ON/OFF
set(BUILD_COSMOS_CORE_DEVICES              ON)         #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_JPEG     ON)          #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_ZLIB     ON)          #ON/OFF
set(BUILD_COSMOS_PROJECT_USER              OFF)         #ON/OFF

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

Option(BUILD_GPROF "Build for Profiling" OFF)
if (BUILD_GPROF)
message("Profiling")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")
else ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")
endif ()

Option(BUILD_STATIC "Build Static Executables" OFF)
if (BUILD_STATIC)
	message("Building static executables")
	set(BUILD_SHARED_LIBS OFF)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif ()

# set the COSMOS CORE path
if (${PROJECT_NAME} MATCHES "COSMOS_CORE")
else()
set(COSMOS_CORE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../core)
endif()
set(CORE ${COSMOS_CORE_SOURCE_DIR})
set(COSMOS_CORE_THIRDPARTY ${CORE}/libraries/thirdparty)
set(COSMOS_CORE_DEVICE ${CORE}/libraries/device)
set(COSMOS_CORE_SUPPORT ${CORE}/libraries/support)
set(COSMOS_PROJECT_USER ${CMAKE_SOURCE_DIR})
SET(MY_INCLUDES "include")
SET(MY_LIBS "lib")
SET(MY_BINS "bin")

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
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install/${OS_FOLDER}")

IF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")
	message("Building support")
    add_subdirectory(${CORE}/libraries/support ${CORE}/build/${OS_FOLDER}/libraries/support)
    set(LINK_LIBRARY_COSMOS_CORE_SUPPORT CosmosSupport)
ENDIF(${BUILD_COSMOS_CORE_SUPPORT} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")
	message("Building device")
    add_subdirectory(${CORE}/libraries/device ${CORE}/build/${OS_FOLDER}/libraries/device)
    set(LINK_LIBRARY_COSMOS_DEVICE CosmosDevice)
ENDIF(${BUILD_COSMOS_CORE_DEVICES} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")
	message("Building zlib")
    add_subdirectory(${CORE}/libraries/thirdparty/zlib ${CORE}/build/${OS_FOLDER}/libraries/thirdparty/zlib)
    set(LINK_LIBRARY_COSMOS_CORE_ZLIB zlib )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_ZLIB} MATCHES "ON")

IF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")
	message("Building jpeg")
    add_subdirectory(${CORE}/libraries/thirdparty/jpeg ${CORE}/build/${OS_FOLDER}/libraries/thirdparty/jpeg)
    set(LINK_LIBRARY_COSMOS_CORE_JPEG jpeg )
ENDIF(${BUILD_COSMOS_CORE_THIRDPARTY_JPEG} MATCHES "ON")

IF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")
	message("Building user")
    add_subdirectory(${COSMOS_PROJECT_USER}/libraries)
    set(LINK_LIBRARY_COSMOS_PROJECT_USER CosmosUser)
ENDIF(${BUILD_COSMOS_PROJECT_USER} MATCHES "ON")

add_subdirectory(${PROJECT_SOURCE_DIR}/programs)
