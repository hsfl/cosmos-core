/*! \file vmt35_lib.c
	\brief Interface routines for VMT-35 Torque Rod Controller
*/

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#ifdef __CYGWIN__
#define __USE_LINUX_IOCTL_DEFS
#endif

#include "vmt35_lib.h"
#include "mathlib.h"
#include "timelib.h"
//#include <unistd.h>
//#include <fcntl.h>

//!	\ingroup vmt35
//! \defgroup vmt35_functions VMT-35 Magnetic Torque Rod functions
//! @{

//! Connect to Torque Rod Controller
/*! Connect to Torque Rod Controller
	\param devname pointer to a character string with the serial port it is connected to.
	\return Handle to use, otherwise negative error number.
	\see cssl_start
	\see cssl_open
	\see cssl_setflowcontrol
*/
int vmt35_connect(char *devname, vmt35_handle *handle)
{
	int32_t iretn;

	if (handle->serial != NULL)
		return(0);

	cssl_start();
	handle->serial = cssl_open(devname,VMT35_BAUD,VMT35_BITS,VMT35_PARITY,VMT35_STOPBITS);
	if (handle->serial == NULL)
		return (-errno);

	if ((iretn=cssl_settimeout(handle->serial, 0, .1)) < 0)
	{
		return (iretn);
	}
	if ((iretn=cssl_setflowcontrol(handle->serial, 0, 0)) < 0)
	{
		return (iretn);
	}

	if ((iretn=vmt35_get_telemetry(handle)))
		return(iretn);

	return 0;
}

//! Close Torque Rod Controller
/*! Close currently open TS-2000.
*/
int vmt35_disconnect(vmt35_handle *handle)
{
	if (handle->serial == NULL)
		return (VMT35_ERROR_CLOSED);

	cssl_close(handle->serial);
	return 0;
}

int vmt35_putbyte(vmt35_handle *handle, uint8_t byte)
{
	cssl_putchar(handle->serial,byte);
	return 0;
}

//! Software and Hardware Reset
/*! Send command byte 0x00 to perform full reset.
	\return 0, otherwise negative error.
*/
int vmt35_reset(vmt35_handle *handle)
{
	uint8_t buf[8];

	// Command Byte
	buf[0] = 0x00;
	// Data
	buf[1] = 0xab;
	buf[2] = 0xcd;
	buf[3] = 0xef;
	// CRC
	buf[4] = 0x98;
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[0] = cssl_getchar(handle->serial);
	if (buf[0] != 0x00)
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Set Torquer Supply Voltage
/*! Send command byte 0xa0 and appropriate data to set DAC level of supply voltage.
	\param channel 0-2
	\param voltage 12-bit (0-4095) DAC value of voltage
	\return 0, otherwise negative error.
*/
int vmt35_set_voltage(vmt35_handle *handle, uint8_t channel, uint16_t voltage)
{
	uint8_t buf[8];

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	// Command Byte
	buf[0] = 0xa0;
	// Data
	buf[1] = channel;
	uint16to(voltage,&buf[2],ORDER_BIGENDIAN);
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[5] != buf[0])
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Set Torquer Supply Voltage in Percentage
/*! Send command to selected torque rod
	\param channel 0-2
	\param voltage 0-100% value of voltage
*/
int32_t vmt35_set_percent_voltage(vmt35_handle *handle, int16_t ch, int16_t percentage)
{
	int32_t iretn;
	int volts = 0;
	// voltage max: 4095
	volts = percentage/100*4095;
	iretn = vmt35_set_voltage(handle, ch, volts);
	return (iretn);
}

//! Set Torquer Supply Current
/*! Send command byte 0xa1 and appropriate data to set DAC level of supply current.
	\param channel 0-2
	\param current 12-bit (0-4095) DAC value of current
	\return 0, otherwise negative error.
*/
int vmt35_set_current_dac(vmt35_handle *handle, uint8_t channel, uint16_t current)
{
	uint8_t buf[8];

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	// Command Byte
	buf[0] = 0xa1;
	// Data
	buf[1] = channel;
	uint16to(current,&buf[2],ORDER_BIGENDIAN);
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[5] != buf[0])
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Enable Torquer Supply Voltage
/*! Senc command byte 0xa2 to switch on the supply voltage of all channels.
	\return 0, otherwise negative error.
*/
int vmt35_enable(vmt35_handle *handle)
{
	uint8_t buf[8];

	// Command Byte
	buf[0] = 0xa2;
	// Data
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[0] != buf[5])
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Disable Torquer Supply Voltage
/*! Senc command byte 0xa3 to switch off the supply voltage of all channels.
	\return 0, otherwise negative error.
*/
int vmt35_disable(vmt35_handle *handle)
{
	uint8_t buf[8];

	// Command Byte
	buf[0] = 0xa3;
	// Data
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[0] != buf[5])
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Reverse Torquer Current
/*! Senc command byte 0xa4 to reverse the current of the requested channel.
	\param channel 0-2
	\return 0, otherwise negative error.
*/
int vmt35_reverse(vmt35_handle *handle, uint8_t channel)
{
	uint8_t buf[8];

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	// Command Byte
	buf[0] = 0xa4;
	// Data
	buf[1] = channel;
	buf[2] = 0x00;
	buf[3] = 0x00;
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[0] != buf[5])
		return (VMT35_ERROR_NACK);

	return 0;
}

