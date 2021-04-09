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

// TODO: add TCP class

#include "support/socketlib.h"
#include "math/mathlib.h"
#include "support/elapsedtime.h"

//! Open UDP socket
/*! Open a UDP socket and configure it for the specified use. Various
flags are set, and the socket is bound, if necessary. Support is
provided for the extra steps necessary for MS Windows.
    \param channel Pointer to ::socket_channel holding final configuration.
    \param ntype type of casting (NetworkType::BROADCAST, NetworkType::MULTICAST, NetworkType::TCP)
    \param address Destination address
    \param port Source port. If zero, automatically assigned.
    \param role Publish, subscribe, communicate.
    \param blocking True or false.
    \param usectimeo Blocking read timeout in micro seconds.
    \param rcvbuf Optional size of buffer for setsockopt SO_RCVBUF.
    \param sndbuf Optional size of buffer for setsockopt SO_SNDBUF.
    \return Zero, or negative error.
*/
int32_t socket_open(socket_channel* channel, NetworkType ntype, const char *address, uint16_t port, uint16_t role,
                    bool blocking, uint32_t usectimeo, uint32_t rcvbuf, uint32_t sndbuf)
{
    int32_t iretn;

    iretn = socket_open(*channel, ntype, address, port, role, blocking, usectimeo, rcvbuf, sndbuf);

    return iretn;
}

