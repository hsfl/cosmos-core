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
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# define the project name
set(PROJECT_NAME                COSMOS_CORE)

# Type of build
set(CMAKE_BUILD_TYPE Debug) # Release or Debug

# Type of architecture (arm, ...)
add_definitions( -DBUILD_TYPE_${BUILD_TYPE})

# define folder where your programs are going to be installed
#set(INSTALL_DIR                run)

# by default


#if("$ENV{COSMOS}" STREQUAL "")
#    set(COSMOS_INSTALL_FOLDER "/usr/local/cosmos/linux_arm")
#else()
#    set(COSMOS_INSTALL_FOLDER "$ENV{COSMOS}/linux_arm")
#endif("$ENV{COSMOS}" STREQUAL "")

#--------------------------------------------------------------------
# automatic config

# comment
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 2.8.12)

project(${PROJECT_NAME})

set(COSMOS_FOLDER ${PROJECT_SOURCE_DIR}/..)
set(COSMOS_CORE ${CMAKE_SOURCE_DIR})

# define what libraries you want to compile
#set(BUILD_COSMOS_CORE_PHYSICS              ON)          #ON/OFF
set(BUILD_COSMOS_CORE_MATH                 ON)          #ON/OFF
set(BUILD_COSMOS_CORE_SUPPORT              ON)          #ON/OFF
set(BUILD_COSMOS_CORE_DEVICES              ON)          #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_JPEG      ON)          #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_ZLIB      ON)          #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_EIGEN      ON)          #ON/OFF
set(BUILD_COSMOS_PROJECT_USER              OFF)         #ON/OFF
set(BUILD_COSMOS_CORE_THIRDPARTY_PNG       ON)          #ON/OFF
if (MSVC)
    set(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     ON)          #ON/OFF
else()
    set(BUILD_COSMOS_CORE_THIRDPARTY_DIRENT     OFF)
endif()

#TODO: is COSMOS_CXX_FLAGS still being used?
#set(COSMOS_CXX_FLAGS "-Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")
set(COSMOS_CXX_FLAGS "")

# Force strict ANSI
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__")

# Debugging?
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MTd")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()
    message("Building for Debugging")
endif()

option(BUILD_GPROF "Build for Profiling" OFF)
if (BUILD_GPROF)
    message("Profiling")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ${COSMOS_CXX_FLAGS}")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COSMOS_CXX_FLAGS}")
endif ()

option(BUILD_STATIC "Build Static Executables" OFF)
if (BUILD_STATIC AND NOT MSVC)
    message("Building static executables")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif ()

# -----------------------------------------------
# Linux specific
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # or just UNIX

    # Configure compiler arguments (GCC)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Set Compiler flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        message("Building with GCC (MinGw)")

        set(COMPILER_IS_LINUX_GCC 1)

        # set the COSMOS_LIBS for GCC
        #TODO: remove redundancy libs, this can be resolved when building the libs
        set(COSMOS_LIBS  )

    endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    #  option(BUILD_ARM "Build for ARM architecture" OFF)
    if (${BUILD_TYPE} MATCHES "arm")
        if("$ENV{COSMOS}" STREQUAL "")
            set(COSMOS_INSTALL_FOLDER "~/cosmos/cross/arm")
        else()
            set(COSMOS_INSTALL_FOLDER "$ENV{COSMOS}/cross/arm")
        endif("$ENV{COSMOS}" STREQUAL "")

        set(CMAKE_TOOLCHAIN_file "${PROJECT_SOURCE_DIR}/build/toolchain_arm")

    else()
        if("$ENV{COSMOS}" STREQUAL "")
            set(COSMOS_INSTALL_FOLDER "~/cosmos") #was /usr/local/cosmos/linux
        else()
            set(COSMOS_INSTALL_FOLDER "$ENV{COSMOS}")
        endif("$ENV{COSMOS}" STREQUAL "")
    endif()

endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


