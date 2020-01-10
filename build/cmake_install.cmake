# Install script for directory: /home/spjy/cosmos/source/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/spjy/cosmos")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/spjy/cosmos/source/core/build/libraries/agent/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/physics/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/math/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/support/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/arduino/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/astrodev/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/ccsds/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/cpu/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/disk/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/general/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/i2c/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/device/serial/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/thirdparty/zlib/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/thirdparty/jpeg/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/libraries/thirdparty/png/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/programs/cmake_install.cmake")
  include("/home/spjy/cosmos/source/core/build/tutorials/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/spjy/cosmos/source/core/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
