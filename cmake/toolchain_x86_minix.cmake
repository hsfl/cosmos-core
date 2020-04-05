# the name of the target operating system
set(CMAKE_SYSTEM_NAME minix)

# which compilers to use for C and C++

SET(CMAKE_FIND_ROOT_PATH  /home1/pilger/work/minix/src/external/gpl3/gcc)
SET(CMAKE_C_COMPILER   /home1/pilger/work/minix/src/external/gpl3/gcc/usr.bin/gcc)
set(CMAKE_C_FLAGS -m32)
SET(CMAKE_CXX_COMPILER /home1/pilger/work/minix/src/external/gpl3/gcc/usr.bin/g++)
set(CMAKE_CXX_FLAGS -m32)

# here is the target environment located
#set(CMAKE_FIND_ROOT_PATH   /usr/i486-linux-gnu )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
