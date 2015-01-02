#ifndef UDPLIB_H
#define UDPLIB_H

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

#define SOCKET_BUFFER_LENGHT 512  //Max length of buffer

//! @}




//-------------------------------------------------------------------
// Simple UDP class to send data

struct SocketOptions{
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
    string address = "127.0.0.1";
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
    SocketOptions(uint16_t p):port{p}{}
    SocketOptions(string a, uint16_t p):address{a},port{p}{}
    SocketOptions(string a, uint16_t p, uint16_t r):address{a},port{p},role{r}{}

} ;

class Udp{

private:
    // agent stuff
    //socket_channel socket;
    SocketOptions sok;
    //beatstruc hbeat;

    int32_t iretn = 0; // return error

    //int32_t openClient();
    int32_t openServer();
    int32_t errorStatus(string functionName);

public:
    // constructors
    Udp(); // : sok("127.0.0.1",8888){}
    //Udp(uint16_t p);
    //Udp(string a, uint16_t p);
    //Udp(string a, uint16_t p, uint16_t r);

    int32_t socketOpen();

    int32_t setupClient();
    int32_t setupClient(string a, uint16_t p);

    int32_t setupClientSimGen(string a, uint16_t p);
    int32_t setupClientAcstb(string a, uint16_t p);

    //int32_t setupServer();
    //int32_t setupServer(uint16_t p);
    int32_t setupServer(uint16_t port, float timeout_sec);


    int32_t send(string package2send);
    int32_t receiveLoop();
    int32_t receiveOnce();

    int32_t close();

    string receivedData; // container for received data

};

#endif // UDPLIB_H
