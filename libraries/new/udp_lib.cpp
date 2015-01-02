//#include "socketlib.h"
#include "udp_lib.h"
#include "mathlib.h"
#include "timelib.h"
#include "elapsedtime.hpp"

#include <iostream>

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
int32_t Udp::socketOpen()
//int32_t socket_open(socket_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t role, bool blocking, uint32_t usectimeo)
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
    if (debug){cout << "\nInitialising Winsock...";}
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
        if (debug){cout << "Initialised." << endl;}
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

    if (debug){cout << "Socket created" << endl;}

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
            return (iretn);
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
    memset(&sok.server,0,sizeof(struct sockaddr_in));
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
        if (debug){cout << "Bind done" << endl;}

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

        if (sok.type == SOCKET_TYPE_MULTICAST)
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
        case SOCKET_TYPE_UDP:
            if ((iretn=setsockopt(sok.handle,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
            {
                CLOSE_SOCKET(sok.handle);
                sok.handle = -errno;
                return (-errno);
            }
            sok.server.sin_addr.s_addr = 0xffffffff;
            break;
        case SOCKET_TYPE_MULTICAST:
#ifndef COSMOS_WIN_OS
            inet_pton(AF_INET,sok.address,&sok.server.sin_addr);
#else
            sslen = sizeof(ss);
            WSAStringToAddressA((LPSTR)sok.address.c_str(),AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
            sok.server.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
            break;
        }
        // >>
        //port = port;
        break;
        //------------------------------------------------------
    case SOCKET_TALK:
#ifndef COSMOS_WIN_OS
        inet_pton(AF_INET,sok.address,&sok.server.sin_addr);
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
        printf("UDP checksum error: %x\n", csum);
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
//    //if ((iretn=socket_open(&socket, SOCKET_TYPE_UDP, address.c_str(), port, SOCKET_TALK, SOCKET_BLOCKING, SOCKET_RCVTIMEO)) < 0)

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
    sok.type        = SOCKET_TYPE_UDP;
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
    cout << functionName << " failed with error code :" << errno <<  " (" << strerror(errno) << ")" << endl;
    cout << "Check the Windows Sockets Error Codes to get more information: http://msdn.microsoft.com/en-us/library/windows/desktop/ms740668%28v=vs.85%29.aspx" << endl;
#else
    cout << functionName << " failed with error code :" << errno <<  " (" << strerror(errno) << ")" << endl;
#endif

    // return negative error number
    return -errno;
    //exit(EXIT_FAILURE);
}

int32_t Udp::send(string package2send){

    if (sendto(sok.handle,
               package2send.c_str(),
               strlen(package2send.c_str()),
               0,
               (struct sockaddr *)&sok.server, //(struct sockaddr *) &socket.caddr,
               sok.addrlen) //sizeof(struct sockaddr_in))
            < 0){

        return errorStatus("Udp::send");
    }
    //cout << "sent: " << package2send << endl;
    // return 0 on sucess
    return 0;
}



int32_t Udp::receiveOnce(){
    // collects the data from the Udp channel
    // and puts into the receivedData string
    // that is defined on the Udp class
    char buf[SOCKET_BUFFER_LENGHT];
    int recv_len = -1;

    //keep listening for data
    //cout << "Waiting for data...";

    //clear the buffer by filling null, it might have previously received data
    memset(buf,'\0', SOCKET_BUFFER_LENGHT);


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
                            SOCKET_BUFFER_LENGHT,
                            0,
                            (struct sockaddr *) &sok.s_other,
                            &sok.addrlen);

        timer = ep.toc();
        //cout << recv_len << " | " << timer << endl;
        //cout << " (udp rx: " << recv_len << " bytes | " << timer << " sec)" << endl;


        if (recv_len == SOCKET_ERROR){
            if (WSAGetLastError() == 10035){
                //cout << "Resource temporarily unavailable. Continue." << endl;
                continue;
            }
            return errorStatus("Udp::receiveOnce");
            //continue;
        }

        if (recv_len > 0) {
            //print details of the client/peer and the data received
            //        cout << "Received packet from " << inet_ntoa(sok.server.sin_addr) << ":" << ntohs(sok.port) << endl;
            //        cout << "Data: " << buf << endl;

            //cout << endl << "<< udp rx: " << recv_len << " bytes | " << setprecision(3) << fixed << timer << " sec" << endl;

            recv_len = 0;
            timer = 0;
            receivedData = string(buf);
            return 0;
        }
        //
    }

    cout << "Failed to receive data in less than 5 sec. Elapsed time: " << ep.toc() << endl;
    return -1;
}



int32_t Udp::receiveLoop(){
    char buf[SOCKET_BUFFER_LENGHT];
    int recv_len;

    //keep listening for data
    cout << "Waiting for data...";

    while(1){

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', SOCKET_BUFFER_LENGHT);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(sok.handle,
                                 buf,
                                 SOCKET_BUFFER_LENGHT,
                                 0,
                                 (struct sockaddr *) &sok.s_other,
                                 &sok.addrlen)) == SOCKET_ERROR){
            return errorStatus("Udp::receiveLoop");
        }

        if (recv_len > 0) {
            //print details of the client/peer and the data received
            cout << "Received packet from " << inet_ntoa(sok.server.sin_addr) << ":" << ntohs(sok.port) << endl;
            cout << "Data: " << buf << endl;
            recv_len = 0;
        }

        //now reply the client with the same data
        if (sendto(sok.handle,
                   buf,
                   recv_len,
                   0,
                   (struct sockaddr*) &sok.s_other,
                   sok.addrlen) == SOCKET_ERROR){
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
