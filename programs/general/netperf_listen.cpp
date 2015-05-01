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


/*
 ****************************************************************************
 *** Title: Network Performance UDP/IP Listener
 ***
 *** Purpose: Used in conjunction with netperf_send.  This program is the
 *** receiver which will take in UDP/IP packets on port 6101 by default.
 *** Statistics will be gathered, and auto-timeout will be used to end
 *** the session.
 ***
 *** Usage: - Run binary, no arguments = interactive mode
 ***        - 1 argument: any argument, prints minimal log header and result
 ***        - 2 arguments: any two arguments, prints results only
 ***        - After running listener, run sender to this listener's IP
 ***        - Use with script to pipe output to file.  CSV compatible.
 ****************************************************************************
 *** Version 1.0
 *** Target OS: Linux Only
 *** Update Log:
 *** - 2013xxxxnn
 *** Created: 7/25/2013 Jeremy Chan
 ****************************************************************************
 ***
 *** Bugs:
 *** 1. Runt packet detection is not yet implemented, need to figure out last space position to calc true intended size.  Buffer overflows may occur if cannot be detected and CRC check goes into uncharted territory.
 ****************************************************************************
*/


#include "agentlib.h"
#include "jsondef.h"
#include "sliplib.h"

#define BUFSIZE 10000
#define MJD_TO_SECONDS 86400
#define CURRENT_TIME_us currentmjd(0.0)*MJD_TO_SECONDS*1000*1000
#define INFO_SIZE 32

bool debug_verbose = false;
char address[] = "0.0.0.0";
uint16_t port = 6101;
uint16_t bsize = 10000;