//! Get Telemetry information
/*! Access the telemetry information of the TCU
	\return VMT-35 telmetry information structure
*/
int vmt35_get_telemetry(vmt35_handle *handle)
{
	uint8_t obuf[5], ibuf[32], crc;
	int32_t iretn;

	obuf[0] = 0xd1;
	obuf[1] = 0x00;
	obuf[2] = 0x00;
	obuf[3] = 0x00;
	obuf[4] = vmt35_crc(obuf,4);

	for (uint16_t i=0; i<3; ++i)
	{
		cssl_putdata(handle->serial,obuf,5);

		// Get command byte back
		if ((iretn=cssl_getchar(handle->serial)) == 0xd1)
			break;
		if (iretn >= 0)
			iretn = VMT35_ERROR_NACK;
	}
	if (iretn < 0)
		return (iretn);

	for (uint16_t i=0; i<32; ++i)
	{
		if ((iretn=cssl_getchar(handle->serial)) < 0)
			return (CSSL_ERROR_READ);
		ibuf[i] = iretn;
	}

	crc = vmt35_crc(ibuf,29);
	if (crc != ibuf[29])
		return (VMT35_ERROR_CRC);

	// Get status
	handle->telem.status = uint16from(&ibuf[0],ORDER_BIGENDIAN);
	handle->telem.count = uint16from(&ibuf[2],ORDER_BIGENDIAN);
	handle->telem.invalidcount = uint16from(&ibuf[4],ORDER_BIGENDIAN);
	handle->telem.dac[0] = int32from(&ibuf[6],ORDER_BIGENDIAN);
	handle->telem.dac[1] = int32from(&ibuf[10],ORDER_BIGENDIAN);
	handle->telem.dac[2] = int32from(&ibuf[14],ORDER_BIGENDIAN);
	handle->telem.temp = uint16from(&ibuf[18],ORDER_BIGENDIAN);
	handle->telem.voltage = uint16from(&ibuf[20],ORDER_BIGENDIAN);
	handle->telem.resetcount = ibuf[28];
	handle->telem.crc = ibuf[29];

	return 0;
}

//! Read Torquer Supply Voltage
/*! Send command byte 0xa5 to return the 12-bit (0-4095) DAC value of the supply
	voltage on the requested channel.
	\param channel 0-2
	\param voltage DAC value of the supply voltage, 12-bit unsigned
	\return 0, otherwise negative error.
*/
int vmt35_get_voltage(vmt35_handle *handle, uint8_t channel, uint16_t *voltage)
{
	uint8_t buf[8];

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	// Command Byte
	buf[0] = 0xa5;
	// Data
	buf[1] = channel;
	buf[2] = 0x00;
	buf[3] = 0x00;
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[0] != buf[5])
		return (VMT35_ERROR_NACK);

	// Get returned data
	buf[0] = cssl_getchar(handle->serial);
	buf[1] = cssl_getchar(handle->serial);
	*voltage = uint16from(buf,ORDER_BIGENDIAN);

	return 0;
}

//! Read Torquer Supply Current
/*! Send command byte 0xa5 to return the 12-bit (0-4095) DAC value of the supply
	current on the requested channel.
	\param channel 0-2
	\param current DAC value of the supply current, 12-bit unsigned.
	\return 0, otherwise negative error.
*/
//THIS FUNCTION LOOKS DEPRECIATED AND DOESN'T SEEM TO RETURN THE CORRECT VALUE FOR THE CURRENT ANYMORE. - Erik Wessel
int vmt35_get_current(vmt35_handle *handle, uint8_t channel, int16_t *current)
{
	uint8_t buf[8];

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	// Command Byte
	buf[0] = 0xa6;
	// Data
	buf[1] = channel;
	buf[2] = 0x00;
	buf[3] = 0x00;
	// CRC
	buf[4] = vmt35_crc(buf,4);
	// Send bytes to VMT-35
	cssl_putdata(handle->serial,buf,5);
	// Check for echoed command byte
	buf[5] = cssl_getchar(handle->serial);
	if (buf[0] != buf[5])
		return (VMT35_ERROR_NACK);

	// Get returned data
	buf[0] = cssl_getchar(handle->serial);
	buf[1] = cssl_getchar(handle->serial);
	*current = int16from(buf,ORDER_BIGENDIAN);
	if (*current < 0)
		*current = -*current - 32768;

	return 0;
}

