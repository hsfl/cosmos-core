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

/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
 
struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
sd = socket(AF_INET, SOCK_DGRAM, 0);
if(sd < 0)
{
perror("Opening datagram socket error");
exit(1);
}
else
printf("Opening datagram socket....OK.\n");
 
/* Enable SO_REUSEADDR to allow multiple instances of this */
/* application to receive copies of the multicast datagrams. */
{
int reuse = 1;
if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
{
perror("Setting SO_REUSEADDR error");
close(sd);
exit(1);
}
else
printf("Setting SO_REUSEADDR...OK.\n");
}
 
/* Bind to the proper port number with the IP address */
/* specified as INADDR_ANY. */
memset((char *) &localSock, 0, sizeof(localSock));
localSock.sin_family = AF_INET;
localSock.sin_port = htons(4321);
localSock.sin_addr.s_addr = INADDR_ANY;
if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
{
perror("Binding datagram socket error");
close(sd);
exit(1);
}
else
printf("Binding datagram socket...OK.\n");
 
/* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
/* called for each local interface over which the multicast */
/* datagrams are to be received. */
group.imr_multiaddr.s_addr = inet_addr("225.1.1.1"); //226.1.1.1
group.imr_interface.s_addr = inet_addr("127.0.0.1"); //203.106.93.94
if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
{
perror("Adding multicast group error");
close(sd);
exit(1);
}
else
printf("Adding multicast group...OK.\n");
 
/* Read from the socket. */
while(1){
datalen = sizeof(databuf);
if(read(sd, databuf, datalen) < 0)
{
perror("Reading datagram message error");
close(sd);
exit(1);
}
else
{
printf("Reading datagram message...OK.\n");
printf("The message from multicast server is: \"%s\"\n", databuf);
}
}
return 0;
}