int main(int argc, char *argv[])
{
	// **** Delcare Variables

	// Variables: Program Control
	bool firstpacket; // Flag indicating whether first packet has been received or not
	unsigned int timeout_count; // Limit on recvfrom timeout intervals to exit after first packet
	unsigned int idle_count; // UDP recvfrom timeout counter
	bool quietmode; // Sets interactive or quiet mode
	double t_display_timer; // Interactive display update timer
	//	bool log_header_print; // Sets whether log header will be printed or not


	// Variables: Packet Reception: Data handling
	socket_channel chan; // Agent UDP Channel Sructure
	unsigned int fromlen; // UDP receive from "fromlen" parameter
	int received; // Num bytes received per UDP packet
	uint8_t buf1[BUFSIZE]; // RX Buffer
	char buf2[BUFSIZE]; // RX Buffer
	//	char *p_data_start; // Pointer to where the packet's payload starts, specified by bsize

	double cmjd; // Test Packet: Header Mean Julian Date
	uint16_t crc; // Test Packet: Header CRC
	uint16_t bindex; // Test Packet: Header Index #
	uint16_t bsize; // Test Packet: Header Data Size, data start = bytes_received-bsize
	uint16_t bindex_next_expected; // Test Packet: Next expected header index
	uint16_t packet_crc_scratch; // Test Packet: Scratchpad for CRC calculation on data

	// Variables: Packet Reception: Statistics
	uint32_t packet_count; // Count of packets with data
	uint32_t packet_count_dropped; // Count of dropped packets (skipped index numbers)
	uint32_t packet_count_crc_err; // Count of dropped packets (CRC didn't match calculated with data)
	uint32_t packet_count_runt; // Count of runt packets (payload larger than received)
	uint32_t data_accumulator=0; // Count of total bytes received
	double initial_t,final_t; // Initial/Final time of packet activity

	double data_rate_min,data_rate_max; // Data rate: Variables to hold min/max data rate
	double data_rate_avg; // Data Rate: Scratchpad to hold current average
	double data_rate_accumulator; // Data Rate: Accumulator to hold past data rate history
	uint32_t data_rate_count; // Data Rate: Count of records in accumulator
	double data_ref_t; // Data rate: Past time record
	double data_rate; // Data rate: Current data rate scratch

	double ref_t,new_t,delta_t; // Inter-Packet Latency: Time Variables, Past/new/delta
	double t_accumulator,t_count; // Inter-Packet Latency: Time accumulator, # accum records count
	double t_min,t_max,t_avg; // Inter-Packet Latency: Min/Max/Average scratch


	// Variables: Scratchpad
	int32_t iretn; // Scratch Return value


	// **** Initialize Variables

	// Initialize Program Control Vars
	timeout_count = 10;
	idle_count = 0;
	firstpacket = false;
	quietmode = false;
	//	log_header_print = true;

	// Initialize Packet Reception Statistics Vars
	// Inter-Packet Latency
	ref_t = CURRENT_TIME_us;
	new_t = CURRENT_TIME_us;
	t_min = 99999;
	t_max = 0;
	t_accumulator = 0;
	t_count = 0;
	// Data Rate
	data_rate_min=99999999;
	data_rate_max=0;
	data_rate_accumulator=0;
	data_rate_count=0;
	data_ref_t = CURRENT_TIME_us;
	// Overall Stats
	initial_t = CURRENT_TIME_us;
	packet_count = 0;
	// Packet Error Counting
	bindex = 0;
	bindex_next_expected = 0;
	packet_count_dropped = 0;
	packet_count_crc_err = 0;
	packet_count_runt = 0;
	
	switch(argc)
	{
	case 2:
		// 1 arguments = log header only and exit
		printf("Packet Size [bytes]\tSpeed [bytes/sec]\tRX Time[us]\tPackets Received\tPacket Drops\tPacket CRC Errors\tPacket Runts\tBytes Received\tData Rate: Min [bytes/sec]\tData Rate: Average [bytes/sec]\tData Rate: Max [bytes/sec]\tInter-Packet Delay: Min [us]\tInter-Packet Delay: Avg [us]\tInter-Packet Delay: Max [us]\n");
		exit(0);
		break;
	case 3:
		// 2 argument = log entry only
		quietmode = true;
		//		log_header_print = false;
		break;
	}


	// Begin Program, Initialize Socket, Blocking, Timeout set for 1 second
	if ((iretn=socket_open(&chan, SOCKET_TYPE_UDP, address, port, AGENT_LISTEN, AGENT_BLOCKING, 2000000)) < 0)
	{
		printf("Err: Unable to open connection to [%s:%d]\n",address,port);
	}

	if(!quietmode) printf("Netperf Listen is now listening on port %d...\n\n",port);

	// Begin Main Forever Loop
	while (1)
	{
		// UDP Receive: Check for new packet, return -1 if none
		received = recvfrom( chan.cudp, (char *)buf1, BUFSIZE, 0, (struct sockaddr*) &chan.caddr, (socklen_t*) &fromlen);
		if (received < 1)
		{
			// If result: No new packet received
			idle_count++; // Increment idle count

			// Report idle
			if(!quietmode) printf("[Idle %d...%d]\n",idle_count, received);
			if(!quietmode) fflush(stdout);

			if(idle_count >= timeout_count && firstpacket == true)
			{
				if(!quietmode) printf("Idle timeout after activity, exiting.\n\n");

				// Begin Exit Report Code
				final_t = new_t;	// Log final time

				// Data Rate Statististics
				data_rate = data_accumulator / ( (final_t - initial_t) / 1000000); // Get [Bytes/s]

				//printf("Exiting, %.0fus %d \n\n",final_t-initial_t, );
				if(!quietmode) printf("Final Report: \n");
				//				if(log_header_print) printf("Packet Size [bytes]\tSpeed [bytes/sec]\tRX Time[us]\tPackets Received\tPacket Drops\tPacket CRC Errors\tBytes Received\tData Rate: Average [bytes/sec]\tInter-Packet Delay: Avg [us]\n");
				if (t_count)
				{
					t_avg = t_accumulator / t_count; // Update average IPD
				}
				else
				{
					t_avg = 0;
				}
				printf("%.0f\t%d\t%d\t%d\t%d\t%.0f\t%.0f\n",(final_t-initial_t),packet_count,packet_count_dropped,packet_count_crc_err,data_accumulator,data_rate,t_avg);
				
				fflush(stdout);
				// End Exit Report Code

				exit(0);
			}
		}
		else
		{
			// If result: packet received
			// Update Inter-Packet Delay Statistics
			new_t = CURRENT_TIME_us; // Get new time
			delta_t = new_t - ref_t; // Get time difference in milliseconds
			ref_t = new_t; // Set new reference time
			if (delta_t < t_min) t_min = delta_t; // Update minium inter-packet delay
			if (delta_t > t_max) t_max = delta_t; // Update maximum inter-packet delay
			t_accumulator += delta_t; // Accumulate inter-packet delay (IPD) records
			t_count++; // Increment count of accumulated IPD records
			t_avg = t_accumulator / t_count; // Update average IPD

			// Parse Packet
			// Fields: currentmjd, count, slip_calc_crc, bsize, binary data
			strncpy(buf2,(const char *)buf1,INFO_SIZE);
			buf2[INFO_SIZE] =0;
			sscanf((char *)buf2,"%lf %hu %hx %hu",&cmjd,&bindex,&crc,&bsize);
			if (debug_verbose)
			{
				printf("Packet: %f, %hu, %hx, %hu\n", cmjd, bindex, crc, bsize);
				printf("PACKET: %s\n",buf2);
			}
			// numsizeread - bsize = start position


			// Test Packet: Sequence Check
			if(bindex != bindex_next_expected)
			{
				packet_count_dropped+=(bindex - bindex_next_expected);
				if (!quietmode) printf("\nError: Index Received: %u, Index Expected: %u, Missed %d packets\n", bindex, bindex_next_expected, bindex-bindex_next_expected);
			}
			bindex_next_expected = bindex + 1;

			// Test Packet: Runt Check
			// Null terminate at least 5 times beyond buffer to ensure strtok does not crash the program if incomplete packet is received.
			buf1[received]=0;
			buf1[received+1]=0;
			buf1[received+2]=0;
			buf1[received+3]=0;
			buf1[received+4]=0;

			// Advance p_data_start to theoretical beginning of data
			//			p_data_start = strtok((char*)buf1, " ");
			//			p_data_start = strtok(NULL, " ");
			//			p_data_start = strtok(NULL, " ");
			//			p_data_start = strtok(NULL, " ");



			if (received < bsize)
			{
				packet_count_runt++;
				if (!quietmode) printf("\nError: Bytes dropped: packet %d expected %u received %u\n", bindex_next_expected, bsize, received);
			}
			else
			{
				// Test Packet: CRC Check
				packet_crc_scratch = slip_calc_crc((uint8_t *) &buf1[INFO_SIZE],bsize-INFO_SIZE);
				if (crc != packet_crc_scratch)
				{
					packet_count_crc_err++;
					if (!quietmode) printf("\nError: Index %d has bad CRC when comparing packet %x to calc %x. \n",bindex, crc, packet_crc_scratch);
				} else {
					if (debug_verbose)printf("\nIndex %d packet %x calc %x \n",bindex, crc, packet_crc_scratch);
				}
			}


			// Overall statistics
			packet_count++; // Packet Count update
			data_accumulator += received; // Increment bytes received

			// Program control: Reset idle count to back off timeout
			idle_count = 0; // Reset idle count
			t_display_timer += delta_t; // Accumulate time since last reset

			// Check if this is the first packet
			if (!firstpacket)
			{
				// If result: first packet found
				initial_t = data_ref_t = new_t; // Data rate: update new past-reference time to be first packet time
				firstpacket = true; // Flag first packet received is now true
				t_display_timer = 0 ; // Reset display timer, delay 1 second
				data_accumulator -= received;
			}


			// Display Control

			// Run display routine every second (assuming t_display_timer in microseconds)
			if (t_display_timer > 1000000) {
				// If result: display timer beyond 1 second
				t_display_timer -= 1000000; // Back off display timer by 1 second

				// Data Rate Statististics
				data_rate = (data_accumulator) / ((new_t - data_ref_t) / 1000000) ; // get Bytes/s
				data_rate_accumulator += data_rate; // Add current data rate to accumulator records
				data_rate_count++; // Update data rate accumulator record count
				data_rate_avg = data_rate_accumulator / data_rate_count; // Calculate average data rate
				if (data_rate < data_rate_min) data_rate_min = data_rate; // Update min data rate
				if (data_rate > data_rate_max) data_rate_max = data_rate; // Update max data rate

				// Interactive display routine
				if(!quietmode)
				{
					printf("%d packets: DRate(1s/Avg): %.1f/%.1f [Bytes/s], I-P Time (Min/Avg/Max): %.0f/%.0f/%.0f [us]\n",packet_count,data_rate,data_rate_avg,t_min,t_avg,t_max);
					fflush(stdout);
				}

				t_display_timer = 0; // Reset Display Timer
				//				data_ref_t = CURRENT_TIME_us; // Reset reference time for data rate

			} // End If: Stopwatch Display Routine

		} // End If: packet reception / parse / idle cycle

	} // End Main Forever Loop

} // End Main


/* Code Deleted!!

//uint16_t speed = 281250;

	//uint8_t buf2[BUFSIZE];
	switch (argc)
	{
	case 4:
		speed = atoi(argv[4]);
	case 4:

	case 3:
		port = atoi(argv[2]);
	case 2:
		strcpy(address,argv[1]);
		break;
	}
				accumulator_reset--;
				if (accumulator_reset == 0)
				{
					accumulator_reset = 60; // Reset accumulator every 60 seconds
					t_max = 0;
					t_min = 999999;
					data_rate_max = 0;
					data_rate_min = 0;
					data_rate_accumulator = 0;
					data_rate_count = 0;
					//t_accumulator = 0;
					//t_count = 0 ;
				}
*/

