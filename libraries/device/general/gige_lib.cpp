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

/*! \file gige_lib.cpp
	\brief Support routines for GigE Vision cameras.
*/

#include "gige_lib.h"
#include "timelib.h"
#include "math/mathlib.h"
#include <cstring>

#ifdef COSMOS_WIN_OS
#include <io.h>
#else
#include <sys/select.h>
#include <sys/ioctl.h>
#endif

//! \ingroup gige
//! \addtogroup sinclair_functions
//! @{

//! Connect to camera
/*! Establish a GVCP control connection to a GIGE camera, at the
 * indicated address, with the requested privileges. Once the
 * camera connection is opened, some register must be either read or
 * written within the supplied Heartbeat_Timeout period or the conection
 * will be shut down.
	\param address IP address of the desired camera
	\param privilege Requested privilege.
	\param heartbeat_msec Period between commands to keep connection open.
	\param socket_usec Timeout on socket listen calls.
	\param streambps Bytes per Second throttle on camera flow rate (socket_usec will be increased to match decreasing streambps.)
	\return A handle to the camera to be used for all subsequent
	calls.
*/
gige_handle *gige_open(char address[18],uint8_t privilege, uint32_t heartbeat_msec, uint32_t socket_usec, uint32_t streambps)
{
	int32_t iretn;
	int32_t nbytes;
	uint32_t myip, theirip, bcastip;
	struct sockaddr_in raddr;
	gige_handle *handle;
	uint8_t bufferin[GIGE_MAX_PACKET];
	std::vector<socket_channel> ifaces;

	if ((handle=new (gige_handle)) == NULL) return nullptr;

	if (streambps < 100000) streambps = 100000;
	if (streambps > 100000000) streambps = 100000000;
	handle->streambps = streambps;

	// Adjust SO_RCVTIMEO to be long enough to receive twice largest command packet at expected flow rate.
	if (socket_usec < (uint32_t)(1024000000/streambps)) socket_usec = (uint32_t)(1024000000/streambps);

	// Open Command socket
	if ((iretn=socket_open(&handle->command, NetworkType::UDP, address, 3956, SOCKET_TALK, true, socket_usec)) < 0)
	{
		delete(handle);
		return nullptr;
	}

	// See if we can get control
    iretn = gige_writereg(handle, GIGE_REG_CCP,privilege);

	// Set Heartbeat Timeout
	if ((iretn = gige_writereg(handle,GIGE_REG_GVCP_HEARTBEAT_TIMEOUT,heartbeat_msec)) < 0)
	{
		close(handle->command.cudp);
		delete(handle);
		return nullptr;
	}

	// Adjust SO_RCVTIMEO to be long enough to receive twice stream largest packet at expected flow rate.
    if (socket_usec < (uint32_t)(16384000000./streambps)) socket_usec = (uint32_t)(16384000000/streambps);

	// Open Stream socket
	if ((iretn=socket_open(&handle->stream, NetworkType::UDP, (char *)"", 0, SOCKET_LISTEN,true,socket_usec)) < 0)
	{
		close(handle->command.cudp);
		return nullptr;
		delete(handle);
	}

	uint32_t n=134217728;
//	uint32_t n=83886080;
//	uint32_t n=838860;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	setsockopt(handle->stream.cudp, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
#else
	setsockopt(handle->stream.cudp, SOL_SOCKET, SO_RCVBUF, (const char *)&n, sizeof(n));
#endif

	// Find our own IP address to send to camera
	theirip = (uint32_t)(*(uint32_t*)&handle->command.caddr.sin_addr);
	ifaces = socket_find_addresses(NetworkType::UDP);
	for (uint16_t i=0; i<ifaces.size(); ++i)
	{
		myip = (uint32_t)(*(uint32_t*)&ifaces[i].caddr.sin_addr);
		bcastip = (uint32_t)(*(uint32_t*)&ifaces[i].baddr.sin_addr);
		if ((theirip & (~bcastip)) == (myip & (~bcastip)))
			strcpy(handle->stream.address,ifaces[i].address);
	}
	memcpy((char *)&handle->stream.caddr, (char *)&raddr, sizeof(raddr));

		if ((iretn=gige_writereg(handle,GIGE_REG_SCDA,gige_address_to_value(handle->stream.address))) < 0)
	{
		close(handle->command.cudp);
		close(handle->stream.cudp);
		delete(handle);
		return nullptr;
	}

	if ((iretn=gige_writereg(handle,GIGE_REG_SCP,handle->stream.cport)) < 0)
	{
			close(handle->command.cudp);
			close(handle->stream.cudp);
			delete(handle);
			return nullptr;
		}

	uint32_t bsize;
	for (bsize=512; bsize<GIGE_MAX_PACKET; bsize+=100)
	{
		if (bsize > GIGE_MAX_PACKET) bsize = GIGE_MAX_PACKET;
		iretn = gige_writereg(handle,GIGE_REG_SCPS,0xc0000000+bsize);
		iretn = gige_readreg(handle,GIGE_REG_SCPS)%65536;
        nbytes=recvfrom(handle->stream.cudp,(char *)bufferin,GIGE_MAX_PACKET,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
		if (nbytes < 0) break;
	}
	handle->bestsize = bsize - 100;

	if ((iretn=gige_writereg(handle,GIGE_REG_SCPS,(uint32_t)handle->bestsize)) < 0)
	{
		close(handle->command.cudp);
		close(handle->stream.cudp);
		delete(handle);
		return nullptr;
	}
	iretn = gige_writereg(handle,GIGE_REG_SCP,0);

	return (handle);
}

//! Close GigE Camera
/*! Close an existing Control Channel to a GigE camera by writing 0 to
the CCP register and closing all sockets.
	\param handle Handle for GigE camera as returned from ::gige_open.
*/
void gige_close(gige_handle *handle)
{
	
	gige_writereg(handle, GIGE_REG_CCP,0x00000000);
	close(handle->command.cudp);
	close(handle->stream.cudp);
}

//! Write Register
/*! Write indicated GigE register with provided data.
	\param handle Handle for GigE camera as returned from ::gige_open.
    \param address Address of register.
	\param data Data to be written.
	\return Zero or negative error.
*/
int gige_writereg(gige_handle *handle, uint32_t address, uint32_t data)
{
	int32_t nbytes, ncount;

    uint16to(0x4201,(uint8_t *)&handle->creg.flag,ByteOrder::BIGENDIAN);
    uint16to(GIGE_CMD_WRITEREG,(uint8_t *)&handle->creg.command,ByteOrder::BIGENDIAN);
    uint16to(0x0008,(uint8_t *)&handle->creg.length,ByteOrder::BIGENDIAN);
    uint16to(++handle->req_id,(uint8_t *)&handle->creg.req_id,ByteOrder::BIGENDIAN);
    uint32to(address,(uint8_t *)&handle->creg.address,ByteOrder::BIGENDIAN);
    uint32to(data,(uint8_t *)&handle->creg.data,ByteOrder::BIGENDIAN);
    if ((nbytes=sendto(handle->command.cudp,(char *)handle->cbyte,16,0,(struct sockaddr *)&handle->command.caddr,sizeof(handle->command.caddr))) < 0)
	{
#ifdef COSMOS_WIN_OS
		return(-WSAGetLastError());
#else
		return (-errno);
#endif
	}

	handle->command.addrlen = sizeof(handle->command.caddr);

	ncount = 100;
	do
	{
        nbytes = recvfrom(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,(socklen_t *)&handle->command.addrlen);
	} while (nbytes <= 0 && ncount--);

    handle->cack.status = uint16from((uint8_t *)&handle->cack.status,ByteOrder::BIGENDIAN);
    handle->cack.acknowledge = uint16from((uint8_t *)&handle->cack.acknowledge,ByteOrder::BIGENDIAN);
    handle->cack.length = uint16from((uint8_t *)&handle->cack.length,ByteOrder::BIGENDIAN);
    handle->cack.ack_id = uint16from((uint8_t *)&handle->cack.ack_id,ByteOrder::BIGENDIAN);

	if (nbytes != 12) return (GIGE_ERROR_NACK);

	if (handle->cack.ack_id != handle->req_id) return (GIGE_ERROR_NACK);
	
	return 0;
}

//! Read GIGE Register
/*! Read indicated GigE register and return data.
	\param handle Handle for GigE camera as returned from ::gige_open.
    \param address Address of register.
	\return Contents of register as 4 byte unsigned integer.
*/
uint32_t gige_readreg(gige_handle *handle, uint32_t address)
{
	int32_t nbytes, ncount;

    uint16to(0x4201,(uint8_t *)&handle->creg.flag,ByteOrder::BIGENDIAN);
    uint16to(GIGE_CMD_READREG,(uint8_t *)&handle->creg.command,ByteOrder::BIGENDIAN);
    uint16to(0x0004,(uint8_t *)&handle->creg.length,ByteOrder::BIGENDIAN);
    uint16to(++handle->req_id,(uint8_t *)&handle->creg.req_id,ByteOrder::BIGENDIAN);
    uint32to(address,(uint8_t *)&handle->creg.address,ByteOrder::BIGENDIAN);
    if ((nbytes=sendto(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,sizeof(handle->command.caddr))) < 0)
	{
#ifdef COSMOS_WIN_OS
		return(-WSAGetLastError());
#else
		return (-errno);
#endif
	}

	handle->command.addrlen = sizeof(handle->command.caddr);

	ncount = 100;
	do
	{
        nbytes = recvfrom(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,(socklen_t *)&handle->command.addrlen);
	} while (nbytes <= 0 && ncount--);

    handle->cack.ack_id = uint16from((uint8_t *)&handle->cack.ack_id,ByteOrder::BIGENDIAN);
	if (handle->cack.ack_id != handle->req_id) return (GIGE_ERROR_NACK);

    handle->cack.status = uint16from((uint8_t *)&handle->cack.status,ByteOrder::BIGENDIAN);
    handle->cack.acknowledge = uint16from((uint8_t *)&handle->cack.acknowledge,ByteOrder::BIGENDIAN);
    handle->cack.length = uint16from((uint8_t *)&handle->cack.length,ByteOrder::BIGENDIAN);

	if (nbytes != 12) return (GIGE_ERROR_NACK);

    handle->cack.data = uint32from((uint8_t *)&handle->cack.data,ByteOrder::BIGENDIAN);
	
	return (handle->cack.data);
}

//! Read GIGE memory
/*! Read indicated GigE memory and return data.
    \param handle Handle for GigE camera as returned from ::gige_open.
    \param address Address of memory.
    \param size Size of memory area to read.
    \return Contents of register as 4 byte unsigned integer.
*/
uint32_t gige_readmem(gige_handle *handle, uint32_t address, uint32_t size)
{
	int32_t nbytes, ncount;

    uint16to(0x4201,(uint8_t *)&handle->creg.flag,ByteOrder::BIGENDIAN);
    uint16to(GIGE_CMD_READMEM,(uint8_t *)&handle->creg.command,ByteOrder::BIGENDIAN);
    uint16to(0x0008,(uint8_t *)&handle->creg.length,ByteOrder::BIGENDIAN);
    uint16to(++handle->req_id,(uint8_t *)&handle->creg.req_id,ByteOrder::BIGENDIAN);
	address = 4 * (address / 4);
    uint32to(address,(uint8_t *)&handle->creg.address,ByteOrder::BIGENDIAN);
	size = 4 * (size / 4);
    uint32to(size,(uint8_t *)&handle->creg.data,ByteOrder::BIGENDIAN);
    if ((nbytes=sendto(handle->command.cudp,(char *)handle->cbyte,16,0,(struct sockaddr *)&handle->command.caddr,sizeof(struct sockaddr_in))) < 0)
	{
#ifdef COSMOS_WIN_OS
		return(-WSAGetLastError());
#else
		return (-errno);
#endif
	}

	handle->command.addrlen = sizeof(handle->command.caddr);

	ncount = 100;
	do
	{
        nbytes = recvfrom(handle->command.cudp,(char *)handle->cbyte,size+12,0,(struct sockaddr *)&handle->command.caddr,(socklen_t *)&handle->command.addrlen);
	} while (nbytes <= 0 && ncount--);

    handle->cack.ack_id = uint16from((uint8_t *)&handle->cack.ack_id,ByteOrder::BIGENDIAN);
	if (handle->cack.ack_id != handle->req_id) return (GIGE_ERROR_NACK);

    handle->cack.status = uint16from((uint8_t *)&handle->cack.status,ByteOrder::BIGENDIAN);
    handle->cack.acknowledge = uint16from((uint8_t *)&handle->cack.acknowledge,ByteOrder::BIGENDIAN);
    handle->cack.length = uint16from((uint8_t *)&handle->cack.length,ByteOrder::BIGENDIAN);
    handle->cack_mem.address = uint32from((uint8_t *)&handle->cack_mem.address,ByteOrder::BIGENDIAN);

	if ((uint32_t)nbytes != size+12) return (GIGE_ERROR_NACK);

	
	return ((uint32_t)nbytes-12);
}

//! Discover GIGE Camera
/*! Broadcast GIGE DISCOVERY_CMD, accepting all the reponses and returning them
 * in a vector of ::gige_acknowledge_ack.
    \return Vector of ::gige_acknowledge_ack containing responses.
*/
std::vector<gige_acknowledge_ack> gige_discover()
{
	int32_t nbytes;
	std::vector<gige_acknowledge_ack> gige_list;
	socket_channel tchan;
	gige_handle handle;
	int on = 1;
	std::vector<socket_channel> ifaces;

	ifaces = socket_find_addresses(NetworkType::UDP);
	if (!ifaces.size()) return (gige_list);

	for (uint16_t i=0; i<ifaces.size(); ++i)
	{
		if ((socket_open(&tchan, NetworkType::UDP, ifaces[i].baddress, 3956, SOCKET_TALK, true, 100000)) < 0) return (gige_list);

		if ((setsockopt(tchan.cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
		{
			close(tchan.cudp);
			continue;
		}

        uint16to(0x4201,(uint8_t *)&handle.creg.flag,ByteOrder::BIGENDIAN);
        uint16to(GIGE_CMD_DISCOVERY,(uint8_t *)&handle.creg.command,ByteOrder::BIGENDIAN);
        uint16to(0x0000,(uint8_t *)&handle.creg.length,ByteOrder::BIGENDIAN);
        uint16to(++handle.req_id,(uint8_t *)&handle.creg.req_id,ByteOrder::BIGENDIAN);

		if ((nbytes=sendto(tchan.cudp,(char *)handle.cbyte,8,0,(struct sockaddr *)&tchan.caddr,sizeof(tchan.caddr))) < 0)
		{
			close(tchan.cudp);
			continue;
		}

		tchan.addrlen = sizeof(tchan.caddr);

		while((nbytes=recvfrom(tchan.cudp,(char *)handle.cbyte,256,0,(struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
		{
            handle.cack.ack_id = uint16from((uint8_t *)&handle.cack.ack_id,ByteOrder::BIGENDIAN);
			if (handle.cack.ack_id != handle.req_id) continue;

            handle.cack.status = uint16from((uint8_t *)&handle.cack.status,ByteOrder::BIGENDIAN);
            handle.cack.acknowledge = uint16from((uint8_t *)&handle.cack.acknowledge,ByteOrder::BIGENDIAN);
            handle.cack.length = uint16from((uint8_t *)&handle.cack.length,ByteOrder::BIGENDIAN);

            handle.cack_ack.spec_major = uint16from((uint8_t *)&handle.cack_ack.spec_major,ByteOrder::BIGENDIAN);
            handle.cack_ack.spec_minor = uint16from((uint8_t *)&handle.cack_ack.spec_minor,ByteOrder::BIGENDIAN);
            handle.cack_ack.device_mode = uint32from((uint8_t *)&handle.cack_ack.device_mode,ByteOrder::BIGENDIAN);
            handle.cack_ack.mac_high = uint16from((uint8_t *)&handle.cack_ack.mac_high,ByteOrder::BIGENDIAN);
            handle.cack_ack.mac_low = uint32from((uint8_t *)&handle.cack_ack.mac_low,ByteOrder::BIGENDIAN);
            handle.cack_ack.ip_config_options = uint32from((uint8_t *)&handle.cack_ack.ip_config_options,ByteOrder::BIGENDIAN);
            handle.cack_ack.ip_config_current = uint32from((uint8_t *)&handle.cack_ack.ip_config_current,ByteOrder::BIGENDIAN);
            handle.cack_ack.address = uint32from((uint8_t *)&handle.cack_ack.address,ByteOrder::BIGENDIAN);
            handle.cack_ack.subnet = uint32from((uint8_t *)&handle.cack_ack.subnet,ByteOrder::BIGENDIAN);
            handle.cack_ack.gateway = uint32from((uint8_t *)&handle.cack_ack.gateway,ByteOrder::BIGENDIAN);
			gige_list.push_back(handle.cack_ack);
		}

		close(tchan.cudp);
	}

	return (gige_list);
}

//! IP Address to value
/*! Convert a 17 character IP address string to a 4 byte unsigned integer.
 * \param address Dot notation IP address.
 * \return 4 byte unsigned integer in same order.
 */
uint32_t gige_address_to_value(char *address)
{
	union
	{
		uint8_t byte[4];
		uint32_t value;
	} v;

	sscanf(address,"%hhu.%hhu.%hhu.%hhu",&v.byte[3],&v.byte[2],&v.byte[1],&v.byte[0]);

	return (v.value);
}

//! IP Value to address
/*! Convert a 4 byte unsigned integer to a dot notation address string.
 * \param value  4 byte unsigned integer.
 * \return String containing dot notation address.
 */
char *gige_value_to_address(uint32_t value)
{
	static char address[18];
	union
	{
		uint8_t byte[4];
		uint32_t value;
	} v;

	v.value = value;
	sprintf(address,"%u.%u.%u.%u",v.byte[3],v.byte[2],v.byte[1],v.byte[0]);
	return (address);
}

//! Configure a35 camera
/*! Setup the basic image parameters for a a35 camera being used over GIGE.
 * The camera must first be opened with a call to ::gige_open.
 * \param handle Pointer to ::gige_handle returned by ::gige_open.
 * \param xsize Number of pixels in x direction.
 * \param ysize Number of pixels in y direction.
 * \param video_rate 30 or 60 Hz.
 * \return Zero, or negative error.
 */
int a35_config(gige_handle *handle, uint32_t xsize, uint32_t ysize, uint32_t video_rate)
{
	uint32_t maxx, maxy;
	int32_t iretn;

	if((iretn=gige_readreg(handle,A35_WIDTH)) < 0) return (iretn);
	maxx = iretn;
	if((iretn=gige_readreg(handle,A35_HEIGHT)) < 0) return (iretn);
	maxy = iretn;

	if (xsize > (maxx)) xsize = (maxx);
	if (ysize > (maxy)) ysize = (maxy);

	if ((iretn=gige_writereg(handle,A35_WIDTH,xsize)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,A35_HEIGHT,ysize)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,0xE984, 3)) < 0) return (iretn);            // Set to 14 bit mode
	if ((iretn=gige_writereg(handle,A35_PIXELFORMAT, A35_PIXELFORMAT_14BIT)) < 0) return (iretn);            // Set to 14 bit mode
	if ((iretn=gige_writereg(handle,A35_CMOSBITDEPTH,3)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,A35_SENSORVIDEOSTANDARD,video_rate)) < 0) return (iretn);
//	if ((iretn=gige_writereg(handle,A35_IMAGEADJUST,A35_IMAGEADJUST_MANUAL)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,A35_IMAGEADJUST,A35_IMAGEADJUST_AUTOBRIGHT)) < 0) return (iretn);

	// Set shutter to manual
//	gige_writereg(handle, A35_FFCMODE, A35_FFCMODE_EXTERNAL); // Set FFC to manual
	gige_writereg(handle, A35_FFCMODE, A35_FFCMODE_MANUAL); // Set FFC to manual

	return 0;
}

//! Take A35 image stream.
/*! Command A35 camera being used over GIGE to take a stream of images of the indicated
 * exposure length. The resulting image will be stored in the provided image buffer.
 * \param handle Pointer to ::gige_handle returned by ::gige_open.
 * \param frames Number of images to store.
 * \param buffer Pointer to buffer for storing image.
 * \param bsize Number of bytes to expect at a go.
 * \return Zero, or negative error.
 */
int a35_image(gige_handle *handle, uint32_t frames, uint8_t *buffer, uint16_t bsize)
{
	int32_t iretn, nbytes;
	uint32_t tbytes, pbytes;
	uint8_t *bufferin;
	double mjd;

	bufferin = (uint8_t *)malloc(bsize);
	if (bufferin == NULL)
		return (-errno);

	gige_writereg(handle,GIGE_REG_SCDA, gige_address_to_value(handle->stream.address));

	iretn = gige_writereg(handle,GIGE_REG_SCP,handle->stream.cport);
	if ((iretn=gige_writereg(handle,GIGE_REG_SCPS,bsize)) < 0)
		return (iretn);
	if ((iretn=gige_writereg(handle,A35_ACQUISITIONSTART,1)) < 0)
		return (iretn);
	pbytes = gige_readreg(handle,A35_WIDTH) * gige_readreg(handle,A35_HEIGHT) * frames * 2;

	tbytes = 0;
	uint32_t elapsed=0;
	uint32_t telapsed=500000 + 2 * 1e6 * pbytes / handle->streambps;
	mjd = currentmjd(0.);
	while (tbytes < pbytes && elapsed<telapsed)
	{
		if ((nbytes=recvfrom(handle->stream.cudp,(char *)bufferin,bsize,0,(struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
		{
			switch (bufferin[4])
			{
			case 1:
				break;
			case 2:
				break;
			case 3:
				memcpy(&buffer[tbytes], &bufferin[8], nbytes-8);
				tbytes += nbytes-8;
				break;
			}
		}
		elapsed = (uint32_t)(1e6*86400.*(currentmjd(0.)-mjd)+.5);
//		iretn = gige_readreg(handle,GIGE_REG_CCP);
	}

//	sdt2 = sqrt((sdt2 - sdt*sdt/count)/(count-1));
//	sdt /= count;
	iretn = gige_writereg(handle,A35_ACQUISITIONSTOP,1);
	iretn = gige_writereg(handle,GIGE_REG_SCP,0);
	free(bufferin);
	return (tbytes);

}

//! Configure Prosilica camera
/*! Setup the basic image parameters for a Prosilica camera being used over GIGE.
 * The camera must first be opened with a call to ::gige_open.
 * \param handle Pointer to ::gige_handle returned by ::gige_open.
 * \param format Pixel format for output as defined in \ref gige_prosilica_constants.
 * \param xbin Factor for binning in x direction.
 * \param ybin Factor for binning in y direction.
 * \param xsize Number of pixels in x direction.
 * \param ysize Number of pixels in y direction.
 * \param xoffset Starting pixel of sub-image in x direction.
 * \param yoffset Starting pixel of sub-image in y direction.
 * \return Zero, or negative error.
 */
int prosilica_config(gige_handle *handle, uint32_t format, uint32_t xbin, uint32_t ybin, uint32_t xsize, uint32_t ysize, uint32_t xoffset, uint32_t yoffset)
{
	uint32_t maxx, maxy, maxbx, maxby;
	int32_t iretn;

	if((iretn=gige_readreg(handle,PROSILICA_SensorWidth)) < 0) return (iretn);
	maxx = iretn;
	if((iretn=gige_readreg(handle,PROSILICA_SensorHeight)) < 0) return (iretn);
	maxy = iretn;
	if((iretn=gige_readreg(handle,PROSILICA_BinningXMax)) < 0) return (iretn);
	maxbx = iretn;
	if((iretn=gige_readreg(handle,PROSILICA_BinningYMax)) < 0) return (iretn);
	maxby = iretn;

	if (xbin > 1 || ybin > 1)
	{
		xoffset = yoffset = 0;
		if (xbin > maxbx) xbin = maxbx;
		if (ybin > maxby) ybin = maxby;
	}

	if (xsize > (maxx/xbin)-xoffset) xsize = (maxx/xbin)-xoffset;
	if (ysize > (maxy/xbin)-yoffset) ysize = (maxy/ybin)-yoffset;

	if ((iretn=gige_writereg(handle,PROSILICA_BinningXValue,xbin-1)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_BinningYValue,ybin-1)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_RegionX,xoffset)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_RegionY,yoffset)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_Width,xsize)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_Height,ysize)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_PixelFormat,format)) < 0) return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_StreamBytesPerSec,handle->streambps)) < 0) return (iretn);

	return 0;
}

