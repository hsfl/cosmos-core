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

#include "configCosmos.h"

#include <stdio.h>

#if defined(COSMOS_LINUX_OS)
#include <net/if.h>
#include <linux/if_tun.h>
#endif


#include "agentlib.h"
#include "cssl_lib.h"

char agentname[COSMOS_MAX_NAME+1] = "tunnel";
char node[50] = "";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

cosmosstruc *cdata; // to access the cosmos data, will change later

void tcv_read_loop();
void tcv_write_loop();
void tun_read_loop();
void tun_write_loop();

std::queue<std::vector<uint8_t> > tun_fifo;
std::queue<std::vector<uint8_t> > tcv_fifo;

std::condition_variable tcv_fifo_check;
std::condition_variable tun_fifo_check;

int tun_fd;

char rxr_devname[20]="";
char txr_devname[20]="";
cssl_t *rxr_serial = NULL;
cssl_t *txr_serial = NULL;

#define MAXBUFFERSIZE 2560 // comm buffe for agents
#define TUN_BUF_SIZE 2000
#define BAUD 19200

int main(int argc, char *argv[])
{
#if defined(COSMOS_LINUX_OS)

	char tunnel_ip[20];
	std::vector<uint8_t> buffer;

	cssl_start();

	switch (argc)
	{
	case 4:
		strcpy(rxr_devname,argv[3]);
		// Open receiver port
		if ((rxr_serial=cssl_open(rxr_devname,BAUD,8,0,1)) == NULL)
		{
			printf("Error opening %s as receiver\n",rxr_devname);
			exit (-1);
		}
	case 3:
		strcpy(txr_devname,argv[2]);
		// Open transmitter port
		txr_serial = cssl_open(txr_devname,BAUD,8,0,1);
		if ((txr_serial=cssl_open(txr_devname,BAUD,8,0,1)) == NULL)
		{
			printf("Error opening %s as transmitter\n",txr_devname);
			exit (-1);
		}
		// Copy transmitter to  receiver port if not already open (Duplex)
		if (rxr_serial == NULL)
			rxr_serial = txr_serial;
		// Get address for tunnel
		strcpy(tunnel_ip,argv[1]);
		break;
	default:
		printf("Usage: agent_tunnel ip_address transmit_device [receive_device]\n");
		exit (-1);
		break;
	}

	// Initialize the Agent
	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,(char *)NULL,(char *)"tunnel",1.,0,MAXBUFFERSIZE,AGENT_MULTIPLE)))
		exit (AGENT_ERROR_JSON_CREATE);

	// Start serial threads
	std::thread tcv_read_thread(tcv_read_loop);
	std::thread tcv_write_thread(tcv_write_loop);

	// Open tunnel device
	int tunnel_sock;
	struct ifreq ifr1, ifr2;
	struct sockaddr_in *addr = (struct sockaddr_in *)&ifr2.ifr_addr;

	if ((tun_fd=open("/dev/net/tun", O_RDWR)) < 0)
	{
		perror("Error opening tunnel device");
		exit (-1);
	}

	memset(&ifr1, 0, sizeof(ifr1));
	ifr1.ifr_flags = IFF_TUN | IFF_NO_PI;
	strncpy(ifr1.ifr_name, cdata[0].agent[0].beat.proc, IFNAMSIZ);
	if (ioctl(tun_fd, TUNSETIFF, (void *)&ifr1) < 0)
	{
		perror("Error setting tunnel interface");
		exit (-1);
	}

	if((tunnel_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
	{
		perror("Error opening tunnel socket");
		exit (-1);
	}

	// Get ready to set things
	strncpy(ifr2.ifr_name, cdata[0].agent[0].beat.proc, IFNAMSIZ);
	ifr2.ifr_addr.sa_family = AF_INET;

	// Set interface address

	inet_pton(AF_INET, tunnel_ip, &addr->sin_addr);
	if (ioctl(tunnel_sock, SIOCSIFADDR, &ifr2) < 0 )
	{
		perror("Error setting tunnel address");
		exit (-1);
	}

	// Set interface netmask
	inet_pton(AF_INET, (char *)"255.255.255.0", &addr->sin_addr);
	if (ioctl(tunnel_sock, SIOCSIFNETMASK, &ifr2) < 0 )
	{
		perror("Error setting tunnel netmask");
		exit (-1);
	}

	if (ioctl(tunnel_sock, SIOCGIFFLAGS, &ifr2) < 0 )
	{
		perror("Error getting tunnel interface flags");
		exit (-1);
	}

	// Bring interface up
	ifr2.ifr_flags |= (IFF_UP | IFF_RUNNING);
	if (ioctl(tunnel_sock, SIOCSIFFLAGS, &ifr2) < 0 )
	{
		perror("Error setting tunnel interface flags");
		exit (-1);
	}

	// Set interface MTU
	ifr2.ifr_mtu = 250;
	if (ioctl(tunnel_sock, SIOCSIFMTU, &ifr2) < 0 )
	{
		perror("Error setting tunnel interface MTU");
		exit (-1);
	}


	close(tunnel_sock);

	// Start tunnel threads
	std::thread tun_read_thread(tun_read_loop);
	std::thread tun_write_thread(tun_write_loop);

	double nmjd = currentmjd(0.);
	int32_t sleept;

	// Start performing the body of the agent
	while(agent_running(cdata))
	{
		// Set beginning of next cycle;
		nmjd += cdata[0].agent[0].aprd/86400.;

		sleept = (int32_t)((nmjd - currentmjd(0.))*86400000000.);
		if (sleept < 0)
		{
			sleept = 0;
		}
		COSMOS_USLEEP(sleept);
	}
#endif
	exit (0);
}

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_MAC_OS)
void tun_read_loop()
{
	std::vector<uint8_t> buffer;
	int32_t nbytes;

	while (agent_running(cdata))
	{

		buffer.resize(TUN_BUF_SIZE);
		nbytes = read(tun_fd, &buffer[0], TUN_BUF_SIZE);
		if (nbytes > 0)
		{	// Start of mutex for tcv fifo
			buffer.resize(nbytes);
			tcv_fifo.push(buffer);
			tcv_fifo_check.notify_one();
		}	// End of mutex for tcv fifo
	}
}

