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
#include "socketlib.h"

#include <stdio.h>

#if defined(COSMOS_LINUX_OS)
#include <net/if.h>
#include <linux/if_tun.h>
#endif


#include "agent/agent.h"
#include "cssl_lib.h"
#include "kpc9612p_lib.h"

char agentname[COSMOS_MAX_NAME+1] = "tunnel";
char node[50] = "";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

CosmosAgent *agent; // to access the cosmos data, will change later

void tcv_read_loop();
void tcv_write_loop();
void tun_read_loop();
void tun_write_loop();

std::queue<std::vector<uint8_t> > tun_fifo;
std::queue<std::vector<uint8_t> > tcv_fifo;

std::mutex tcv_fifo_lock;
std::condition_variable tcv_fifo_check;
std::mutex tun_fifo_lock;
std::condition_variable tun_fifo_check;

int tun_fd;

char rxr_devname[20]="";
char txr_devname[20]="";
//cssl_t *rxr_serial = NULL;
//cssl_t *txr_serial = NULL;
kpc9612p_handle rxr_handle;
kpc9612p_handle txr_handle;

#define MAXBUFFERSIZE 2560 // comm buffe for agents
#define TUN_BUF_SIZE 2000
#define BAUD 19200

int main(int argc, char *argv[])
{

    std::string tunnel_ip;
    std::vector<uint8_t> buffer;
	int32_t iretn;

	//	cssl_start();

	switch (argc)
	{
	case 4:
		strcpy(rxr_devname,argv[3]);
		// Open receiver port
		if ((iretn=kpc9612p_connect(rxr_devname, &rxr_handle, 0x00)) < 0)
		{
			printf("Error opening %s as receiver\n",rxr_devname);
			exit (-1);
		}
	case 3:
		strcpy(txr_devname,argv[2]);
		// Open transmitter port
		if ((iretn=kpc9612p_connect(txr_devname, &txr_handle, 0x00)) < 0)
		{
			printf("Error opening %s as transmitter\n",txr_devname);
			exit (-1);
		}
		// Copy transmitter to  receiver port if not already open (Duplex)
		if (rxr_handle.serial == NULL)
			rxr_handle = txr_handle;
		// Get address for tunnel
		tunnel_ip = argv[1];
		break;
	default:
		printf("Usage: agent_tunnel ip_address transmit_device [receive_device]\n");
		exit (-1);
		break;
	}

	// Initialize the Agent
    if (!(agent = new CosmosAgent(NetworkType::UDP, "", ("tunnel_"+tunnel_ip), 1., MAXBUFFERSIZE)))
		exit (AGENT_ERROR_JSON_CREATE);

	// Start serial threads
    std::thread tcv_read_thread(tcv_read_loop);
    std::thread tcv_write_thread(tcv_write_loop);

#if defined(COSMOS_LINUX_OS)
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
    strncpy(ifr1.ifr_name, agent->cinfo->pdata.agent[0].beat.proc, IFNAMSIZ);
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
    strncpy(ifr2.ifr_name, agent->cinfo->pdata.agent[0].beat.proc, IFNAMSIZ);
	ifr2.ifr_addr.sa_family = AF_INET;

	// Set interface address

	inet_pton(AF_INET, tunnel_ip.c_str(), &addr->sin_addr);
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
	ifr2.ifr_mtu = KPC9612P_MTU;
	if (ioctl(tunnel_sock, SIOCSIFMTU, &ifr2) < 0 )
	{
		perror("Error setting tunnel interface MTU");
		exit (-1);
	}


	close(tunnel_sock);
#endif

	// Start tunnel threads
    std::thread tun_read_thread(tun_read_loop);
    std::thread tun_write_thread(tun_write_loop);

	double nmjd = currentmjd(0.);
	int32_t sleept;

	// Start performing the body of the agent
    while(agent->running())
	{
		// Set beginning of next cycle;
        nmjd += agent->cinfo->pdata.agent[0].aprd/86400.;

		sleept = (int32_t)((nmjd - currentmjd(0.))*86400000000.);
		if (sleept < 0)
		{
			sleept = 0;
		}
		COSMOS_USLEEP(sleept);
	}
	tun_read_thread.join();
	tcv_read_thread.join();
	tun_write_thread.join();
	tcv_write_thread.join();
	exit (0);
}

//#if defined(COSMOS_LINUX_OS) || defined(COSMOS_MAC_OS)
void tun_read_loop()
{
    std::vector<uint8_t> buffer;
	int32_t nbytes;

    while (agent->running())
	{

		buffer.resize(TUN_BUF_SIZE);
		nbytes = read(tun_fd, &buffer[0], TUN_BUF_SIZE);
		if (nbytes > 0)
		{
			buffer.resize(nbytes);

			// Add UDP checksum if necessary
			if (buffer[SOCKET_IP_BYTE_PROTOCOL] == SOCKET_IP_PROTOCOL_UDP)
			{
				socket_set_udp_checksum(buffer);
			}

			tcv_fifo.push(buffer);
			tcv_fifo_check.notify_one();
		}
	}
	tcv_fifo_check.notify_all();
}

