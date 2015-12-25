if(${USE_COSMOS_FROM} MATCHES "INSTALL_FOLDER")

    # -----------------------------------------------
    # compile on Linux host
    if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")

        message("Compiling on Linux")

        #Check for ARM Build
        if (${BUILD_TYPE} MATCHES "arm")

            # ARM Build Detected

            # Set installation directory
            if("$ENV{COSMOS}" STREQUAL "")
                SET(COSMOS_FOLDER "/usr/local/cosmos/linux_arm")
            ELSE()
                SET(COSMOS_FOLDER "$ENV{COSMOS}/linux_arm")
            endif("$ENV{COSMOS}" STREQUAL "")

            # Set Toolchain File
            SET(CMAKE_TOOLCHAIN_FILE "${PROJECT_SOURCE_DIR}/build/toolchain_arm")

            # Add preprocessor definition to help source files determine code to use for ARMv7 specific implementation
            add_definitions(-DBUILD_TYPE_arm) #20150825JC: Added definition

        ELSE()

            # Non-ARM Build Detected

            if("$ENV{COSMOS}" STREQUAL "")
                SET(COSMOS_FOLDER "/usr/local/cosmos/linux")
            ELSE()
                SET(COSMOS_FOLDER "$ENV{COSMOS}/linux")
            endif("$ENV{COSMOS}" STREQUAL "")

        endif()

    endif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")

    # -----------------------------------------------
    # compile on Mac host
    if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")

        message("Compiling on Mac")

        # Mac OS X specific code
        if("$ENV{COSMOS}" STREQUAL "")
            SET(COSMOS_FOLDER "/Applications/cosmos/macos")
        ELSE()
            SET(COSMOS_FOLDER "$ENV{COSMOS}/macos")
        endif("$ENV{COSMOS}" STREQUAL "")
    endif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")


    # -----------------------------------------------
    # compile on Windows host
    if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")

        message("Compiling on Windows")

       # Windows 7 specific code
       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

        #   Option(${BUILD_TYPE} MATCHES "arm" "Build for ARM architecture" OFF)
        if (${BUILD_TYPE} MATCHES "arm")
            SET(COSMOS_FOLDER "c:/cosmos/windows_arm")
            SET(CMAKE_TOOLCHAIN_FILE "${PROJECT_SOURCE_DIR}/cmake/toolchain_arm_win")
        ELSE()
            if("$ENV{COSMOS}" STREQUAL "")
                SET(COSMOS_FOLDER "c:/cosmos/windows")
            ELSE()
                SET(COSMOS_FOLDER "$ENV{COSMOS}/windows")
            endif("$ENV{COSMOS}" STREQUAL "")
        endif()

        # Convert "/" to "\" if we are in Windows
        string(REPLACE "\\" "/" COSMOS_FOLDER ${COSMOS_FOLDER})

        # TODO: implement the option to compile the code using cosmos-source directly
        # SET(COSMOS_SOURCE_CORE "e:/cosmos-source/core/")
        # add_subdirectory(${COSMOS_SOURCE_CORE} cosmos/core)

    endif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")


message("COSMOS FOLDER set to " ${COSMOS_FOLDER})

endif(${USE_COSMOS_FROM} MATCHES "INSTALL_FOLDER")