//! Take Prosilica image.
/*! Command Prosilica camera being used over GIGE to take a single image of the indicated
 * exposure length. The resulting image will be stored in the provided image buffer.
 * \param handle Pointer to ::gige_handle returned by ::gige_open.
 * \param emode One of ::PROSILICA_ExposureMode_AutoOff, ::PROSILICA_ExposureMode_AutoOnce, ::PROSILICA_ExposureMode_Auto.
 * \param exposure Exposure time in usec.
 * \param gain DN mutiplicative value.
 * \param buffer Pointer to buffer for storing image.
 * \param bsize Maximum size of buffer.
 * \return Zero, or negative error.
 */
int prosilica_image(gige_handle *handle, uint16_t emode, uint32_t exposure, uint32_t gain, uint8_t *buffer, uint16_t bsize)
{
	int32_t iretn, nbytes;
	uint32_t tbytes, pbytes;
	uint8_t *bufferin;
	double mjd;

	bufferin = (uint8_t *)malloc(bsize);
	if (bufferin == NULL)
		return (-errno);

	iretn = gige_writereg(handle,GIGE_REG_SCP,handle->stream.cport);
	if ((iretn=gige_writereg(handle,GIGE_REG_SCPS,bsize)) < 0)
		return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_ExposureMode,emode)) < 0)
		return (iretn);
	if (emode != PROSILICA_ExposureMode_AutoOff)
	{
		if((iretn=gige_readreg(handle,PROSILICA_ExposureValue)) < 0)
			return (iretn);
		exposure = iretn;
	}
	else
	{
		if ((iretn=gige_writereg(handle,PROSILICA_ExposureValue,exposure)) < 0)
			return (iretn);
	}
	if ((iretn=gige_writereg(handle,PROSILICA_GainValue,gain)) < 0)
		return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_AcquisitionMode,PROSILICA_AcquisitionMode_SingleFrame)) < 0)
		return (iretn);
	if ((iretn=gige_writereg(handle,PROSILICA_AcquisitionCommand,PROSILICA_AcquisitionCommand_Start)) < 0)
		return (iretn);
	if ((iretn = gige_readreg(handle,PROSILICA_PayloadSize)) < 0)
		return (iretn);
	pbytes = iretn;

