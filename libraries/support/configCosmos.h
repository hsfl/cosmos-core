// MN change to cosmos.h ??
#ifndef CONFIGCOSMOS_H
#define CONFIGCOSMOS_H

//! \file configCosmos.h
//! \brief Headers and definitions common to all COSMOS

//! \ingroup defs
//! \defgroup defs_macros Special COSMOS macros
//!

// Building under Windows
#ifdef _WIN32

//! // if we're compiling with MSVC
#ifdef _MSC_BUILD
#define COSMOS_WIN_BUILD_MSVC
#endif

#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#endif // Building under Windows

// for MSVC
#define _CRT_SECURE_NO_DEPRECATE

// --------------------- FOR ALL PLATFORMS ------------------------------
#define _USE_MATH_DEFINES
#include <climits>
#include <csignal>
#include <cstdint>
#include <cinttypes>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>
#include <condition_variable>
#include <mutex>
#include <cmath>
#include <iostream>
#ifdef _MSC_BUILD
#include <io.h> // replaces in someways unistd for windows
#else
#include <unistd.h>
#endif
#include <fcntl.h>

#include "cosmos-errno.h"
#include "cosmos-defs.h"

//! @{
#define COSMOS_SIZEOF(element) ((ptrdiff_t)(((element*)0)+1))
//! @}


// To check the OS Pre-defined Compiler Macros go to
// http://sourceforge.net/p/predef/wiki/OperatingSystems/

// --------------------- LINUX ------------------------------------
// linux definition can be UNIX or __unix__ or LINUX or __linux__.
// For GCC on Linux: __GNUC__
#ifdef __linux__
//! \addtogroup defs_macros
//! @{
#define COSMOS_LINUX_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) usleep((uint32_t)((sec>=0.?sec:0)*1e6)) // this allows decimal seconds
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp, mode) mkdir((char *)dtemp,mode)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#endif

// --------------------- WINDOWS ------------------------------------
// for c++x0 WIN32 is not defined, use _WIN32 (with underscore)
// For MingW on Windows: #ifdef __MINGW32__
// Windows (x64 and x86)
#ifdef _WIN32 // Defined for both 32-bit and 64-bit environments 1
//! \addtogroup defs_macros
//! @{
//!
#ifdef _MSC_BUILD
#include <direct.h>
#define COSMOS_MKDIR(dtemp, mode) _mkdir((char *)dtemp)
#else
#define COSMOS_MKDIR(dtemp, mode) mkdir((char *)dtemp)
#endif

#define COSMOS_WIN_OS
#define COSMOS_USLEEP(usec) Sleep((uint32_t)(usec/1000. + .5))
#define COSMOS_SLEEP(sec) Sleep((uint32_t)((sec>=0.?sec:0)*1000))
#define CLOSE_SOCKET(socket) closesocket(socket)
//! @}

#include <ws2tcpip.h>
#define HAVE_BOOLEAN
#include <iphlpapi.h>
#include <windows.h>
#include <mmsystem.h>

#ifdef __MINGW32__
#include <pthread.h>
#include <sys/time.h>
#endif

#include <thread>
#include <io.h>
#include <process.h>

#endif

// --------------------- MAC ------------------------------------
// For Mac OS: #ifdef __APPLE__
#ifdef __MACH__
//! \addtogroup defs_macros
//! @{
#define COSMOS_MAC_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) sleep((uint32_t)(sec>=0.?sec:0))
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp, mode) mkdir((char *)dtemp, mode)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif

// --------------------- CYGWIN ------------------------------------
#ifdef __CYGWIN__
//! \addtogroup defs_macros
//! @{
#define COSMOS_CYGWIN_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) sleep((uint32_t)(sec>=0.?sec:0))
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp, mode) mkdir((char *)dtemp,mode)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/vfs.h>
#endif // COSMOS_CYGWIN_OS

#endif // CONFIGCOSMOS_H
