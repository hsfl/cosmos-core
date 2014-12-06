#include "socketlib.h"
#include "mathlib.h"

//! Open UDP socket
/*! Open a UDP socket and configure it for the specified use. Various
flags are set, and the socket is bound, if necessary. Support is
provided for the extra steps necessary for MS Windows.
    \param channel Pointer to ::agent_channel holding final configuration.
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
    struct sockaddr_storage ss;
    int sslen;
    WORD wVersionRequested;
    WSADATA wsaData;
    static bool started=false;

    if (!started)
    {
        wVersionRequested = MAKEWORD( 1, 1 );
        iretn = WSAStartup( wVersionRequested, &wsaData );
    }
#endif

    if ((channel->cudp = socket(AF_INET,SOCK_DGRAM,0)) <0)
    {
        return (-errno);
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

        channel->caddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(channel->cudp,(struct sockaddr *)&channel->caddr, sizeof(struct sockaddr_in)) < 0)
        {
            CLOSE_SOCKET(channel->cudp);
            channel->cudp = -errno;
            return (-errno);
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
            mreq.imr_multiaddr.s_addr = inet_addr(address);
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
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
            sslen = sizeof(ss);
            WSAStringToAddressA((char *)address,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
            channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
            break;
        }
        channel->cport = port;
        break;
    case SOCKET_TALK:
#ifndef COSMOS_WIN_OS
        inet_pton(AF_INET,address,&channel->caddr.sin_addr);
#else
        sslen = sizeof(ss);
        WSAStringToAddressA((char *)address,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
        channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
        channel->cport = port;
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

	if (blocking == AGENT_NONBLOCKING)
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