//! Set Torque Rod current
/*! Set the current for the requested torque rod
	\param channel Integer 0, 1 or 2
	\param current Current in Amps, -.1 to +.1
	\return 0 or negative error
*/
int32_t vmt35_set_amps(vmt35_handle *handle, uint8_t channel,float current)
{
	int32_t dac, iretn;
	uint8_t buf[8];

	//current=-current;

	if (channel > 2)
		return (VMT35_ERROR_RODNUM);

	if (current < -0.1 || current > 0.1)
		return (VMT35_ERROR_OUTOFRANGE);


	// Data
	dac = (int32_t)(current*1e6);
	int32to(dac,buf,ORDER_BIGENDIAN);
	// Command Byte
	buf[0] = 0xc0 + channel;
	// CRC
	buf[4] = vmt35_crc(buf,4);
    // Send bytes to VMT-35, try three times if fail
	for (uint16_t i=0; i<3; ++i)
	{
		cssl_putdata(handle->serial,buf,5);

		// Check for echoed command byte
		iretn = cssl_getchar(handle->serial);
		if (iretn < 0)
        {// continue trying two more times
			continue;
		}
		if (buf[0] == iretn)
		{
			break;
		}
		iretn = VMT35_ERROR_NACK;
	}

	if (iretn < 0)
		return (iretn);

	return 0;
}

//! Set VMT35 Magnetic Moment
/*! Calculate the current to use for the indicated torque rod using the requested
 * magnetic moment and the supplied polynomial coefficients. Wait for the current
 * to stabilize at the new value.
 * \param handle Pointer to ::vmt35_handle.
 * \param channel Index of torque rod.
 * \param mom Magnetic moment.
 * \param npoly Coefficients for 6th order polynomial to be used with negative moments.
 * \param ppoly Coefficients for 6th order polynomial to be used with positive moments.
 * \return Zero or negative error.
 */
int32_t vmt35_set_moment(vmt35_handle *handle, uint16_t channel, double mom, float npoly[7], float ppoly[7])
{
	int32_t iretn;
    double amp, setamp;

	if (mom == 0.)
	{
		amp = 0.;
	}
	else
	{
		amp = vmt35_calc_amp(mom, npoly, ppoly);
    }
    double cmjd=currentmjd(0.);
    setamp = amp+10; //just in case vmt35_get_telemetry fails on the first try, so setamp still gets initialized
	do
	{
		if ((iretn=vmt35_set_amps(handle, channel, amp)) < 0)
		{
			continue;
		}
		if ((iretn=vmt35_get_telemetry(handle)) < 0)
		{
			continue;
        }
		switch (channel)
		{
		case 0:
			{
				setamp = handle->telem.dac[0]/1e6;
				break;
			}
		case 1:
			{
				setamp = handle->telem.dac[1]/1e6;
				break;
			}
		case 2:
			{
				setamp = handle->telem.dac[2]/1e6;
				break;
			}
		}
    } while ((currentmjd(0.)-cmjd) < 1./86400. && fabs(amp-setamp) > .0005);
    return 0;

}

//! Set all three VMT35 Magnetic Moment
/*! Calculate the current to use for each torque rod using the requested ::rvector of
 * magnetic moments and the supplied polynomial coefficients. Wait for the current
 * to stabilize at the new value.
 * \param handle Pointer to ::vmt35_handle.
 * \param mom Magnetic moments.
 * \param npoly Coefficients for 6th order polynomial to be used with negative moments.
 * \param ppoly Coefficients for 6th order polynomial to be used with positive moments.
 * \return Zero or negative error.
 */
int32_t vmt35_set_moments(vmt35_handle *handle, rvector mom, float npoly[3][7], float ppoly[3][7])
{
	double amp[3], setamp;
    // limit the moment to a maximum of 32 Am^2
	for (uint16_t i =0; i<3; ++i)
    {
		if (fabs(mom.col[i]) > 32.)
		{
			double decrease = 32. / fabs(mom.col[i]);
			for (uint16_t j=0; j<3; ++j)
			{
				mom.col[j] *= decrease;
			}
		}
	}
    //compute the current that produces the desired torque, for the 3 torque rods
	for (uint16_t i =0; i<3; ++i)
	{
		if (mom.col[i] == 0.)
		{
			amp[i] = 0.;
		}
		else
		{
			amp[i] = vmt35_calc_amp(mom.col[i], npoly[i], ppoly[i]);
		}
	}

	double cmjd=currentmjd(0.);
	for (uint16_t i=0; i<3; ++i)
	{
		handle->telem.dac[i] = 10000;
	}
//?? please explain
	do
	{
		for (uint16_t i=0; i<3; ++i)
		{
			if (fabs(amp[i] - handle->telem.dac[i]/1e6) > .0005)
			{
				vmt35_set_amps(handle, i, amp[i]);
			}
		}
		vmt35_get_telemetry(handle);
		setamp = 0.;
		for (uint16_t i=0; i<3; ++i)
		{
			setamp += fabs(amp[i] - handle->telem.dac[i]/1e6);
		}
	} while ((currentmjd(0.)-cmjd) < 1./86400. && setamp > .0005);

	return ((int32_t)(setamp*1e3));


}

