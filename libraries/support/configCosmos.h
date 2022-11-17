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

//! \file configCosmos.h
//! \brief Headers and definitions common to all COSMOS

#include "support/configCosmosKernel.h"

#include <mutex>
using std::mutex;
using std::unique_lock;
#include <thread>
using std::thread;
#include <condition_variable>
using std::condition_variable;
#include <map>
using std::map;

#include "support/cosmos-errclass.h"

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