//	COSMOS_USLEEP(exposure);
//	COSMOS_USLEEP(520000);
	tbytes = 0;
	uint32_t elapsed=0;
	uint32_t telapsed=2*(exposure+1e6*pbytes/handle->streambps);
	mjd = currentmjd(0.);
	while (tbytes < pbytes && elapsed<telapsed)
	{
		if ((nbytes=recvfrom(handle->stream.cudp,(char *)bufferin,bsize,0,(struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
		{
			switch (bufferin[4])
			{
			case 1:
				break;
			case 2:
				break;
			case 3:
				memcpy(&buffer[tbytes], &bufferin[8], nbytes-8);
				tbytes += nbytes-8;
				break;
			}
		}
		elapsed = (uint32_t)(1e6*86400.*(currentmjd(0.)-mjd)+.5);
//		iretn = gige_readreg(handle,GIGE_REG_CCP);
	}

//	sdt2 = sqrt((sdt2 - sdt*sdt/count)/(count-1));
//	sdt /= count;
	iretn = gige_writereg(handle,PROSILICA_AcquisitionCommand,PROSILICA_AcquisitionCommand_Stop);
	iretn = gige_writereg(handle,GIGE_REG_SCP,0);
	free(bufferin);
	return (tbytes);

}



// ***************************************************
// *********************  A35  ***********************
// ***************************************************


//! Read GIGE Register for A35 with different flag
/*! Read indicated GigE register and return data.
    \param handle Handle for GigE camera as returned from ::gige_open.
    \param address Address of register.
    \return Contents of register as 4 byte unsigned integer.
*/
uint32_t gige_readreg2(gige_handle *handle, uint32_t address)
{
    int32_t nbytes, ncount;

    uint16to(0x4200,(uint8_t *)&handle->creg.flag,ByteOrder::BIGENDIAN);
    uint16to(GIGE_CMD_READREG,(uint8_t *)&handle->creg.command,ByteOrder::BIGENDIAN);
    uint16to(0x0004,(uint8_t *)&handle->creg.length,ByteOrder::BIGENDIAN);
    uint16to(0x0001,(uint8_t *)&handle->creg.req_id,ByteOrder::BIGENDIAN);
    uint32to(address,(uint8_t *)&handle->creg.address,ByteOrder::BIGENDIAN);
    if ((nbytes=sendto(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,sizeof(handle->command.caddr))) < 0)
    {
#ifdef COSMOS_WIN_OS
        return(-WSAGetLastError());
#else
        return (-errno);
#endif
    }

    handle->command.addrlen = sizeof(handle->command.caddr);

    ncount = 100;
    do
    {
        nbytes = recvfrom(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,(socklen_t *)&handle->command.addrlen);
    } while (nbytes <= 0 && ncount--);

    handle->cack.ack_id = uint16from((uint8_t *)&handle->cack.ack_id,ByteOrder::BIGENDIAN);
    if (handle->cack.ack_id != handle->req_id) return (GIGE_ERROR_NACK);

    handle->cack.status = uint16from((uint8_t *)&handle->cack.status,ByteOrder::BIGENDIAN);
    handle->cack.acknowledge = uint16from((uint8_t *)&handle->cack.acknowledge,ByteOrder::BIGENDIAN);
    handle->cack.length = uint16from((uint8_t *)&handle->cack.length,ByteOrder::BIGENDIAN);

    if (nbytes != 12) return (GIGE_ERROR_NACK);

    handle->cack.data = uint32from((uint8_t *)&handle->cack.data,ByteOrder::BIGENDIAN);

    return (handle->cack.data);
}


//! Send A35 discover message?
/*! Read indicated GigE register and return data.
    \param handle Handle for GigE camera as returned from ::gige_open.
    \param address Address of register.
    \return Contents of register as 4 byte unsigned integer.
*/
uint32_t gige_request(gige_handle *handle, uint32_t address)
{
    int32_t nbytes, ncount;

    uint16to(0x4200,(uint8_t *)&handle->creg.flag,ByteOrder::BIGENDIAN);
    uint16to(GIGE_CMD_DISCOVERY,(uint8_t *)&handle->creg.command,ByteOrder::BIGENDIAN);
    uint16to(0x0004,(uint8_t *)&handle->creg.length,ByteOrder::BIGENDIAN);
    uint16to(0xC000,(uint8_t *)&handle->creg.req_id,ByteOrder::BIGENDIAN);
    uint32to(address,(uint8_t *)&handle->creg.address,ByteOrder::BIGENDIAN);
    if ((nbytes=sendto(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,sizeof(handle->command.caddr))) < 0)
    {
#ifdef COSMOS_WIN_OS
        return(-WSAGetLastError());
#else
        return (-errno);
#endif
    }

    handle->command.addrlen = sizeof(handle->command.caddr);

    ncount = 100;
    do
    {
        nbytes = recvfrom(handle->command.cudp,(char *)handle->cbyte,12,0,(struct sockaddr *)&handle->command.caddr,(socklen_t *)&handle->command.addrlen);
    } while (nbytes <= 0 && ncount--);

    handle->cack.ack_id = uint16from((uint8_t *)&handle->cack.ack_id,ByteOrder::BIGENDIAN);
    if (handle->cack.ack_id != handle->req_id) return (GIGE_ERROR_NACK);

    handle->cack.status = uint16from((uint8_t *)&handle->cack.status,ByteOrder::BIGENDIAN);
    handle->cack.acknowledge = uint16from((uint8_t *)&handle->cack.acknowledge,ByteOrder::BIGENDIAN);
    handle->cack.length = uint16from((uint8_t *)&handle->cack.length,ByteOrder::BIGENDIAN);

    if (nbytes != 12) return (GIGE_ERROR_NACK);

    handle->cack.data = uint32from((uint8_t *)&handle->cack.data,ByteOrder::BIGENDIAN);

    return (handle->cack.data);
}


//! @}
