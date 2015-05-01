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

#include "socketlib.h"
#include "mathlib.h"

//! Open UDP socket
/*! Open a UDP socket and configure it for the specified use. Various
flags are set, and the socket is bound, if necessary. Support is
provided for the extra steps necessary for MS Windows.
	\param channel Pointer to ::socket_channel holding final configuration.
    \param address Destination address
    \param port Source port. If zero, automatically assigned.
    \param role Publish, subscribe, communicate.
    \param blocking True or false.
    \param usectimeo Blocking read timeout in micro seconds.
    \return Zero, or negative error.
*/
int32_t socket_open(socket_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t role, bool blocking, uint32_t usectimeo)
{
    socklen_t namelen;
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
	case SOCKET_TYPE_BROADCAST:
	case SOCKET_TYPE_MULTICAST:
		{
			if ((channel->cudp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) <0)
			{
				return (-errno);
			}
		}
		break;
	case SOCKET_TYPE_TCP:
		{
			if ((channel->cudp = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) <0)
			{
				return (-errno);
			}
		}
		break;
	}

    if (blocking == SOCKET_NONBLOCKING)
    {
        iretn = 0;
#ifdef COSMOS_WIN_OS
        if (ioctlsocket(channel->cudp, FIONBIO, &nonblocking) != 0)
        {
            iretn = -WSAGetLastError();
        }
#else
        if (fcntl(channel->cudp, F_SETFL,O_NONBLOCK) < 0)
        {
            iretn = -errno;
        }
#endif
        if (iretn < 0)
        {
            CLOSE_SOCKET(channel->cudp);
            channel->cudp = iretn;
            return (iretn);
        }
    }

    // this defines the wait time for a response from a request
    if (usectimeo)
    {
#ifdef COSMOS_WIN_OS
        int msectimeo = usectimeo/1000;
        iretn = setsockopt(channel->cudp,SOL_SOCKET,SO_RCVTIMEO,(const char *)&msectimeo,sizeof(msectimeo));
#else
        struct timeval tv;
        tv.tv_sec = usectimeo/1000000;
        tv.tv_usec = usectimeo%1000000;
        iretn = setsockopt(channel->cudp,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
#endif
    }

    memset(&channel->caddr,0,sizeof(struct sockaddr_in));
    channel->caddr.sin_family = AF_INET;
    channel->caddr.sin_port = htons(port);

    switch (role)
    {
    case SOCKET_LISTEN:
#ifdef COSMOS_MAC_OS
        if (setsockopt(channel->cudp,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on)) < 0)
#else
        if (setsockopt(channel->cudp,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0)
#endif
        {
            CLOSE_SOCKET(channel->cudp);
            channel->cudp = -errno;
            return (-errno);
        }

		switch (ntype)
		{
		case SOCKET_TYPE_BROADCAST:
		case SOCKET_TYPE_MULTICAST:
			{
				channel->caddr.sin_addr.s_addr = htonl(INADDR_ANY);
				if (::bind(channel->cudp,(struct sockaddr *)&channel->caddr, sizeof(struct sockaddr_in)) < 0)
				{
					CLOSE_SOCKET(channel->cudp);
					channel->cudp = -errno;
					return (-errno);
				}
			}
			break;
		case SOCKET_TYPE_TCP:
			{
				channel->caddr.sin_addr.s_addr = htonl(INADDR_ANY);
				channel->caddr.sin_port = htons(port);
				if (::bind(channel->cudp,(struct sockaddr *)&channel->caddr, sizeof(struct sockaddr_in)) < 0)
				{
					CLOSE_SOCKET(channel->cudp);
					channel->cudp = -errno;
					return (-errno);
				}

				if (listen(channel->cudp, 1) < 0)
				{
					CLOSE_SOCKET(channel->cudp);
					channel->cudp = -errno;
					return (-errno);
				}
			}
			break;
		}

        // If we bound to port 0, then find out what our assigned port is.
        if (!port)
        {
            namelen = sizeof(struct sockaddr_in);
            if ((iretn = getsockname(channel->cudp, (sockaddr*)&channel->caddr, &namelen)) == -1)
            {
                CLOSE_SOCKET(channel->cudp);
                channel->cudp = -errno;
                return (-errno);
            }
            channel->cport = ntohs(channel->caddr.sin_port);
        }
        else
        {
            channel->cport = port;
        }

        if (ntype == SOCKET_TYPE_MULTICAST)
        {
            //! 2. Join multicast
			inet_pton(AF_INET,address,&mreq.imr_multiaddr.s_addr);
//			mreq.imr_multiaddr.s_addr = inet_addr(address);
//            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			if (setsockopt(channel->cudp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
            {
                CLOSE_SOCKET(channel->cudp);
                channel->cudp = -errno;
                return (-errno);
            }
        }
        break;
    case SOCKET_JABBER:
        switch (ntype)
        {
        case SOCKET_TYPE_UDP:
            if ((iretn=setsockopt(channel->cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
            {
                CLOSE_SOCKET(channel->cudp);
                channel->cudp = -errno;
                return (-errno);
            }
            channel->caddr.sin_addr.s_addr = 0xffffffff;
            break;
        case SOCKET_TYPE_MULTICAST:
#ifndef COSMOS_WIN_OS
            inet_pton(AF_INET,address,&channel->caddr.sin_addr);
#else
			inet_pton(AF_INET,address,&channel->caddr.sin_addr);
//			struct sockaddr_storage ss;
//		    int sslen;
//            sslen = sizeof(ss);
//            WSAStringToAddressA((char *)address,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
//            channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
            break;
        }
        channel->cport = port;
        break;
    case SOCKET_TALK:
#ifndef COSMOS_WIN_OS
        inet_pton(AF_INET,address,&channel->caddr.sin_addr);
#else
		inet_pton(AF_INET,address,&channel->caddr.sin_addr);
//		struct sockaddr_storage ss;
//	    int sslen;
//        sslen = sizeof(ss);
//        WSAStringToAddressA((char *)address,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
//        channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
        channel->cport = port;

		if (ntype == SOCKET_TYPE_TCP)
			{
				if (connect(channel->cudp,(struct sockaddr *)&channel->caddr, sizeof(struct sockaddr_in)) < 0 && errno != EINPROGRESS)
				{
					CLOSE_SOCKET(channel->cudp);
					channel->cudp = -errno;
					return (-errno);
				}
			}
		break;
    }

    strncpy(channel->address,address,17);
    channel->type = ntype;
    channel->addrlen = sizeof(struct sockaddr_in);

    return 0;
}

//! Calculate UDP Checksum
/*! Calculate UDP Checksum, as detailed in RFC 768, based on the provided IP packet.
 * \param packet IP packet
 * \param csum Location to store calculated Checksum.
 * \return Zero or negative error.
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

    //	if (csum == uint16from(&packet[26], ORDER_NETWORK))
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
    uint16to(csum, &packet[SOCKET_IP_BYTE_UDP_CS], ORDER_LITTLEENDIAN);

    return 0;
}

int32_t socket_blocking(socket_channel *channel, bool blocking)
{
	int32_t iretn;

	if (blocking == SOCKET_NONBLOCKING)
	{
		iretn = 0;
#ifdef COSMOS_WIN_OS
		unsigned long nonblocking = 1;
		if (ioctlsocket(channel->cudp, FIONBIO, &nonblocking) != 0)
		{
			iretn = -WSAGetLastError();
		}
#else
		if (fcntl(channel->cudp, F_SETFL,O_NONBLOCK) < 0)
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
		if (ioctlsocket(channel->cudp, FIONBIO, &nonblocking) != 0)
		{
			iretn = -WSAGetLastError();
		}
#else
		int oldfl;
		if ((oldfl = fcntl(channel->cudp, F_GETFL)) == -1)
		{
			iretn = -errno;
		}
		else
		{
			if (fcntl(channel->cudp, F_SETFL, oldfl & ~O_NONBLOCK) < 0)
			{
				iretn = -errno;
			}
		}
#endif
	}
	return (iretn);
}

//! Close socket
/*! Close down open socket connectiom.
	\param channel Pointer to ::socket_channel holding final configuration.
 * \return Zero or negative error.
 */
int32_t socket_close(socket_channel *channel)
{
	int32_t iretn = 0;

#ifdef COSMOS_WIN_OS
		if (closesocket(channel->cudp) < 0)
		{
			iretn = -WSAGetLastError();
		}
#else
		if (close(channel->cudp) < 0)
		{
			iretn = -errno;
		}
#endif
	return iretn;
}

//! Discover interfaces
/*! Return a vector of ::socket_channel containing info on each valid interface. For IPV4 this
 *	will include the address and broadcast address, in both string sockaddr_in format.
	\param ntype Type of network (Multicast, Broadcast UDP, CSP)
	\return Vector of interfaces
	*/
vector<socket_channel> socket_find_addresses(uint16_t ntype)
{
	vector<socket_channel> iface;
	socket_channel tiface;

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
	case SOCKET_TYPE_MULTICAST:
	case SOCKET_TYPE_UDP:
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
			inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,tiface.address,sizeof(tiface.address));
//			strcpy(tiface.address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
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
			if (ntype == SOCKET_TYPE_MULTICAST)
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
				if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
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
			if (ifra->ifr_addr.sa_family != AF_INET) continue;
			inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));

			if (ioctl(cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

			if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0) continue;

			if (ntype == SOCKET_TYPE_MULTICAST)
			{
				inet_pton(AF_INET,COSMOSMCAST,&tiface.caddr.sin_addr);\
				strcpy(tiface.baddress, COSMOSMCAST);
				inet_pton(AF_INET,COSMOSMCAST,&tiface.baddr.sin_addr);\
			}
			else
			{
				if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
				{
					continue;
				}

				strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);
				if (ioctl(cudp,SIOCGIFBRDADDR,(char *)ifra) < 0) continue;
				memcpy((char *)&tiface.baddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
				if (ioctl(cudp,SIOCGIFADDR,(char *)ifra) < 0) continue;
				memcpy((char *)&tiface.caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));
				inet_ntop(tiface.baddr.sin_family,&tiface.baddr.sin_addr,tiface.baddress,sizeof(tiface.baddress));
			}
			tiface.type = ntype;
			iface.push_back(tiface);
		}

#endif // COSMOS_WIN_OS

		break;
	}

	return (iface);
}