void tun_write_loop()
{
	std::vector<uint8_t> buffer;
	int32_t nbytes;
	std::mutex tun_fifo_lock;
	std::unique_lock<std::mutex> locker(tun_fifo_lock);

	while (agent_running(cdata))
	{

//			while (tun_fifo.empty())
//			{
				tun_fifo_check.wait(locker);
//			}	// End of mutex for tun fifo

		while (!tun_fifo.empty())
		{
			buffer = tun_fifo.front();
			nbytes = write(tun_fd, &buffer[0], buffer.size());
			if (nbytes > 0)
			{
				tun_fifo.pop();
			}
		}
	}
}

void tcv_read_loop()
{
	std::vector<uint8_t> buffer;
	int32_t nbytes;

	while (agent_running(cdata))
	{
		// Read data from receiver port
		buffer.resize(TUN_BUF_SIZE);
		nbytes = cssl_getslip(rxr_serial, &buffer[0], TUN_BUF_SIZE);
		if (nbytes > 0)
		{ // Start of mutex for tun FIFO
			buffer.resize(nbytes);
			tun_fifo.push(buffer);
			tun_fifo_check.notify_one();
			printf("In:\n");
			for (uint16_t i=0; i<(buffer.size()<100?buffer.size():100); ++i)
			{
				printf("%x ", buffer[i]);
			}
			printf("\n");
		} // End of mutex for tun FIFO
	}

	cssl_close(rxr_serial);
}

void tcv_write_loop()
{
	std::mutex tcv_fifo_lock;
	std::unique_lock<std::mutex> locker(tcv_fifo_lock);
	std::vector<uint8_t> buffer;

	while (agent_running(cdata))
	{

//			while (tcv_fifo.empty())
//			{
				tcv_fifo_check.wait(locker);
//			}

		while (!tcv_fifo.empty())
		{
			// Get next packet from transceiver FIFO
			buffer = tcv_fifo.front();
			tcv_fifo.pop();
			// Write data to transmitter port
			cssl_putslip(txr_serial, &buffer[0], buffer.size());
		}
	}

	cssl_close(txr_serial);
}

#endif // #ifdef COSMOS_LINUX_OS
