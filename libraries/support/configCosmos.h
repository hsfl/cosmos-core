// MN change to cosmos.h ??
#ifndef CONFIGCOSMOS_H
#define CONFIGCOSMOS_H

//! \file configCosmos.h
//! \brief Headers and definitions common to all COSMOS

//! \ingroup defs
//! \defgroup defs_macros Special COSMOS macros
#define _USE_MATH_DEFINES
#include <climits>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cinttypes>
#include <cerrno>
#include <cstring>
#include <string>
#include <condition_variable>
#include <mutex>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cosmos-errno.h"
#include "cosmos-defs.h"

//! @{
#define COSMOS_SIZEOF(element) ((ptrdiff_t)(((element*)0)+1))
//! @}


// To check the OS Pre-defined Compiler Macros go to
// http://sourceforge.net/p/predef/wiki/OperatingSystems/

// linux definition can be UNIX or __unix__ or LINUX or __linux__.
// For GCC on Linux: __GNUC__
#ifdef __linux__
//! \addtogroup defs_macros
//! @{
#define COSMOS_LINUX_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) usleep((uint32_t)(sec*1e6)) // this allows decimal seconds
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp) mkdir((char *)dtemp,00777)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#endif

// for c++x0 WIN32 is not defined, use _WIN32 (with underscore)
// For MingW on Windows: #ifdef __MINGW32__
// Windows (x64 and x86)
#ifdef _WIN32 // Defined for both 32-bit and 64-bit environments 1
//! \addtogroup defs_macros
//! @{
#define COSMOS_WIN_OS
#define COSMOS_USLEEP(usec) Sleep((uint32_t)usec/1000)
#define COSMOS_SLEEP(sec) Sleep((uint32_t)(sec*1000))
#define CLOSE_SOCKET(socket) closesocket(socket)
#define COSMOS_MKDIR(dtemp) mkdir((char *)dtemp)
//! @}
//#define NTDDI_VERSION 0x06010000
//#define _WIN32_WINNT 0x0601
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mmsystem.h>
#include <pthread.h>
#include <io.h>
#endif

// For Mac OS: #ifdef __APPLE__
#ifdef __MACH__
//! \addtogroup defs_macros
//! @{
#define COSMOS_MAC_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) sleep((uint32_t)sec)
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp) mkdir((char *)dtemp,00777)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>

#include <sys/param.h>
#include <sys/mount.h>
#endif

#ifdef __CYGWIN__
//! \addtogroup defs_macros
//! @{
#define COSMOS_CYGWIN_OS
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
#define COSMOS_SLEEP(sec) sleep((uint32_t)sec)
#define CLOSE_SOCKET(socket) close(socket)
#define COSMOS_MKDIR(dtemp) mkdir((char *)dtemp,00777)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>

#include <sys/vfs.h>
#endif // COSMOS_CYGWIN_OS

#endif // CONFIGCOSMOS_H