# -----------------------------------------------
# Windows specific
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows") #WIN32

    # Configure compiler arguments GCC (MinGw)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # Set Compiler flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -Wno-invalid-offsetof")
        message("Building with G++ (MinGw)")
        set (COMPILER_IS_WINDOWS_MINGW 1)

        # set the COSMOS_LIBS for MinGW
        #TODO: remove redundancy libs, this can be resolved when building the libs
        set(COSMOS_LIBS  wsock32 winmm ws2_32 iphlpapi)

    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC") #or just MSVC
        message("Building with MSVC")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")

        set(COMPILER_IS_WINDOWS_MSVC 1)

        # set the COSMOS_LIBS for MSVC
        #TODO: remove redundancy libs, this can be resolved when building the libs
        set(COSMOS_LIBS  dirent wsock32 winmm ws2_32 iphlpapi)

    endif()

    # default COSMOS install folder
    set(COSMOS_INSTALL_FOLDER "c:/cosmos/")

    # if we are cross compiling then put the files inside the "crosscompile" folder
    if (${BUILD_TYPE} MATCHES "arm")
        message("Building for targe arm")
        set(COSMOS_INSTALL_FOLDER "c:/cosmos/cross/arm")
        set(CMAKE_TOOLCHAIN_file "${PROJECT_SOURCE_DIR}/cmake/toolchain_arm_win")
    endif()

    # if we have set an environment variable COSMOS_INSTALL (for specific reasons, not default)
    if(NOT ("$ENV{COSMOS_INSTALL}" STREQUAL ""))
        set(COSMOS_INSTALL_FOLDER "$ENV{COSMOS_INSTALL}")
    endif()

    if(MSVC AND ((${USE_MSVC_STATIC_RUNTIME} MATCHES "ON")))
        message("Building MSVC Static")
        # Below foreach is to remove the /MD from the linker settings to use a static runtime link
        # referenced from https://www.mail-archive.com/cmake@cmake.org/msg02225.html
        foreach (MODE "_DEBUG" "_MINSIZEREL" "_RELEASE" "_RELWITHDEBINFO")
            STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_C_FLAGS${MODE}}")
            set(CMAKE_C_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
            #message(STATUS "C_${MODE}=${CMAKE_C_FLAGS${MODE}}")
            STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_CXX_FLAGS${MODE}}")
            set(CMAKE_CXX_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
            #message(STATUS "CXX_${MODE}=${CMAKE_CXX_FLAGS${MODE}}")
        endforeach (MODE)

        message("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")

    elseif(MSVC)
        message("MSVC CXX Flags: ${CMAKE_CXX_FLAGS}")

    endif()

endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# -----------------------------------------------
# MacOS specific
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    message("Building on Mac")
    message("Compiler is ${CMAKE_CXX_COMPILER_ID}")

    # Configure compiler arguments (GCC)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Set Compiler flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11 -Wno-invalid-offsetof")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        message("Building with GCC")

        set(COMPILER_IS_MACOS_GCC 1)

        # set the COSMOS_LIBS for GCC
        set(COSMOS_LIBS  )
    endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    # Configure compiler arguments (Clang)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # Set Compiler flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11 -Wno-invalid-offsetof")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
        message("Building with Clang")

        set(COMPILER_IS_MACOS_CLANG 1)

        # set the COSMOS_LIBS for GCC
        set(COSMOS_LIBS  )
    endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")


    if("$ENV{COSMOS}" STREQUAL "")
        set(COSMOS_INSTALL_FOLDER "/Applications/cosmos")
    else()
        set(COSMOS_INSTALL_FOLDER "$ENV{COSMOS}/macos")
    endif("$ENV{COSMOS}" STREQUAL "")

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


# -----------------------------------------------
# Extend COSMOS_LIBS with standard libraries
set(COSMOS_LIBS  CosmosSupport CosmosMath zlib ${COSMOS_LIBS})

message("COSMOS_INSTALL_FOLDER: ${COSMOS_INSTALL_FOLDER}")

set(CMAKE_INSTALL_PREFIX "${COSMOS_INSTALL_FOLDER}" CACHE PATH "default install path" FORCE )
message("Install: ${CMAKE_INSTALL_PREFIX}")


#get_filename_component(OS_FOLDER ${CMAKE_BINARY_DIR} NAME)

# Establish includes, both standard and device specific
# Standard inludes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/thirdparty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/math)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/support)

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
SUBDIRlist(DEVICEDIRS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/device)

# Make sure includes in all folders are available
foreach(subdir ${DEVICEDIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/device/${subdir})
endforeach()


# -----------------------------------------------
# To load the COSMOS source into another project using cmake
# we cannot use the CMAKE_SOURCE_DIR by default because it will conflict
# with the new project CMAKE_SOURCE_DIR, let's use the COSMOS_CORE variable to
# properly define where the COSMOS_CORE files are

# Establish libraries, both standard and device specific
include(${CMAKE_SOURCE_DIR}/cmake/build_core_libs.cmake)
set(COSMOS_LIBS CosmosDeviceGeneral ${COSMOS_LIBS})
foreach(subdir ${DEVICEDIRS})
    message(${subdir})
    add_subdirectory(${COSMOS_FOLDER}/core/libraries/device/${subdir} ${CMAKE_BINARY_DIR}/libraries/device/${subdir})
    list(APPEND librarylist ${libraryName})
    if(${subdir} MATCHES "general")
    else()
        set(COSMOS_LIBS ${libraryName} ${COSMOS_LIBS})
message("Setting " ${libraryName})
    endif()
endforeach()
message("core: COSMOS_LIBS: " ${COSMOS_LIBS})

# -----------------------------------------------
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# -----------------------------------------------
# now that the libraries have been compiled let's add the cosmos-core programs
add_subdirectory(${PROJECT_SOURCE_DIR}/programs)


# -----------------------------------------------
# now that the libraries have been compiled let's add the cosmos-core programs
add_subdirectory(${PROJECT_SOURCE_DIR}/tutorials)
