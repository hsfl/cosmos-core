set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 2.6)
project(COSMOS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x -U__STRICT_ANSI__ -Wno-invalid-offsetof")

add_subdirectory(device)
add_subdirectory(support)
add_subdirectory(thirdparty/Eigen)
add_subdirectory(testbed)
add_subdirectory(agents)
add_subdirectory(otb/agents)
add_subdirectory(otb/drivers)
add_subdirectory(device/tests)
add_subdirectory(gs)