//! Set Torque Rod current
/*! Set the current for the requested torque rod
	\param rod Integer 0, 1 or 2
	\param current Current in Amps
	\return 0 or negative error
*/
int vmt35_set_current(vmt35_handle *handle, uint8_t rod, float current)
{
	int16_t dac;
	uint8_t buf[8];

	if (rod > 2)
		return (VMT35_ERROR_RODNUM);

	if (current >= -.20 && current <= .20)
	{
		cssl_putchar(handle->serial,0xda+rod);
		dac = (uint16_t)(4095 * current / .18);
		int16to(dac,buf,ORDER_BIGENDIAN);
		cssl_putchar(handle->serial,buf[0]);
		cssl_putchar(handle->serial,buf[1]);
		buf[0] = cssl_getchar(handle->serial);
		if (buf[0] != 0xda+rod)
			return (VMT35_ERROR_BYTE);
		return 0;
	}
	return (VMT35_ERROR_OUTOFRANGE);
}

//! Calculate VMT35 CRC
/*! Calculate the CRC for the provided VMT35 string
	\param string Data string to calculate CRC for
	\param length Number of bytes
	\return One byte CRC
*/
uint8_t vmt35_crc(uint8_t *string, uint16_t length)
{
	uint8_t crc;
	int i;

	crc = 0;
	for (i=0; i<length; i++)
		crc += string[i];

	crc = 0xff - crc;
	return (crc);
}

//! Calculate VMT35 amperage
/*! Calculate the amperage required for the specific torque rod given the
 * desired Moment and the coefficients for a 6th order polynomial fit.
 * \param mom The desired magnetic mooment
 * \param npoly The negative polynomial coefficients
 * \param ppoly The positive polynomial coefficients
 * \return The calculated amperage to be used.
 */
double vmt35_calc_amp(double mom, float npoly[6], float ppoly[6])
{
	double amp;

	if (mom == 0.)
	{
		amp = 0.;
	}
	else
	{
		if (mom < 0.)
		{
			amp = npoly[0] + mom * (npoly[1] + mom * (npoly[2] + mom * (npoly[3] + mom * (npoly[4] + mom * (npoly[5] + mom * npoly[6])))));
		}
		else
		{
			amp = ppoly[0] + mom * (ppoly[1] + mom * (ppoly[2] + mom * (ppoly[3] + mom * (ppoly[4] + mom * (ppoly[5] + mom * ppoly[6])))));
		}
	}

	if (amp > .0999)
	{
		amp = .0999;
	}
	if (amp < -.0999)
	{
		amp = -.0999;
	}

	return (amp);
}

//! Calculate VMT35 moment
/*! Calculate the moment generated for the specific torque rod given the
 * desired amperage and the coefficients for a 6th order polynomial fit.
 * \param amp The desired magnetic mooment
 * \param npoly The negative polynomial coefficients
 * \param ppoly The positive polynomial coefficients
 * \return The calculated moment to be generated.
 */
double vmt35_calc_moment(double amp, float npoly[6], float ppoly[6])
{
	double mom=0.;
	double slope;
	double namp=0.;

	if (fabs(amp) < .0001)
	{
		return (0.);
	}

	if (amp > .0999)
	{
		amp = .0999;
	}
	if (amp < -.0999)
	{
		amp = -.0999;
	}

	uint16_t count=0;
	do
	{
		if (amp > 0.)
		{
			slope = 1./(ppoly[1] + mom * (2. * ppoly[2] + mom * (3. * ppoly[3] + mom * (4. * ppoly[4] + mom * (5. * ppoly[5] + mom * 6. * ppoly[6])))));
		}
		else
		{
			slope = 1./(npoly[1] + mom * (2. * npoly[2] + mom * (3. * npoly[3] + mom * (4. * npoly[4] + mom * (5. * npoly[5] + mom * 6. * npoly[6])))));
		}
		mom += slope * (amp - namp);
		namp = vmt35_calc_amp(mom, npoly, ppoly);
		//		fprintf(stderr, "%f %f %f\n",amp,namp,mom);
	} while(++count < 100 && fabs(namp-amp) > .0001);

	return (mom);

}

//! @}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
