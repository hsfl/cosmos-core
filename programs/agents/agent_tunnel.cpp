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

#include "support/configCosmos.h"

#include <stdio.h>

#if defined(COSMOS_LINUX_OS)
#include <net/if.h>
#include <linux/if_tun.h>
#endif


#include "agent/agentclass.h"
#include "device/serial/serialclass.h"

static Agent *agent; // to access the cosmos data, will change later

void tcv_read_loop();
void tcv_write_loop();
void tun_read_loop();
void tun_write_loop();

static std::queue<std::vector<uint8_t> > tun_fifo;
static std::queue<std::vector<uint8_t> > tcv_fifo;

static std::condition_variable tcv_fifo_check;
static std::condition_variable tun_fifo_check;

static int tun_fd;

static string rxr_devname;
static string txr_devname;
static Serial *rxr_serial=nullptr;
static Serial *txr_serial=nullptr;
static uint32_t rxr_baud = 9600;
static uint32_t txr_baud = 9600;

#define MAXBUFFERSIZE 2560 // comm buffer for agents
#define TUN_BUF_SIZE 2000

int main(int argc, char *argv[])
{
#if defined(COSMOS_LINUX_OS)

    int32_t iretn;
	char tunnel_ip[20];
	std::vector<uint8_t> buffer;

	switch (argc)
	{
	case 4:
        {
            rxr_devname = argv[3];
            size_t tloc = rxr_devname.find(":");
            if (tloc != string::npos)
            {
                rxr_baud = atol(rxr_devname.substr(tloc+1, rxr_devname.size()-(tloc+1)).c_str());
                rxr_devname = rxr_devname.substr(0, tloc);
            }
            // Open receiver port
            rxr_serial = new Serial(rxr_devname, rxr_baud);
            if (rxr_serial->get_error() < 0)
            {
                printf("Error opening %s as receiver - %s\n",rxr_devname.c_str(), cosmos_error_string(rxr_serial->get_error()).c_str());
                exit (-1);
            }
        }
	case 3:
        {
            txr_devname = argv[2];
            size_t tloc = txr_devname.find(":");
            if (tloc != string::npos)
            {
                txr_baud = atol(txr_devname.substr(tloc+1, txr_devname.size()-tloc+1).c_str());
                txr_devname = txr_devname.substr(0, tloc);
            }
            // Open receiver port
            txr_serial = new Serial(txr_devname, txr_baud);
            if (txr_serial->get_error() < 0)
            {
                printf("Error opening %s as receiver - %s\n",txr_devname.c_str(), cosmos_error_string(txr_serial->get_error()).c_str());
                exit (-1);
            }
            // Copy transmitter to  receiver port if not already open (Duplex)
            if (rxr_serial == nullptr)
                rxr_serial = txr_serial;
            // Get address for tunnel
            strcpy(tunnel_ip,argv[1]);
        }
		break;
	default:
		printf("Usage: agent_tunnel ip_address transmit_device [receive_device]\n");
		exit (-1);
	}

	// Initialize the Agent
    agent = new Agent("", "tunnel", 1., MAXBUFFERSIZE, true);
    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to start Agent %s on Node %s : %s\n", agent->getAgent().c_str(), agent->getNode().c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }


	// Start serial threads
	thread tcv_read_thread(tcv_read_loop);
	thread tcv_write_thread(tcv_write_loop);

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
    strncpy(ifr1.ifr_name, agent->cinfo->agent[0].beat.proc, IFNAMSIZ);
    if (ioctl(tun_fd, TUNSETIFF, static_cast<void *>(&ifr1)) < 0)
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
    strncpy(ifr2.ifr_name, agent->cinfo->agent[0].beat.proc, IFNAMSIZ);
	ifr2.ifr_addr.sa_family = AF_INET;

	// Set interface address

	inet_pton(AF_INET, tunnel_ip, &addr->sin_addr);
	if (ioctl(tunnel_sock, SIOCSIFADDR, &ifr2) < 0 )
	{
		perror("Error setting tunnel address");
		exit (-1);
	}

	// Set interface netmask
    inet_pton(AF_INET, static_cast<const char *>("255.255.255.0"), &addr->sin_addr);
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
	thread tun_read_thread(tun_read_loop);
	thread tun_write_thread(tun_write_loop);

	double nmjd = currentmjd(0.);
	int32_t sleept;

	// Start performing the body of the agent
    while(agent->running())
	{
		// Set beginning of next cycle;
        nmjd += agent->cinfo->agent[0].aprd/86400.;

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
    ssize_t nbytes;

    while (agent->running())
	{

		buffer.resize(TUN_BUF_SIZE);
		nbytes = read(tun_fd, &buffer[0], TUN_BUF_SIZE);
		if (nbytes > 0)
		{	// Start of mutex for tcv fifo
            buffer.resize(static_cast<size_t>(nbytes));
			tcv_fifo.push(buffer);
			tcv_fifo_check.notify_one();
		}	// End of mutex for tcv fifo
	}
}

void tun_write_loop()
{
	std::vector<uint8_t> buffer;
    ssize_t nbytes;
	std::mutex tun_fifo_lock;
	std::unique_lock<std::mutex> locker(tun_fifo_lock);

    while (agent->running())
	{

        tun_fifo_check.wait(locker);
        // End of mutex for tun fifo

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
    ssize_t nbytes;

    while (agent->running())
	{
		// Read data from receiver port
		buffer.resize(TUN_BUF_SIZE);
        nbytes = rxr_serial->get_slip(buffer, TUN_BUF_SIZE);
		if (nbytes > 0)
		{ // Start of mutex for tun FIFO
            buffer.resize(static_cast<size_t>(nbytes));
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
}

void tcv_write_loop()
{
	std::mutex tcv_fifo_lock;
	std::unique_lock<std::mutex> locker(tcv_fifo_lock);
	std::vector<uint8_t> buffer;

    while (agent->running())
	{

        tcv_fifo_check.wait(locker);

		while (!tcv_fifo.empty())
		{
			// Get next packet from transceiver FIFO
			buffer = tcv_fifo.front();
			tcv_fifo.pop();
			// Write data to transmitter port
            txr_serial->put_slip(buffer);
		}
	}
}

#endif // #ifdef COSMOS_LINUX_OS
