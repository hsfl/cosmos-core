/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

// ??? change to cosmos.h
#ifndef CONFIGCOSMOS_H
#define CONFIGCOSMOS_H

// NB: EJP 20170403 - added to suppress the ocean of format errors that will never be correct for all
// platforms. We will just have to get the formatting right on our own.
//pragma GCC diagnostic ignored "-Wformat="

//! \file configCosmos.h
//! \brief Headers and definitions common to all COSMOS

// ------------------------------------------------------------------
// Building under Windows
#ifdef _WIN32

#define COSMOS_WIN_OS

//! // determine if we're compiling with MSVC
#ifdef _MSC_BUILD
#define COSMOS_WIN_BUILD_MSVC
#endif

// TODO: Explain the reason why this is here
#define NTDDI_VERSION NTDDI_WIN7

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif
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
#include <cmath>
using std::isfinite;
//using std::isinf;
//using std::isnan;
#include <fstream>
#include <iosfwd>
using std::ifstream;
using std::ofstream;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <sstream>
using std::stringstream;
#include <regex>
#include <iomanip>
using std::fixed;
using std::setprecision;
using std::setw;
using std::left;
using std::right;
#ifdef COSMOS_WIN_BUILD_MSVC
#include <io.h> // replaces in some ways unistd for windows
#else
#include <unistd.h>
#endif
#include <fcntl.h>

#include <limits>
#include <string>
using std::string;
//using std::to_string;
#include <vector>
using std::vector;
#include <stack>
using std::stack;
#include <deque>
using std::deque;
// #include <thread>
// using std::thread;
#include <queue>
using std::queue;
#include <sstream>
using std::istringstream;
using std::pair;
#include <unordered_map>
using std::unordered_map;
#include <list>
using std::list;
#include <limits>
using std::numeric_limits;

//#include "support/cosmos-errno.h"
#include "support/cosmos-defs.h"

namespace Cosmos { namespace Support {} }

using namespace Cosmos;
using namespace Cosmos::Support;

//! \ingroup defs
//! \defgroup defs_macros Special COSMOS macros
//!
//! @{
#define COSMOS_SIZEOF(element) (reinterpret_cast<ptrdiff_t>(((element*)0)+1))
//! @}


// To check the OS Pre-defined Compiler Macros go to
// http://sourceforge.net/p/predef/wiki/OperatingSystems/

// --------------------- LINUX ------------------------------------
// linux definition can be UNIX or __unix__ or LINUX or __linux__.
// For GCC on Linux: __GNUC__
#if defined(__linux__) || defined(USE_LINUX_LIBRARIES)
//! \addtogroup defs_macros More Special COSMOS macros
//! @{
#define COSMOS_LINUX_OS
#ifndef COSMOS_MICRO_COSMOS // reduce includes for Micro-COSMOS 
#define COSMOS_USLEEP(usec) usleep(static_cast<uint32_t>(usec))
#define COSMOS_SLEEP(sec) usleep(static_cast<uint32_t>((sec>=0.?sec:0)*1e6)) // this allows decimal seconds
#define CLOSE_SOCKET(socket) ::close(socket)
#define COSMOS_MKDIR(dtemp, mode) mkdir(const_cast<char *>(dtemp), mode)
//! @}
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "json11.hpp"
#endif
#endif

// --------------------- WINDOWS ------------------------------------
// for c++x0 WIN32 is not defined, use _WIN32 (with underscore)
// For MingW on Windows: #ifdef __MINGW32__
// Windows (x64 and x86)

#ifdef COSMOS_WIN_OS // Defined for both 32-bit and 64-bit environments 1

#include <winsock2.h> // must come before <windows.h>

//! \addtogroup defs_macros More Special COSMOS macros
//! @{
#ifdef COSMOS_WIN_BUILD_MSVC
#include <direct.h>
#define COSMOS_MKDIR(dtemp, mode) _mkdir((char *)dtemp)
#else
#define COSMOS_MKDIR(dtemp, mode) mkdir((char *)dtemp)
#endif

#define COSMOS_USLEEP(usec) Sleep((uint32_t)(usec/1000. + .5))
#define COSMOS_SLEEP(sec) Sleep((uint32_t)((sec>=0.?sec:0)*1000))
#define CLOSE_SOCKET(socket) closesocket(socket)
//! @}

#include <ws2tcpip.h>
#define HAVE_BOOLEAN
#include <iphlpapi.h>
#include <windows.h>
#include <mmsystem.h>

// for MinGW
#ifdef __MINGW32__
#include <pthread.h>
#include <sys/time.h>
#endif

#include <thread>
#include <io.h>
#include <process.h>
#include "json11.hpp"

#endif

// --------------------- MAC ------------------------------------
// For Mac OS: #ifdef __APPLE__
#ifdef __MACH__
#define COSMOS_MAC_OS
//! \addtogroup defs_macros More Special COSMOS macros
//! @{
#define COSMOS_USLEEP(usec) usleep((uint32_t)usec)
//#define COSMOS_SLEEP(sec) sleep((uint32_t)(sec>=0.?sec:0))
#define COSMOS_SLEEP(sec) usleep((uint32_t)((sec>=0.?sec:0)*1e6))
#define CLOSE_SOCKET(socket) ::close(socket)
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
#include "json11.hpp"
#endif

// --------------------- CYGWIN ------------------------------------
#ifdef __CYGWIN__
#define COSMOS_CYGWIN_OS
//! \addtogroup defs_macros More Special COSMOS macros
//! @{
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
#include "json11.hpp"
#endif // COSMOS_CYGWIN_OS

#endif // CONFIGCOSMOS_H
