SET(CMAKE_SYSTEM_NAME Linux)
SET(CROSS_TYPE iobc)

SET(CMAKE_C_COMPILER   /home1/shared/software/kubos/iobc_toolchain/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /home1/shared/software/kubos/iobc_toolchain/bin/arm-linux-g++)

SET(CMAKE_FIND_ROOT_PATH  /home1/shared/software/kubos/iobc_toolchain/arm-buildroot-linux-gnueabi)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