int32_t socket_open(socket_channel& channel, NetworkType ntype, const char *address, uint16_t port, uint16_t role,
                    bool blocking, uint32_t usectimeo, uint32_t rcvbuf, uint32_t sndbuf)
{
//    socklen_t namelen;
    int32_t iretn;
    struct ip_mreq mreq;
    int on = 1;

#ifdef COSMOS_WIN_OS
    unsigned long nonblocking = 1;
    WORD wVersionRequested;
    WSADATA wsaData;
    static bool started=false;

    if (!started)
    {
        wVersionRequested = MAKEWORD( 1, 1 );
        iretn = WSAStartup( wVersionRequested, &wsaData );
        if (iretn != 0)
        {
            return SOCKET_ERROR_OPEN;
        }
        started = true;
    }
#endif

    switch (ntype)
    {
    case NetworkType::MULTICAST:
        {
            if ((channel.cudp = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP)) <0)
            {
                return (-errno);
            }
        }
        break;
    case NetworkType::BROADCAST:
    case NetworkType::UDP:
        {
            if ((channel.cudp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) <0)
            {
                return (-errno);
            }
        }
        break;
    case NetworkType::TCP:
        {
            if ((channel.cudp = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) <0)
            {
                return (-errno);
            }
        }
        break;
    default:
        return (SOCKET_ERROR_PROTOCOL);
        break;
    }

    if (blocking == SOCKET_NONBLOCKING)
    {
        iretn = 0;
#ifdef COSMOS_WIN_OS
        if (ioctlsocket(channel.cudp, FIONBIO, &nonblocking) != 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        if (fcntl(channel.cudp, F_SETFL,O_NONBLOCK) < 0)
        {
            iretn = -errno;
        }
#endif
        if (iretn < 0)
        {
            CLOSE_SOCKET(channel.cudp);
            channel.cudp = iretn;
            return iretn;
        }
    }

    // this defines the wait time for a response from a request
    if (usectimeo)
    {
#ifdef COSMOS_WIN_OS
        int msectimeo = usectimeo/1000;
        iretn = setsockopt(channel.cudp,SOL_SOCKET,SO_RCVTIMEO,(const char *)&msectimeo,sizeof(msectimeo));
#else
        struct timeval tv;
        tv.tv_sec = usectimeo/1000000;
        tv.tv_usec = usectimeo%1000000;
        iretn = setsockopt(channel.cudp,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
#endif
    }

    memset(&channel.caddr,0,sizeof(struct sockaddr_in));
    channel.caddr.sin_family = AF_INET;
    channel.caddr.sin_port = htons(port);

    switch (role)
    {
    case SOCKET_LISTEN:
#ifdef COSMOS_MAC_OS
        if (setsockopt(channel.cudp,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on)) < 0)
#else
        if (setsockopt(channel.cudp,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0)
#endif
        {
            CLOSE_SOCKET(channel.cudp);
            channel.cudp = -errno;
            return (-errno);
        }

        switch (ntype)
        {
        case NetworkType::MULTICAST:
        case NetworkType::BROADCAST:
        case NetworkType::UDP:
            {
                channel.caddr.sin_addr.s_addr = htonl(INADDR_ANY);
                if (::bind(channel.cudp,(struct sockaddr *)&channel.caddr, sizeof(struct sockaddr_in)) < 0)
                {
                    CLOSE_SOCKET(channel.cudp);
                    channel.cudp = -errno;
                    return (-errno);
                }
            }
            break;
        case NetworkType::TCP:
            {
                channel.caddr.sin_addr.s_addr = htonl(INADDR_ANY);
                channel.caddr.sin_port = htons(port);
                if (::bind(channel.cudp,(struct sockaddr *)&channel.caddr, sizeof(struct sockaddr_in)) < 0)
                {
                    CLOSE_SOCKET(channel.cudp);
                    channel.cudp = -errno;
                    return (-errno);
                }

                if (listen(channel.cudp, 1) < 0)
                {
                    CLOSE_SOCKET(channel.cudp);
                    channel.cudp = -errno;
                    return (-errno);
                }
            }
            break;
        default:
            return (SOCKET_ERROR_PROTOCOL);
            break;
        }

        if (ntype == NetworkType::MULTICAST)
        {
            //! 2. Join multicast
            inet_pton(AF_INET,address,&mreq.imr_multiaddr.s_addr);
//            inte_pton(AF_INET, , &mreq.imr_interface.s_addr);
            //			mreq.imr_multiaddr.s_addr = inet_addr(address);
                        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            if (setsockopt(channel.cudp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
            {
                CLOSE_SOCKET(channel.cudp);
                channel.cudp = -errno;
                return (-errno);
            }
        }
        break;
    case SOCKET_JABBER:
#ifdef COSMOS_MAC_OS
        if (setsockopt(channel.cudp,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on)) < 0)
#else
        if (setsockopt(channel.cudp,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0)
#endif
        {
            CLOSE_SOCKET(channel.cudp);
            channel.cudp = -errno;
            return (-errno);
        }

        switch (ntype)
        {
        case NetworkType::BROADCAST:
        case NetworkType::UDP:
            // Set up output
            if ((iretn=setsockopt(channel.cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
            {
                CLOSE_SOCKET(channel.cudp);
                channel.cudp = -errno;
                return (-errno);
            }
            // Set up input
            channel.caddr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (::bind(channel.cudp,(struct sockaddr *)&channel.caddr, sizeof(struct sockaddr_in)) < 0)
            {
                CLOSE_SOCKET(channel.cudp);
                channel.cudp = -errno;
                return (-errno);
            }
            break;
        case NetworkType::MULTICAST:
            inet_pton(AF_INET,address,&channel.caddr.sin_addr);
            break;
        default:
            return (SOCKET_ERROR_PROTOCOL);
            break;
        }
        channel.cport = port;
        break;
    case SOCKET_TALK:
        inet_pton(AF_INET,address,&channel.caddr.sin_addr);
        channel.cport = port;

        if (ntype == NetworkType::TCP)
        {
            if (connect(channel.cudp,(struct sockaddr *)&channel.caddr, sizeof(struct sockaddr_in)) < 0 && errno != EINPROGRESS)
            {
                CLOSE_SOCKET(channel.cudp);
                channel.cudp = -errno;
                return (-errno);
            }
        }
        break;
    }

    // Find assigned port, place in cport, and set caddr to requested port
    channel.baddr = channel.caddr;
    channel.baddr.sin_addr.s_addr |= 0xff;

    switch (ntype)
    {
    case NetworkType::TCP:
        channel.cport = ntohs(channel.caddr.sin_port);
        break;
    default:
        iretn = sendto(channel.cudp, (const char *)nullptr, 0, 0, (struct sockaddr *)&channel.baddr, sizeof(struct sockaddr_in));
        sockaddr_in taddr = channel.caddr;
        socklen_t namelen = sizeof(struct sockaddr_in);
        if ((iretn = getsockname(channel.cudp, (sockaddr*)&channel.caddr, &namelen)) == -1)
        {
            CLOSE_SOCKET(channel.cudp);
            channel.cudp = -errno;
            return (-errno);
        }
        channel.cport = ntohs(channel.caddr.sin_port);
        channel.caddr = taddr;
        break;
    }

    if (rcvbuf)
    {
        setsockopt(channel.cudp, SOL_SOCKET, SO_RCVBUF, (char *)&rcvbuf, 4);
    }

    if (sndbuf)
    {
        setsockopt(channel.cudp, SOL_SOCKET, SO_SNDBUF, (char *)&sndbuf, 4);
    }

    strncpy(channel.address,address,17);
    channel.type = ntype;
    channel.addrlen = sizeof(struct sockaddr_in);

    return 0;
}

//! Establish Bus for sending out broadcasts
//! \param bus vector of ::socket_channel, one for each interface on system
//! \param port Port number to broadcast on
//! \param usectimeout Micro seconds before timeout
int32_t socket_open(socket_bus& bus, uint16_t port, uint32_t usectimeout)
{
    bus = socket_find_addresses(NetworkType::UDP, port);
    if (!bus.size())
    {
        return COSMOS_AGENT_ERROR_DISCOVERY;
    }
//    for (size_t i=0; i<bus.size(); ++i)
//    {
//        socket_channel tchan;
//        if ((socket_open(&tchan, NetworkType::UDP, ifaces[i].baddress, 3956, SOCKET_TALK, true, 100000)) < 0) return (gige_list);

//        if ((setsockopt(tchan.cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
//        {
//            close(tchan.cudp);
//            continue;
//        }
//    }
    return 0;
}

//! Accept TCP Client connection
//! If ::socket_channel is a TCP connection, accept the next client and make it available
//! for ::socket_recvfrom call.
//! \param server ::socket_channel for existing server connection.
//! \param client ::socket_channel with new client information.
//! \return Zero or negative error.
int32_t socket_accept(socket_channel server, socket_channel& client)
{
    int32_t iretn=0;
    if (server.type != NetworkType::TCP)
    {
        return SOCKET_ERROR_PROTOCOL;
    }

    iretn = accept(server.cudp, reinterpret_cast<struct sockaddr *>(&client.caddr), reinterpret_cast<socklen_t *>(&client.addrlen));
    if (iretn < 0)
    {
        return -errno;
    }

    client.cudp = iretn;

    return iretn;
}

//! Calculate UDP Checksum
/*! Calculate UDP Checksum, as detailed in RFC 768, based on the provided IP packet.
 * \param packet IP packet
 * \return Calculated checksum.
 * */
uint16_t socket_calc_udp_checksum(vector<uint8_t> packet)
{
    union
    {
        uint8_t* bytes;
        uint16_t* ints;
    } ;

    bytes = &packet[0];

    uint32_t csum32;
    csum32 = 0;
    // Pseudo header
    // Source Address
    csum32 += ints[6];
    csum32 += ints[7];
    // Destination Address
    csum32 += ints[8];
    csum32 += ints[9];
    // Protocol byte
    csum32 += bytes[SOCKET_IP_BYTE_PROTOCOL]*256U;
    // UDP Length
    csum32 += ints[12];

    // UDP header
    // UDP Source Port
    csum32 += ints[10];
    // UDP Destination Port
    csum32 += ints[11];
    // UDP Length
    csum32 += ints[12];
    // UDP Checksum
    csum32 += ints[13];

    // Actual data bytes. Do any even number, then special treatment for any odd byte.
    uint16_t udpl = (bytes[24]*256U + bytes[25]) - 8;
    if (udpl)
    {
        for (uint16_t i=0; i<udpl/2; ++i)
        {
            csum32 += ints[14+i];
        }

        if (2*(udpl/2) != udpl)
        {
            csum32 += bytes[27+udpl];
        }
    }

    // Perform end around carry
    while (csum32 > 0xffff)
    {
        csum32 = (csum32 & 0xffff) + (csum32 >> 16);
    }

    csum32 = ~csum32;

    if ((uint16_t)csum32 == 0)
    {
        return 0xffff;
    }
    else
    {
        return (uint16_t)csum32;
    }
}

//! Check UDP checksum
/*! Calculate UDP checksum for provided UDP packet and return whether it is valid or not.
 * \param packet UDP packet
 * \return Zero or negative error.
 */
int32_t socket_check_udp_checksum(vector<uint8_t> packet)
{
    uint16_t csum = socket_calc_udp_checksum(packet);

    //	if (csum == uint16from(&packet[26], ByteOrder::NETWORK))
    if (csum == 0xffff)
    {
        return 0;
    }
    else
    {
        return SOCKET_ERROR_CS;
    }
}

//! Set UDP checksum
/*! Clear UDP checksum field, calculate UDP checksum, and set UDP checksum field in provided
 * UDP packet.
 * \param packet UDP packet
 * \return Zero or negative error.
 */
int32_t socket_set_udp_checksum(vector<uint8_t>& packet)
{
    // Check if this is UDP packet
    if (packet[SOCKET_IP_BYTE_PROTOCOL] != 17)
    {
        return SOCKET_ERROR_PROTOCOL;
    }

    // Set UDP checksum bytes to zero.
    packet[SOCKET_IP_BYTE_UDP_CS] = packet[SOCKET_IP_BYTE_UDP_CS+1] = 0;

    // Calculate checksum
    uint16_t csum = socket_calc_udp_checksum(packet);

    // Place it in checksum bytes
    uint16to(csum, &packet[SOCKET_IP_BYTE_UDP_CS], ByteOrder::LITTLEENDIAN);

    return 0;
}

int32_t socket_blocking(socket_channel* channel, bool blocking)
{
    int32_t iretn;

    iretn = socket_blocking(*channel, blocking);

    return iretn;
}

int32_t socket_blocking(socket_channel& channel, bool blocking)
{
    int32_t iretn;

    if (blocking == SOCKET_NONBLOCKING)
    {
        iretn = 0;
#ifdef COSMOS_WIN_OS
        unsigned long nonblocking = 1;
        if (ioctlsocket(channel.cudp, FIONBIO, &nonblocking) != 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        if (fcntl(channel.cudp, F_SETFL,O_NONBLOCK) < 0)
        {
            iretn = -errno;
        }
#endif
    }
    else
    {
        iretn = 0;
#ifdef COSMOS_WIN_OS
        unsigned long nonblocking = 0;
        if (ioctlsocket(channel.cudp, FIONBIO, &nonblocking) != 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        int oldfl;
        if ((oldfl = fcntl(channel.cudp, F_GETFL)) == -1)
        {
            iretn = -errno;
        }
        else
        {
            if (fcntl(channel.cudp, F_SETFL, oldfl & ~O_NONBLOCK) < 0)
            {
                iretn = -errno;
            }
        }
#endif
    }
    return iretn;
}

//! Close socket
/*! Close down open socket connectiom.
    \param channel Pointer to ::socket_channel holding final configuration.
 * \return Zero or negative error.
 */
int32_t socket_close(socket_channel* channel)
{
    int32_t iretn;

    iretn = socket_close(*channel);

    return iretn;
}

int32_t socket_close(socket_channel& channel)
{
    int32_t iretn = 0;

    if (channel.cudp >= 0)
    {
#ifdef COSMOS_WIN_OS
        if (closesocket(channel.cudp) < 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        if (close(channel.cudp) < 0)
        {
            iretn = -errno;
        }
#endif
        //    channel.address[0] = 0;
        channel.cudp = -1;
    }
    return iretn;
}

//! Discover interfaces
/*! Return a vector of ::socket_channel containing info on each valid interface. For IPV4 this
 *	will include the address and broadcast address, in both string sockaddr_in format.
    \param ntype Type of network (Multicast, Broadcast UDP, CSP)
    \return Vector of interfaces
    */
vector<socket_channel> socket_find_addresses(NetworkType ntype, uint16_t port)
{
    vector<socket_channel> iface;

#ifdef COSMOS_WIN_OS
    INTERFACE_INFO ilist[20];
    unsigned long nbytes;
    uint32_t nif;
    uint32_t ip, net, bcast;
#else
    struct ifconf confa;
    struct ifreq *ifra;
    char data[512];
#endif // COSMOS_WIN_OS
    int32_t iretn;
    int on = 1;
    int32_t cudp;

    switch (ntype)
    {
    case NetworkType::MULTICAST:
    case NetworkType::UDP:
    case NetworkType::BROADCAST:
        {
            if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
            {
                return (iface);
            }

            // Use above socket to find available interfaces and establish
            // publication on each.
#ifdef COSMOS_WIN_OS
            if (WSAIoctl(cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
            {
                CLOSE_SOCKET(cudp);
                return (iface);
            }

            nif = nbytes / sizeof(INTERFACE_INFO);
            PIP_ADAPTER_ADDRESSES pAddresses = NULL;
            PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
            pAddresses = (IP_ADAPTER_ADDRESSES *) calloc(sizeof(IP_ADAPTER_ADDRESSES), 2*nif);
            ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES) * 2 * nif;
            DWORD dwRetVal;
            if ((dwRetVal=GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen)) == ERROR_BUFFER_OVERFLOW)
            {
                free(pAddresses);
                return (iface);
            }

            for (uint32_t i=0; i<nif; i++)
            {
                socket_channel tiface;
                tiface.cudp = cudp;
                inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,tiface.address,sizeof(tiface.address));
                //			strncpy(tiface.address, inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr), 17);
                if (!strcmp(tiface.address,"127.0.0.1"))
                {
                    continue;
                }

                pCurrAddresses = pAddresses;
                while (pAddresses)
                {
                    if (((struct sockaddr_in *)(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr))->sin_addr.s_addr == ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.s_addr)
                    {
                        strcpy(tiface.name, pCurrAddresses->AdapterName);
                        break;
                    }
                    pCurrAddresses = pCurrAddresses->Next;
                }
                memset(&tiface.caddr,0,sizeof(struct sockaddr_in));
                memset(&tiface.baddr,0,sizeof(struct sockaddr_in));
                tiface.caddr.sin_family = AF_INET;
                tiface.baddr.sin_family = AF_INET;
                if (ntype == NetworkType::MULTICAST)
                {
                    inet_pton(AF_INET,(char *)COSMOSMCAST,&tiface.caddr.sin_addr);
                    inet_pton(AF_INET,(char *)COSMOSMCAST,&tiface.baddr.sin_addr);
                    //				struct sockaddr_storage ss;
                    //			    int sslen;
                    //				sslen = sizeof(ss);
                    //				WSAStringToAddressA((char *)COSMOSMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                    //				tiface.caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                    //				tiface.baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                }
                else
                {
                    if ((iretn = setsockopt(tiface.cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                    {
                        continue;
                    }
                    ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                    net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                    bcast = ip | (~net);

                    tiface.caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                    tiface.caddr.sin_addr.S_un.S_addr = ip;
                    tiface.baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                    tiface.baddr.sin_addr.S_un.S_addr = bcast;
                }
                //			struct sockaddr_storage ss;
                //			((struct sockaddr_in *)&ss)->sin_addr = tiface.caddr.sin_addr;
                //			ssize = strlen(tiface.address);
                //			WSAAddressToStringA((struct sockaddr *)&tiface.caddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.address, (LPDWORD)&ssize);
                inet_ntop(tiface.caddr.sin_family,&tiface.caddr.sin_addr,tiface.address,sizeof(tiface.address));
                //			ssize = strlen(tiface.baddress);
                //			WSAAddressToStringA((struct sockaddr *)&tiface.baddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.baddress, (LPDWORD)&ssize);
                inet_ntop(tiface.baddr.sin_family,&tiface.baddr.sin_addr,tiface.baddress,sizeof(tiface.baddress));
                tiface.type = ntype;
                iface.push_back(tiface);
            }
#else
            confa.ifc_len = sizeof(data);
            confa.ifc_buf = (caddr_t)data;
            if (ioctl(cudp,SIOCGIFCONF,&confa) < 0)
            {
                CLOSE_SOCKET(cudp);
                return (iface);
            }
            // Use result to discover interfaces.
            ifra = confa.ifc_req;
            for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++)
            {
                if (ifra->ifr_addr.sa_family != AF_INET)
                {
                    continue;
                }

                socket_channel tiface;
                // Open socket again if we had to close it
                if ((tiface.cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                {
                    continue;
                }

                if (fcntl(tiface.cudp, F_SETFL,O_NONBLOCK) < 0)
                {
                    iretn = -errno;
                    CLOSE_SOCKET(tiface.cudp);
                    tiface.cudp = iretn;
                    continue;
                }

                inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));
                memcpy((char *)&tiface.caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));

                if (ioctl(tiface.cudp,SIOCGIFFLAGS, (char *)ifra) < 0)
                {
                    continue;
                }
                tiface.flags = ifra->ifr_flags;

                if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || ((ifra->ifr_flags & (IFF_BROADCAST)) == 0 && (ifra->ifr_flags & (IFF_POINTOPOINT)) == 0))
                {
                    continue;
                }

                if (ntype == NetworkType::MULTICAST)
                {
                    inet_pton(AF_INET,COSMOSMCAST,&tiface.caddr.sin_addr);
                    strncpy(tiface.baddress, COSMOSMCAST, 17);
                    inet_pton(AF_INET,COSMOSMCAST,&tiface.baddr.sin_addr);
                }
                else
                {
                    if ((tiface.flags & IFF_POINTOPOINT))
                    {
                        if (ioctl(tiface.cudp,SIOCGIFDSTADDR,(char *)ifra) < 0)
                        {
                            continue;
                        }
                        tiface.baddr = tiface.caddr;
                        inet_ntop(ifra->ifr_dstaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_dstaddr)->sin_addr,tiface.baddress,sizeof(tiface.baddress));
                        inet_pton(AF_INET,tiface.baddress,&tiface.baddr.sin_addr);

                    }
                    else
                    {
                        if ((iretn = setsockopt(tiface.cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                        {
                            CLOSE_SOCKET(tiface.cudp);
                            continue;
                        }

                        strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);

                        if (ioctl(tiface.cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                        {
                            continue;
                        }
                        memcpy((char *)&tiface.baddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
                    }

                    if (ioctl(tiface.cudp,SIOCGIFADDR,(char *)ifra) < 0)
                    {
                        continue;
                    }
                    memcpy((char *)&tiface.caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));
                    inet_ntop(tiface.baddr.sin_family,&tiface.baddr.sin_addr,tiface.baddress,sizeof(tiface.baddress));
                }

                // Find assigned port, place in cport, and set caddr to requested port
                socklen_t namelen = sizeof(struct sockaddr_in);
                if ((iretn = getsockname(tiface.cudp, (sockaddr*)&tiface.caddr, &namelen)) == -1)
                {
                    CLOSE_SOCKET(tiface.cudp);
                    continue;
                }
                tiface.cport = ntohs(tiface.caddr.sin_port);
                tiface.caddr.sin_port = htons(port);
                inet_pton(AF_INET,tiface.address,&tiface.caddr.sin_addr);
                tiface.baddr.sin_port = htons(port);
                tiface.type = ntype;
                iface.push_back(tiface);
            }

#endif // COSMOS_WIN_OS
        }
        break;
    default:
        break;
    }

    return (iface);
}

int32_t socket_poll(socket_bus &bus, vector<uint8_t> &buffer, size_t maxlen, int flags)
{
    for (socket_channel channel : bus)
    {
        int count;
        if (ioctl(channel.cudp, FIONREAD, count) == 0 && count)
        {
            return socket_recvfrom(channel, buffer, maxlen, flags);
        }
    }
    return 0;
}

int32_t socket_recvfrom(socket_channel &channel, string &buffer, size_t maxlen, int flags)
{
    int32_t iretn;
    vector<uint8_t> data;
    iretn = socket_recvfrom(channel, data, maxlen, flags);
    string str(data.begin(), data.end());
    buffer = str;
    return iretn;
}

int32_t socket_recvfrom(socket_channel &channel, vector<uint8_t> &buffer, size_t maxlen, int flags)
    {
    int32_t nbytes;
    buffer.resize(maxlen);
    if ((nbytes = recvfrom(channel.cudp, (char *)buffer.data(), maxlen, flags, (struct sockaddr *)&channel.caddr, (socklen_t *)&channel.addrlen)) > 0)
    {
        buffer.resize(nbytes);
        inet_ntop(channel.caddr.sin_family, &channel.caddr.sin_addr, channel.address, sizeof(channel.address));
    }
    else
    {
        buffer.clear();
        nbytes = -errno;
    }
    return nbytes;
}

int32_t socket_recv(socket_channel &channel, vector<uint8_t> &buffer, size_t maxlen, int flags)
    {
    int32_t nbytes;
    buffer.resize(maxlen);
    if ((nbytes = recv(channel.cudp, (char *)buffer.data(), maxlen, flags)) > 0)
    {
        buffer.resize(nbytes);
    }
    else
    {
        buffer.clear();
        nbytes = -errno;
    }
    return nbytes;
}

int32_t socket_post(socket_bus &channel, const string buffer, int flags)
{
    vector<uint8_t> data(buffer.begin(), buffer.end());
    return socket_post(channel, data, flags);
}

int32_t socket_post(socket_bus &bus, const vector<uint8_t> buffer, int flags)
{
    vector<uint8_t> data(buffer.begin(), buffer.end());
    for (socket_channel channel : bus)
    {
        int32_t iretn = socket_post(channel, data, flags);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    return 0;
}

int32_t socket_post(socket_channel &channel, const string buffer, int flags)
{
    vector<uint8_t> data(buffer.begin(), buffer.end());
    return socket_post(channel, data, flags);
}

int32_t socket_post(socket_channel &channel, const vector<uint8_t> buffer, int flags)
{
    int32_t nbytes;
#if defined(COSMOS_WIN_OS)
    nbytes = sendto(channel.cudp, (const char *)(buffer.data()), buffer.size(), flags, (struct sockaddr *)&channel.baddr, channel.addrlen);
#else
    if ((nbytes = sendto(channel.cudp, (buffer.data()), buffer.size(), flags, reinterpret_cast<struct sockaddr *>(&channel.baddr), static_cast<socklen_t>(sizeof(struct sockaddr_in)))) < 0)
    {
        nbytes = -errno;
    }
#endif
    return nbytes;
}

int32_t socket_sendto(socket_bus &bus, const string buffer, int flags)
{
    vector<uint8_t> data(buffer.begin(), buffer.end());
    for (socket_channel channel : bus)
    {
        int32_t iretn = socket_sendto(channel, data, flags);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    return 0;
}

int32_t socket_sendto(socket_channel &channel, const string buffer, int flags)
{
    vector<uint8_t> data(buffer.begin(), buffer.end());
    return socket_sendto(channel, data, flags);
}

int32_t socket_sendto(socket_channel &channel, const vector<uint8_t> buffer, int flags)
{
    int32_t nbytes;
#if defined(COSMOS_WIN_OS)
    nbytes = sendto(channel.cudp, (const char *)(buffer.data()), buffer.size(), flags, (struct sockaddr *)&channel.caddr, sizeof(struct sockaddr_in));
#else
    if ((nbytes = sendto(channel.cudp, const_cast<uint8_t*>(buffer.data()), buffer.size(), flags, reinterpret_cast<struct sockaddr *>(&channel.caddr), static_cast<socklen_t>(sizeof(struct sockaddr_in)))) < 0)
    {
        nbytes = -errno;
    }
#endif
    return nbytes;
}

int32_t socket_send(socket_channel &channel, const vector<uint8_t> buffer, int flags)
{
    int32_t nbytes;
#if defined(COSMOS_WIN_OS)
    nbytes = send(channel.cudp, (const char *)(buffer.data()), buffer.size(), flags);
#else
    if ((nbytes = send(channel.cudp, const_cast<uint8_t*>(buffer.data()), buffer.size(), flags)) < 0)
    {
        nbytes = -errno;
    }
#endif
    return nbytes;
}


//! Open UDP socket
/*! Open a UDP socket and configure it for the specified use. Various
flags are set, and the socket is bound, if necessary. Support is
provided for the extra steps necessary for MS Windows.
    \return Zero, or negative error.
*/
// TODO: try to merge with socket_open
int32_t Udp::socketOpen()
{
    socklen_t namelen;
    int32_t iretn;
    struct ip_mreq mreq;
    int on = 1;
    int debug = false; //turn on or off debug statements

#ifdef COSMOS_WIN_OS
    unsigned long nonblocking = 1;
    struct sockaddr_storage ss;
    int sslen;
    WORD wVersionRequested;
    WSADATA wsaData;
    static bool started=false;

    //Initialise winsock
    if (debug){std::cout << "\nInitialising Winsock...";}
    if (!started)
    {
        wVersionRequested = MAKEWORD( 1, 1 );
        //wVersionRequested = MAKEWORD( 2, 2 );
        //iretn = WSAStartup( wVersionRequested, &wsaData );

        if ( (iretn=WSAStartup(wVersionRequested,&wsaData)) != 0)
        {
            if (debug){printf("Failed. Error Code : %d",WSAGetLastError());
                return errno;
            }
        }
        if (debug){std::cout << "Initialised." << std::endl;}
    }
#endif

    //Create a socket
    if (sok.stream){
        if ((sok.handle = socket(AF_INET, SOCK_STREAM,0)) <0){
            {
                if (debug){
#ifdef COSMOS_WIN_OS
                    printf("Could not create socket stream: %d" , WSAGetLastError());
#else
                    printf("Could not create socket stream : %d" , errno);
#endif
                }
                return (-errno);
            }
        }
    } else {
        //default
        if ((sok.handle = socket(AF_INET, SOCK_DGRAM,0)) <0){
            {
                if (debug){
#ifdef COSMOS_WIN_OS
                    printf("Could not create socket : %d" , WSAGetLastError());
#else
                    printf("Could not create socket : %d" , errno);
#endif
                }
                return (-errno);
            }
        }
    }

    if (debug){std::cout << "Socket created" << std::endl;}

    if (sok.blocking == SOCKET_NONBLOCKING)
    {
        iretn = 0;
#ifdef COSMOS_WIN_OS
        if (ioctlsocket(sok.handle, FIONBIO, &nonblocking) != 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        if (fcntl(sok.handle, F_SETFL,O_NONBLOCK) < 0)
        {
            iretn = -errno;
        }
#endif
        if (iretn < 0)
        {
            CLOSE_SOCKET(sok.handle);
            sok.handle = iretn;
            return iretn;
        }
    }

    // this defines the wait time for a response from a request
    if (sok.timeout)
    {
#ifdef COSMOS_WIN_OS
        int msectimeo = sok.timeout/1000;
        iretn = setsockopt(sok.handle,SOL_SOCKET,SO_RCVTIMEO,(const char *)&msectimeo,sizeof(msectimeo));
#else
        struct timeval tv;
        tv.tv_sec = sok.timeout/1000000;
        tv.tv_usec = sok.timeout%1000000;
        iretn = setsockopt(sok.handle,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
#endif
    }

    //Prepare the sockaddr_in structure
    memset(&sok.server, 0, sizeof(struct sockaddr_in));
    sok.server.sin_family = AF_INET;
    sok.server.sin_port = htons(sok.port);

    switch (sok.role)
    {
    case SOCKET_LISTEN:
#ifdef COSMOS_MAC_OS
        if (setsockopt(sok.handle,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on)) < 0)
#else
        if (setsockopt(sok.handle,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0)
#endif
        {
            CLOSE_SOCKET(sok.handle);
            sok.handle = -errno;
            return (-errno);
        }

        sok.server.sin_addr.s_addr = htonl(INADDR_ANY);

        //Bind
        if (::bind(sok.handle,(struct sockaddr *)&sok.server, sok.addrlen) < 0) //addrlen = sizeof(struct sockaddr_in)
        {
            if (debug){
#ifdef COSMOS_WIN_OS
                printf("Bind failed with error code : %d" , WSAGetLastError());
#endif
            }

            CLOSE_SOCKET(sok.handle);
            sok.handle = -errno;
            return (-errno);
        }
        if (debug){std::cout << "Bind done" << std::endl;}

        // If we bound to port 0, then find out what our assigned port is.
        if (!sok.port)
        {
            namelen = sizeof(struct sockaddr_in);
            if ((iretn = getsockname(sok.handle, (sockaddr*)&sok.server, &namelen)) == -1)
            {
                CLOSE_SOCKET(sok.handle);
                sok.handle = -errno;
                return (-errno);
            }
            sok.port = ntohs(sok.server.sin_port);
        }
        else
        {
            //>>
            //port = port;
        }

        if (sok.type == NetworkType::MULTICAST)
        {
            //! 2. Join multicast
            mreq.imr_multiaddr.s_addr = inet_addr(sok.address.c_str());
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            if (setsockopt(sok.handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
            {
                CLOSE_SOCKET(sok.handle);
                sok.handle = -errno;
                return (-errno);
            }
        }


        break;
    case SOCKET_JABBER:
        switch (sok.type)
        {
        case NetworkType::UDP:
        case NetworkType::BROADCAST:
            if ((iretn=setsockopt(sok.handle,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
            {
                CLOSE_SOCKET(sok.handle);
                sok.handle = -errno;
                return (-errno);
            }
            sok.server.sin_addr.s_addr = 0xffffffff;
            break;
        case NetworkType::MULTICAST:
#ifndef COSMOS_WIN_OS
            inet_pton(AF_INET,sok.address.c_str(),&sok.server.sin_addr);
#else
            sslen = sizeof(ss);
            WSAStringToAddressA((LPSTR)sok.address.c_str(),AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
            sok.server.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
            break;
        default:
            return (SOCKET_ERROR_PROTOCOL);
            break;
        }
        // >>
        //port = port;
        break;
        //------------------------------------------------------
    case SOCKET_TALK:
#ifndef COSMOS_WIN_OS
        inet_pton(AF_INET,sok.address.c_str(),&sok.server.sin_addr);
#else
        sslen = sizeof(ss);
        WSAStringToAddressA((LPSTR)sok.address.c_str(),AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
        sok.server.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
        //>>
        //port = port;

        if (sok.connect){
            if ((iretn=connect(sok.handle, (struct sockaddr *)&sok.server, sok.addrlen)) < 0)
            {
#ifdef COSMOS_WIN_OS
                iretn = WSAGetLastError();
                if (iretn != WSAEWOULDBLOCK)
                {
                    return (errno);
                }
#else
                iretn = errno;
                if (iretn != EINPROGRESS)
                {
                    return (errno);
                }
#endif
                COSMOS_SLEEP(1);
            }
        }


        break;
    }

    return 0;
}


//-------------------------------------------------------------------
// Simple UDP class to send data

int32_t Udp::setupClient(){
    return socketOpen();
}

int32_t Udp::setupClient(string a, uint16_t p){
    // config
    sok.address = a;
    sok.port = p;

    return socketOpen();
}

int32_t Udp::setupClientSimGen(string a, uint16_t p){
    // config
    sok.address     = a;
    sok.port        = p;
    sok.blocking    = SOCKET_NONBLOCKING;
    sok.stream      = true;
    sok.timeout     = 0;
    sok.connect     = true;

    return socketOpen();
}

int32_t Udp::setupClientAcstb(string a, uint16_t p){
    // config
    sok.address     = a;
    sok.port        = p;
    sok.blocking    = SOCKET_NONBLOCKING;
    sok.stream      = true;
    sok.timeout     = 0;
    sok.connect     = true;

    return socketOpen();

}


//int32_t Udp::openClient(){

//    int32_t iretn;
//    //SocketOptions options;
//    //if ((iretn=socket_open(&socket, NetworkType::UDP, address.c_str(), port, SOCKET_TALK, SOCKET_BLOCKING, SOCKET_RCVTIMEO)) < 0)

//    if ((iretn=socketOpen()) < 0)
//    {
//        return (-errno);
//    }
//    return 0;
//}

/*
int32_t Udp::setupServer(){
    return openServer();
}
*/

int32_t Udp::setupServer(uint16_t port, float timeout_sec){
    // the sok.server does not need an ip because it runs on the
    // local computer (with preassigned ip)
    sok.port        = port;
    sok.type        = NetworkType::UDP;
    sok.role        = SOCKET_LISTEN;
    sok.blocking    = SOCKET_BLOCKING;
    sok.timeout     = timeout_sec*1000000;

    //    int socket_timeout = 0;
    //    if (timeout == 0){
    //        socket_timeout = SOCKET_RCVTIMEO;
    //    } else {
    //        socket_timeout = timeout;
    //    }

    if ((iretn=socketOpen()) < 0)
    {
        return (-errno);
    }
    return 0;
}

/*
int32_t Udp::openServer(){

    int32_t iretn;

    if ((iretn=socketOpen()) < 0)
    {
        return (-errno);
    }

    return 0;
}
*/

// Udp class default contructor
Udp::Udp(){
    // default values (for Matlab)
    //sok.address     = "127.0.0.1";
    //sok.port        = 8888;

    //sok("127.0.0.1",8888);
    //sok {"127.0.0.1",8888};
    //iretn = init();
}

//Udp class overloaded contructor to use
//other ip and port
//Udp::Udp(string a, uint16_t p){
//    sok.address = a;
//    sok.port    = p;

//    //SocketOptions(a,p);
//    //iretn = init();
//}


//Udp class overloaded contructor to use
//other ip and port
//Udp::Udp(string a, uint16_t p, uint16_t r){
//    sok.address = a;
//    sok.port    = p;
//    sok.role    = r;
//    //SocketOptions(a,p,r);
//}


int32_t Udp::errorStatus(string functionName){

#ifdef COSMOS_WIN_OS
    errno = WSAGetLastError();
    std::cout << functionName << " failed with error code :" << errno <<  " (" << strerror(errno) << ")" << std::endl;
    std::cout << "Check the Windows Sockets Error Codes to get more information: http://msdn.microsoft.com/en-us/library/windows/desktop/ms740668%28v=vs.85%29.aspx" << std::endl;
#else
    std::cout << functionName << " failed with error code :" << errno <<  " (" << strerror(errno) << ")" << std::endl;
#endif

    // return negative error number
    return -errno;
    //exit(EXIT_FAILURE);
}

int32_t Udp::send(string package2send){

    if (sendto(sok.handle,                      // socket
               package2send.c_str(),            // buffer to send
               strlen(package2send.c_str()),    // size of buffer
               0,                               // flags
               (struct sockaddr *)&sok.server,  // socket address
               sok.addrlen)                     // size of address to socket pointer //sizeof(struct sockaddr_in))
        < 0){

        return errorStatus("Udp::send");
    }
    //std::cout << "sent: " << package2send << std::endl;
    // return 0 on sucess
    return 0;
}



int32_t Udp::receiveOnce(){
    // collects the data from the Udp channel
    // and puts into the receivedData string
    // that is defined on the Udp class
    char buf[SOCKET_BUFFER_LENGTH];
    int recv_len = -1;

    //keep listening for data
    //std::cout << "Waiting for data...";

    //clear the buffer by filling null, it might have previously received data
    memset(buf,'\0', SOCKET_BUFFER_LENGTH);


    ElapsedTime ep;
    ep.print = false;
    ep.tic();
    float timeout = 5.0; // seconds
    double timer = -1.;

    // just to test timer
    //COSMOS_SLEEP(0.001);

    while ( (recv_len < 0) && (timer < timeout) ){
        // keep trying to receive message
        //receivedStatus = udp.receiveOnce();

        //try to receive some data
        recv_len = recvfrom(sok.handle,
                            buf,
                            SOCKET_BUFFER_LENGTH,
                            0,
                            (struct sockaddr *) &sok.s_other,
                            (socklen_t *)&sok.addrlen);

        timer = ep.toc();
        //std::cout << recv_len << " | " << timer << std::endl;
        //std::cout << " (udp rx: " << recv_len << " bytes | " << timer << " sec)" << std::endl;


        if (recv_len < 0)
        {
#ifdef COSMOS_WIN_OS
            if (WSAGetLastError() == 10035){
                //std::cout << "Resource temporarily unavailable. Continue." << std::endl;
                continue;
            }
#endif
            return errorStatus("Udp::receiveOnce");
            //continue;
        }

        if (recv_len > 0) {
            //print details of the client/peer and the data received
            //        std::cout << "Received packet from " << inet_ntoa(sok.server.sin_addr) << ":" << ntohs(sok.port) << std::endl;
            //        std::cout << "Data: " << buf << std::endl;

            //std::cout << std::endl << "<< udp rx: " << recv_len << " bytes | " << std::setprecision(3) << std::fixed << timer << " sec" << std::endl;

            recv_len = 0;
            timer = 0;
            receivedData = string(buf);
            return 0;
        }
        //
    }

    std::cout << "Failed to receive data in less than 5 sec. Elapsed time: " << ep.toc() << std::endl;
    return -1;
}



int32_t Udp::receiveLoop(){
    char buf[SOCKET_BUFFER_LENGTH];
    int recv_len;

    //keep listening for data
    std::cout << "Waiting for data...";

    while(1){

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', SOCKET_BUFFER_LENGTH);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(sok.handle,
                                 buf,
                                 SOCKET_BUFFER_LENGTH,
                                 0,
                                 (struct sockaddr *) &sok.s_other,
                                 (socklen_t *)&sok.addrlen)) < 0){
            return errorStatus("Udp::receiveLoop");
        }

        if (recv_len > 0) {
            //print details of the client/peer and the data received
            std::cout << "Received packet from " << inet_ntoa(sok.server.sin_addr) << ":" << ntohs(sok.port) << std::endl;
            std::cout << "Data: " << buf << std::endl;
            recv_len = 0;
        }

        //now reply the client with the same data
        if (sendto(sok.handle,
                   buf,
                   recv_len,
                   0,
                   (struct sockaddr*) &sok.s_other,
                   sok.addrlen) < 0){
            return errorStatus("Udp::receiveLoop");
        }

    }


    return 0;
}


int32_t Udp::close(){

#ifdef COSMOS_WIN_OS
    closesocket(sok.handle);
    WSACleanup();
#endif

    return 0;
}
