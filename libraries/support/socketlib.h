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

#ifndef SOCKETLIB_H
#define SOCKETLIB_H

/*! \file socketlib.h
*	\brief Socket Support header file
*/

//! \ingroup support
//! \defgroup socketlib Socket Library
//! UDP Socket.
//!
//! Allows the creation of UDP or TCP sockets for incoming or outgoing communication.
//! Supports blocking or non-blocking; Unicast, Broadcast or Multicast; Windows,
//! MacOS, or Linux.
//!
//! The sockets support being opened for specific types of operation:
//! - LISTEN: Wait for input on a specific port, and then optionally send a reply.
//! - COMMUNICATE: Send a message to a specific port, and then optionally receive a reply.
//! - JABBER: Broadcast packets over multiple interfaces.

#include "configCosmos.h"
#ifdef COSMOS_WIN_OS
#include <io.h>
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

//! \ingroup socketlib
//! \defgroup socketlib_constants Socket library constants
//! @{

//! Agent socket using Multicast UDP
#define SOCKET_TYPE_MULTICAST 0
//! Agent socket using Broadcast UDP
#define SOCKET_TYPE_BROADCAST 2
//! Agent socket using Unicast UDP
#define SOCKET_TYPE_UDP 2
//! Agent socket using Unicast TCP
#define SOCKET_TYPE_TCP 3
//! Agent socket using Broadcast CSP
#define SOCKET_TYPE_CSP 4

//! Blocking Agent
#define SOCKET_BLOCKING true
//! Non-blocking Agent
#define SOCKET_NONBLOCKING false
//! Talk followed by optional listen (sendto address)
#define SOCKET_TALK 0
//! Listen followed by optional talk (recvfrom INADDRANY)
#define SOCKET_LISTEN 1
//! Communicate socket (sendto followed by recvfrom)
#define SOCKET_COMMUNICATE 2
//! Talk over multiple interfaces
#define SOCKET_JABBER 3

//! Default SOCKET RCVTIMEO (100 msec)
#define SOCKET_RCVTIMEO 100000

//! IP Version Byte
#define SOCKET_IP_BYTE_VERSION 0
#define SOCKET_IP_BYTE_LEN_LOW 3
#define SOCKET_IP_BYTE_LEN_HIGH 2
#define SOCKET_IP_BYTE_PROTOCOL 9
#define SOCKET_IP_BYTE_SRC_ADDR 12
#define SOCKET_IP_BYTE_DEST_ADDR 16
#define SOCKET_IP_BYTE_UDP_LEN 24
#define SOCKET_IP_BYTE_UDP_CS 26

#define SOCKET_IP_PROTOCOL_UDP 17

#define SOCKET_BUFFER_LENGTH 512  //Max length of buffer

//! @}

//! \ingroup socketlib
//! \defgroup socketlib_typedefs Socket library typedefs
//! @{

//! Socket Channel
//! Storage for socket information
struct socket_channel
{
	// Channel type
	int32_t type;
	// Channel UDP socket handle
	int32_t cudp;
	// Channel UDP INET4 address
	struct sockaddr_in caddr;
	// Channel UDP INET4 broadcast address
	struct sockaddr_in baddr;
	// Channel UDP INET6 address
	struct sockaddr_in6 caddr6;
	// Length for chosen address
	int addrlen;
	// Channel port
	uint16_t cport;
	// Channel's maximum message size
	uint16_t msgsize;
	// Channel's protocol address in string form
	char address[17];
	// Channel's broadcast address in string form
	char baddress[17];
	// Channel's interface name
	char name[COSMOS_MAX_NAME+1];
};


//! @}

//! \ingroup socketlib
//! \defgroup socketlib_functions Socket library functions
//! @{

int32_t socket_open(socket_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t direction, bool blocking, uint32_t usectimeo);
uint16_t socket_calc_udp_checksum(std::vector<uint8_t> packet);
int32_t socket_check_udp_checksum(std::vector<uint8_t> packet);
int32_t socket_set_udp_checksum(std::vector<uint8_t>& packet);
int32_t socket_blocking(socket_channel *channel, bool blocking);
int32_t socket_close(socket_channel *channel);
std::vector<socket_channel> socket_find_addresses(uint16_t ntype);

//-------------------------------------------------------------------
// Simple UDP class to send data

struct SocketOptions
{
	// set the defaults
	// Channel type
	int32_t type = SOCKET_TYPE_UDP;
	// UDP socket handle
	int32_t handle = 0;
	// UDP INET4 address
	struct sockaddr_in server;
	struct sockaddr_in s_other;
	// to use socket connect function
	bool connect = false;

	// Channel's protocol address in string form
    std::string address = "127.0.0.1";
	// port
	uint16_t port = 8888;

	// Length for chosen address
	int addrlen = sizeof(server);

	uint16_t role = SOCKET_TALK;
	bool blocking = SOCKET_BLOCKING;
	uint32_t timeout = SOCKET_RCVTIMEO;

	bool stream = false; // for SOCK_STREAM in simgen

public:
	// constructor
	SocketOptions(){}
	//SocketOptions():type(0),handle(0),address(""),port(),addrlen(0),role(0),blocking(false),timeout(0){}

    // using list initialization is not supported be gcc 4.8 and/or MSVC 2013
    // so for the moment keep these commented out
    //  SocketOptions(uint16_t p):port{p}{}
    //  SocketOptions(std::string a, uint16_t p):address{a},port{p}{}
    //  SocketOptions(std::string a, uint16_t p, uint16_t r):address{a},port{p},role{r}{}

    SocketOptions(uint16_t p);
    SocketOptions(std::string a, uint16_t p);
    SocketOptions(std::string a, uint16_t p, uint16_t r);


} ;

class Udp
{

private:
	// agent stuff
	//socket_channel socket;
	SocketOptions sok;
	//beatstruc hbeat;

	int32_t iretn = 0; // return error

	//int32_t openClient();
	int32_t openServer();
    int32_t errorStatus(std::string functionName);

public:
	// constructors
	Udp(); // : sok("127.0.0.1",8888){}
	//Udp(uint16_t p);
    //Udp(std::string a, uint16_t p);
    //Udp(std::string a, uint16_t p, uint16_t r);

	int32_t socketOpen();

	int32_t setupClient();
    int32_t setupClient(std::string a, uint16_t p);

    int32_t setupClientSimGen(std::string a, uint16_t p);
    int32_t setupClientAcstb(std::string a, uint16_t p);

	//int32_t setupServer();
	//int32_t setupServer(uint16_t p);
	int32_t setupServer(uint16_t port, float timeout_sec);


    int32_t send(std::string package2send);
	int32_t receiveLoop();
	int32_t receiveOnce();

	int32_t close();

    std::string receivedData; // container for received data

};

//! @}

#endif // SOCKETLIB_H