void tun_write_loop()
{
    std::vector<uint8_t> buffer;
	int32_t nbytes;

    while (agent->running())
	{
		{	// Start of mutex for tun fifo
            std::unique_lock<std::mutex> locker(tun_fifo_lock);

			while (tun_fifo.empty())
			{
				tun_fifo_check.wait(locker);
			}	// End of mutex for tun fifo
		}

		while (!tun_fifo.empty())
		{
			buffer = tun_fifo.front();
			nbytes = write(tun_fd, &buffer[0], buffer.size());
			if (nbytes > 0)
			{
//				printf("Out TUN: [%u,%u] ", nbytes, buffer.size());
//				for (uint16_t i=0; i<(buffer.size()<150?buffer.size():150); ++i)
//				{
//					if (i == 0)
//					{
//						printf("[ ");
//					}
//					printf("%x ", buffer[i]);
//					if (i+1 == buffer.size() || i == 149)
//					{
//						printf("] ");
//					}
//				}
//				printf("\n");
				tun_fifo.pop();
			}
		}
	}
}

void tcv_read_loop()
{
    std::vector<uint8_t> buffer;
	int32_t iretn;
	double lastin = currentmjd(0.);
	double lastbeacon = currentmjd(0.);

    while (agent->running())
	{
		// Read data from receiver port
		iretn = kpc9612p_recvframe(&rxr_handle);
		if (iretn > 0)
		{ // Start of mutex for tun FIFO
			kpc9612p_unloadframe(&rxr_handle, buffer);
			//			uint16_t cs1 = kpc9612p_calc_fcs(&buffer[17], buffer.size()-17);
			//			uint16_t cs2 = *(uint16_t*)(astroin->payload+astroin->header.size-4);
			if (buffer[SOCKET_IP_BYTE_VERSION]>>4 == 4 && buffer.size() == buffer[SOCKET_IP_BYTE_LEN_HIGH]*256U+buffer[SOCKET_IP_BYTE_LEN_LOW])
			{
				if (buffer[SOCKET_IP_BYTE_PROTOCOL] != SOCKET_IP_PROTOCOL_UDP || (buffer[SOCKET_IP_BYTE_PROTOCOL] == SOCKET_IP_PROTOCOL_UDP && !socket_check_udp_checksum(buffer)))
				{
					tun_fifo.push(buffer);
					tun_fifo_check.notify_one();
					printf("Buffer: [%u,%" PRIu32 "] %f\n", rxr_handle.frame.size, buffer.size(), 86400.*(currentmjd(0.)-lastin));
					lastin = currentmjd(0.);
//					for (uint16_t i=0; i<(rxr_handle.frame.size<150?rxr_handle.frame.size:150); ++i)
//					{
//						if (i == 17)
//						{
//							printf("[ ");
//						}
//						printf("%x ", rxr_handle.frame.full[i]);
//						if (i == buffer.size()+16 || i == 149)
//						{
//							printf("] ");
//						}
//					}
//					printf("\n");
					fflush(stdout);
				}
				else
				{
					printf("UDP checksum error:\n");
				}
			}
			else
			{
				printf("Beacon: [%d,%u,%" PRIu32 "] %f\n", iretn, rxr_handle.frame.size, buffer.size(), 86400.*(currentmjd(0.)-lastbeacon));
				lastbeacon = currentmjd(0.);
//				std::string str(buffer.begin(), buffer.end());
//				std::cout << "\t" << str << std::endl;
			}
		} // End of mutex for tun FIFO
	}
	tun_fifo_check.notify_all();

	kpc9612p_disconnect(&rxr_handle);
}

void tcv_write_loop()
{
    std::vector<uint8_t> buffer;
//	int32_t iretn;
//	double lastout = currentmjd(0.);

    while (agent->running())
	{
		{	// Start of mutex for tcv FIFO
            std::unique_lock<std::mutex> locker(tcv_fifo_lock);

			while (tcv_fifo.empty())
			{
				tcv_fifo_check.wait(locker);
			}
		}	// End of mutex for tcv FIFO

		while (!tcv_fifo.empty())
		{
			// Get next packet from transceiver FIFO
			buffer = tcv_fifo.front();
			tcv_fifo.pop();
			// Write data to transmitter port
			kpc9612p_loadframe(&txr_handle, buffer);
			kpc9612p_sendframe(&txr_handle);
//			printf("Out Radio: [%d,%u,%u] %f\n", iretn, txr_handle.frame.size, buffer.size(), 86400.*(currentmjd(0.)-lastout));
//			lastout = currentmjd(0.);
//			if (iretn >= 0)
//			{
//				for (uint16_t i=0; i<(txr_handle.frame.size<150?txr_handle.frame.size:150); ++i)
//				{
//					if (i == 17)
//					{
//						printf("[ ");
//					}
//					if (i == buffer.size()+17)
//					{
//						printf("] ");
//					}
//					printf("%x ", txr_handle.frame.full[i]);
//				}
//				printf("\n");
//			}
		}
	}

	kpc9612p_disconnect(&txr_handle);
}

//#endif // #ifdef COSMOS_LINUX_OS
