#ifndef SOCKETLIB_H
#define SOCKETLIB_H

/*! \file socketlib.h
*	\brief Socket Support header file
*/

//! \ingroup support
//! \defgroup socketlib Socket Library
//! UDP Socket support library.
//!
//! Allows the creation of UDP sockets for incoming or outgoing communication.
//! Supports blocking or non-blocking; Unicast, Broadcast or Multicast; Windows,
//! MacOS, or Linux.

#include "configCosmos.h"
#ifdef COSMOS_WIN_OS
//#include <iostream.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <iphlpapi.h>
//#include "mmsystem.h"
#include <io.h>
#else
//#include <sys/select.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <netdb.h>
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
//! Agent socket using Broadcast CSP
#define SOCKET_TYPE_CSP 3

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
typedef struct
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
	char name[COSMOS_MAX_NAME];
} socket_channel;


//! @}

//! \ingroup socketlib
//! \defgroup socketlib_functions Socket library functions
//! @{

int32_t socket_open(socket_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t direction, bool blocking, uint32_t usectimeo);
uint16_t socket_calc_udp_checksum(vector<uint8_t> packet);
int32_t socket_check_udp_checksum(vector<uint8_t> packet);
int32_t socket_set_udp_checksum(vector<uint8_t>& packet);
int32_t socket_blocking(socket_channel *channel, bool blocking);

//! @}

#endif // SOCKETLIB_H
