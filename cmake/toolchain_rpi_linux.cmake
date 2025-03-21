# This toolchain file is intended to target a Raspberry Pi 4 running Raspbian 32-bit OS
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

SET(HOSTCC gcc)
SET(CMAKE_C_COMPILER   /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-g++)
SET(CMAKE_Fortran_COMPILER /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-gfortran) # Compiling OpenBLAS requires Fortran compiler
SET(CMAKE_FIND_ROOT_PATH  /opt/cross-pi-gcc/arm-linux-gnueabihf)
SET(TARGET ARMV7) # ARMV7 is the target architecture for Raspberry Pi 4

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
