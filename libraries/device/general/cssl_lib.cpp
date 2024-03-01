/**
 * @file cssl_lib.cpp
 * @brief Simple Serial library source file
 * 
 * @todo This is not HSFL code. Verify the appropriate attributions are 
 * included.
 * 
 * Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details
 * Columbo Simple Serial Library is an easy to use,
 * event driven serial port communication library for Linux.
 * Project website: http://sourceforge.net/projects/cssl/
 */

#include "support/configCosmos.h"
/** @todo Move to .h file */
#include <stdio.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "device/general/cssl_lib.h"

/* Static variables and constants */

/**
 * @brief boolean that say if we have started cssl
 * 
 * @todo Move to .h file
 */
static int cssl_started=0;

/**
 * @brief head of the cssl_t list
 * 
 * @todo Move to .h file
 */
static cssl_t *head=0;

/**
 * @brief error messages table
 * 
 * @todo Move to .h file
 */
static const char *cssl_errors[]= {
	"cssl: OK",
	"cssl: there's no free signal",
	"cssl: not started",
	"cssl: null pointer",
	"cssl: oops",
	"cssl: out of memory",
	"cssl: cannot open file"
};

/**
 * @brief status of last cssl function
 * 
 * @todo Move to .h file
 */
static cssl_error_t cssl_error=CSSL_OK;

/**
 * @addtogroup cssl_lib_functions 
 * @{
 */

/**
 * @brief gets the last operation status message
 * 
 * get the error message
 * 
 * @return const char* 
 * 
 * @todo Document this.
 */
const char *cssl_geterrormsg()
{
	return cssl_errors[cssl_error];
}

/**
 * @brief gets the last error code
 * 
 * get the error code
 * 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_geterror()
{
	return cssl_error;
}

/**
 * @brief start the cssl
 * 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_start()
{
	if (cssl_started) {
		return 0;
	}

	/* OK, the cssl is started */
	cssl_started=1;
	cssl_error=CSSL_OK;
	return 0;
}

/**
 * @brief stops the cssl
 * 
 * finish all jobs, clear memory, etc.
 * 
 * @return int32_t 
 */
int32_t cssl_stop()
{
	/* if not started we do nothing */
	if (!cssl_started)
		return 0;

	/* we close all ports, and free the list */
	while (head)
		cssl_close(head);

	/* And at least : */
	cssl_started=0;
	cssl_error=CSSL_OK;
	return 0;
}

/**
 * @brief opens the port
 * 
 * alloc new cssl_t struct and open the port
 * 
 * @param fname pathname of port file, for example "/dev/ttyS0"
 * @param baud baudrate, integer, for example 19200
 * @param bits data bits: 7 or 8
 * @param parity parity: 0 - none, 1-odd, 2-even
 * @param stop stop bits: 1 or 2
 * @return cssl_t* 
 * 
 * @todo Document this.
 */
cssl_t *cssl_open(const char *fname, int baud, int bits, int parity, int stop)
{
	cssl_t *serial;

	if (!cssl_started) {
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return NULL;
	}

	/* create new cssl_t structure */
	serial=(cssl_t*)calloc(1,sizeof(cssl_t));

	/* oops, no memory */
	if (!serial) {
		cssl_error=CSSL_ERROR_MEMORY;
		return 0;
	}

	/* opening the file */
	/* the read/write operations will be blocking */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS)
    serial->fd=open(fname, O_RDWR | O_NOCTTY);
#endif

#if  defined(COSMOS_MAC_OS)
    serial->fd=open(fname, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

#if defined(COSMOS_WIN_OS)
    serial->handle = CreateFileA(fname,
                                 GENERIC_READ|GENERIC_WRITE,
                                 0,                 /* no share  */
                                 NULL,              /* no security */
                                 OPEN_EXISTING,
                                 0,  /* no threads */ // FILE_ATTRIBUTE_NORMAL
                                 NULL);                 /* no templates */

    if(serial->handle==INVALID_HANDLE_VALUE)
    {
      std::cout << "unable to open serial port" << std::endl;
      exit(0);
    }

	serial->fd = _open_osfhandle((intptr_t)serial->handle, 0);
#endif

	/* oops, cannot open */
	if (serial->fd == -1) {
		cssl_error=CSSL_ERROR_OPEN;
		free(serial);
		return NULL;
	}

	/* we remember old termios */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	tcgetattr(serial->fd,&(serial->oldtio));
#else // windows
    // does not seem to be necessary
    // GetCommState(serial->handle, &(serial->olddcb));
#endif

	/* now we set new values */
    // TODO: what a bit ERROR, parity and bits is flipped
    //cssl_setup(serial,baud,parity,bits,stop);

    // new! TODO: check if all functions that call cssl_open work
    cssl_setup(serial,baud,bits,parity,stop);

	// Default to no flow control
	cssl_setflowcontrol(serial, 0, 0);

	// Set minimal timeout so we don't give up too soon
	cssl_settimeout(serial, 0, .1);

	/* we add the serial to our list */
	serial->next=head;
	head=serial;
	
	cssl_error=CSSL_OK;

	return serial;
}

/**
 * @brief closes the port, and frees its cssl_t struct
 * 
 * closes file, removes serial from the list and frees it
 * 
 * @param serial 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_close(cssl_t *serial)
{
	cssl_t *cur;

	if (!cssl_started) {
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial) {
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}

	/* first we flush the port */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	tcflush(serial->fd,TCOFLUSH);
	tcflush(serial->fd,TCIFLUSH);
#endif

	/* then we restore old settings */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	tcsetattr(serial->fd,TCSANOW,&(serial->oldtio));
#else
	SetCommState(serial->handle, &(serial->dcb));
#endif

	/* and close the file */
	close(serial->fd);

	/* now we can remove the serial from the list */

	if (head==serial) {
		head=serial->next;
		free(serial);
		cssl_error=CSSL_OK;
		return 0;
	}

	for (cur=head;cur;cur=cur->next) {
		if (cur->next==serial) {
			cur->next=serial->next;
			free(serial);
			cssl_error=CSSL_OK;
			return 0;
		}
	}

	/* we should never reach there,
	   it means, that serial was not found in the list */
	cssl_error=CSSL_ERROR_OOPS;
	return (cssl_error);
}

/**
 * @brief sets up the port parameters
 * 
 * setups the port, look at cssl_open
 * 
 * @param serial 
 * @param baud 
 * @param bits 
 * @param parity 
 * @param stop 
 * @return int32_t 
 */
int32_t cssl_setup(cssl_t *serial, int baud, int bits, int parity, int stop)
{
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	tcflag_t baudrate;
	tcflag_t databits;
	tcflag_t stopbits;
	tcflag_t checkparity;
#endif

	if (!cssl_started) {
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial) {
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}

	/* get the proper baudrate */
	serial->baud = baud;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	switch (baud) {
	case 75:
		baudrate=B75;
		break;
	case 110:
		baudrate=B110;
		break;
	case 150:
		baudrate=B150;
		break;
	case 300:
		baudrate=B300;
		break;
	case 600:
		baudrate=B600;
		break;
	case 1200:
		baudrate=B1200;
		break;
	case 2400:
		baudrate=B2400;
		break;
	case 4800:
		baudrate=B4800;
		break;
	case 9600:
		baudrate=B9600;
		break;
	case 19200:
		baudrate=B19200;
		break;
	case 38400:
		baudrate=B38400;
		break;
	case 57600:
		baudrate=B57600;
		break;
	case 115200:
		baudrate=B115200;
		break;
	default:
		baudrate=B9600;
	}

	/* databits */
	switch (bits) {
	case 7:
		databits=CS7;
		break;
	case 8:
		databits=CS8;
		break;
	default:
		databits=CS8;
	}

	/* parity, */
	switch (parity) {
	case 0:
		checkparity=0;
		break;
	case 1:   /* odd */
		checkparity=PARENB|PARODD;
		break;
	case 2:
		checkparity=PARENB;
		break;
	default:
		checkparity=0;
	}

	/* and stop bits */
	switch (stop) {
	case 1:
		stopbits=0;
		break;
	case 2:
		stopbits=CSTOPB;
		break;
	default:
		stopbits=0;
	}

	/* now we setup the values in port's termios */
	serial->tio.c_cflag=baudrate|databits|checkparity|stopbits|CLOCAL|CREAD;
	serial->tio.c_iflag=IGNPAR;
	serial->tio.c_oflag=0;
	serial->tio.c_lflag=0;
	serial->tio.c_cc[VMIN]=1;
	serial->tio.c_cc[VTIME]=0;

	/* we flush the port */
	tcflush(serial->fd,TCOFLUSH);
	tcflush(serial->fd,TCIFLUSH);

	/* we send new config to the port */
	tcsetattr(serial->fd,TCSANOW,&(serial->tio));
#else // windows
	serial->dcb.BaudRate = baud;
	serial->dcb.Parity = parity;
	serial->dcb.StopBits = stop;
	serial->dcb.ByteSize = bits;
	serial->dcb.DCBlength = sizeof(DCB);
	SetCommState(serial->handle, &(serial->dcb));
#endif

	cssl_error=CSSL_OK;
	return 0;
}

/**
 * @brief 
 * 
 * @param serial 
 * @param rtscts Boolean:  0 - no rts/cts control, 1 - rts/cts control
 * @param xonxoff Boolean:  0 - no xon/xoff,  1 - xon/xoff
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_setflowcontrol(cssl_t *serial, int rtscts, int xonxoff)
{
	if (!cssl_started)
	{
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial)
	{
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}

#if defined(COSMOS_WIN_OS)
	GetCommState(serial->handle, &(serial->dcb));
	if (rtscts)
	{
		serial->dcb.fRtsControl	= RTS_CONTROL_HANDSHAKE;
		serial->dcb.fOutxCtsFlow = TRUE;

	}
	else
	{
		serial->dcb.fRtsControl	= RTS_CONTROL_DISABLE;
		serial->dcb.fOutxCtsFlow = FALSE;
	}
#else
	/* We setup rts/cts (hardware) flow control */
	if (rtscts)
	{
		serial->tio.c_cflag |= CRTSCTS;
	} else
	{
		serial->tio.c_cflag &= ~CRTSCTS;
	}
#endif

#if defined(COSMOS_WIN_OS)
	if (xonxoff)
	{
        serial->dcb.fInX = serial->dcb.fOutX = TRUE;
	}
	else
	{
		serial->dcb.fInX = serial->dcb.fOutX = FALSE;

	}
	SetCommState(serial->handle, &(serial->dcb));
#else
	/* We setup xon/xoff (soft) flow control */
	if (xonxoff)
	{
		serial->tio.c_iflag |= (IXON|IXOFF);
	}
	else
	{
		serial->tio.c_iflag &= ~(IXON|IXOFF);
	}

	tcsetattr(serial->fd,TCSANOW,&(serial->tio));
#endif

	cssl_error=CSSL_OK;
	return 0;
}

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
/**
 * @brief Set read timeout
 * 
 * Sets timeout for reading in blocking mode. Read will return after at least 
 * minchar bytes, or timeout seconds between bytes. Timeout granularity is .1 
 * seconds.
 * 
 * @param serial Handle returned by::cssl_open
 * @param minchar Minimum number of characters
 * @param timeout Minimum inter character time in seconds.
 * 
 * @todo Verify this documentation.
 */
int32_t cssl_settimeout(cssl_t *serial, int minchar, double timeout)
#else // Windows
/**
 * @brief Set read timeout
 * 
 * Sets timeout for reading in blocking mode. Read will return after at least 
 * minchar bytes, or timeout seconds between bytes. Timeout granularity is .1 
 * seconds.
 * 
 * @param serial Handle returned by::cssl_open
 * @param minchar Minimum number of characters
 * @param timeout Minimum inter character time in seconds.
 * 
 * @todo minchar is not defined here.
 */
int32_t cssl_settimeout(cssl_t *serial, int, double timeout)
#endif
{
	if (!cssl_started)
	{
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial)
	{
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	serial->tio.c_cc[VMIN]=minchar;
	serial->tio.c_cc[VTIME]=(int)(timeout*10.+.4);

	tcsetattr(serial->fd,TCSANOW,&(serial->tio));
#else // windows
	_COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = timeout * 1000.;
	SetCommTimeouts(serial->handle, &timeouts);
#endif

	cssl_error=CSSL_OK;
	return 0;
}

/**
 * @brief sending a char
 * 
 * sends a char via serial port
 * 
 * @param serial 
 * @param c 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_putchar(cssl_t *serial, uint8_t c)
{
	if (!cssl_started)
	{
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (CSSL_ERROR_NOTSTARTED);
	}

	if (!serial)
	{
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (CSSL_ERROR_NULLPOINTER);
	}

#ifdef COSMOS_WIN_OS
    int n=0;
    WriteFile(serial->handle, &c, 1, (LPDWORD)((void *)&n), NULL);
    if(n<0)  return(-errno);
#else
	if (write(serial->fd,&c,1) < 0)
	{
		return (-errno);
	}
#endif

	COSMOS_USLEEP(10000000/serial->baud);
	return 0;
}

/**
 * @brief sending a null-terminated string
 * 
 * sends a null terminated string
 * 
 * @param serial 
 * @param str 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_putstring(cssl_t *serial, char *str)
{
	if (!cssl_started) {
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial) {
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}
	int nbytes = write(serial->fd,str,strlen(str));
	return nbytes;
}

/**
 * @brief sending a data of known size
 * 
 * sends a data of known size
 * 
 * @param serial 
 * @param data data
 * @param datalen length of data
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_putdata(cssl_t *serial, uint8_t *data, uint32_t datalen)
{
//	int32_t iretn = 0;

	if (!cssl_started)
	{
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (CSSL_ERROR_NOTSTARTED);
	}

	if (!serial)
	{
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (CSSL_ERROR_NULLPOINTER);
	}

	/*
	for (uint32_t i=0; i<datalen; ++i)
	{
		if ((iretn=cssl_putchar(serial, data[i])) < 0)
		{	
			return iretn;
		}
	}
	*/
	if (write(serial->fd,data,datalen) < 0)
	{
		return (-errno);
	}
	return 0;
}

/**
 * @brief SLIP mode: writing a data buffer
 * 
 * @param serial 
 * @param buf 
 * @param size 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_putslip(cssl_t *serial, uint8_t *buf, size_t size)
{
	size_t i, j;

	i = 0;
	cssl_putchar(serial,SLIP_FEND);
	for (j=0; j<size; j++)
	{
		switch (buf[j])
		{
		case SLIP_FEND:
			cssl_putchar(serial,SLIP_FESC);
			cssl_putchar(serial,SLIP_TFEND);
			i+=2;
			break;
		case SLIP_FESC:
			cssl_putchar(serial,SLIP_FESC);
			cssl_putchar(serial,SLIP_TFESC);
			i+=2;
			break;
		default:
			cssl_putchar(serial,buf[j]);
			i++;
			break;
		}
	}
	cssl_putchar(serial,SLIP_FEND);
	return (i);
}

/**
 * @brief NMEA mode: writing a data buffer
 * 
 * @param serial 
 * @param buf 
 * @param size 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_putnmea(cssl_t *serial, uint8_t *buf, size_t size)
{
	size_t j;
	uint8_t cs_in, digit1, digit2;
    string message_sent; // for debugging

	cs_in = 0;

    // All commands start with a dollar sign, followed by a
    // character command, a comma, command specific parameters,
    // an asterisk, a checksum, and a newline character.
    // An example for VN-100 command is shown below.
    // $VNRRG,11*73

    // start command '$'
	cssl_putchar(serial,'$');
    message_sent = '$';

    // iterate through the buffer to send each charcter to serial port
	for (j=0; j<size; j++)
	{
		cssl_putchar(serial,buf[j]);
        message_sent += buf[j];
        // check sum (xor?)
		cs_in ^= (uint8_t)buf[j];
	}
    // end of command '*'
	cssl_putchar(serial,'*');
    message_sent += '*';

	if (cs_in > 16)
	{
		digit1 = cs_in/16;
        if (digit1 < 10)
        {
            cssl_putchar(serial, '0'+digit1);
            message_sent += '0'+digit1;
        }
        else
        {
            cssl_putchar(serial, 'A'+digit1-10);
            message_sent += 'A'+digit1-10;
        }
	}
	else
	{
		cssl_putchar(serial, '0');
        message_sent += '0';
	}

	++j;
	digit2 = cs_in%16;
    if (digit2 <10 )
    {
        cssl_putchar(serial, '0'+digit2);
        message_sent += '0'+digit2;
    }
    else
    {
        cssl_putchar(serial, 'A'+digit2-10);
        message_sent += 'A'+digit2-10;
    }
	++j;
	cssl_putchar(serial, '\n');
    message_sent += "<CR><LF>";
	return (j+3);
}

/**
 * @brief waits until all data has been transmitted
 * 
 * @param serial 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_drain(cssl_t *serial)
{
	if (!cssl_started) {
		cssl_error=CSSL_ERROR_NOTSTARTED;
		return (cssl_error);
	}

	if (!serial) {
		cssl_error=CSSL_ERROR_NULLPOINTER;
		return (cssl_error);
	}

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
//	tcdrain(serial->fd);
//	tcflush(serial->fd,TCOFLUSH);
//	tcflush(serial->fd,TCIFLUSH);
	tcflush(serial->fd, TCIOFLUSH);
#else // windows
	PurgeComm(serial->handle, PURGE_RXCLEAR|PURGE_TXCLEAR);
#endif
	return 0;
}

/**
 * @brief blocking mode: reading a char
 * 
 * reads a char in blocking mode
 * 
 * @param serial 
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_getchar(cssl_t *serial)
{
	int result;
	uint8_t c;

#ifdef COSMOS_WIN_OS
    int n=0;
	result = ReadFile(serial->handle, &c, 1, (LPDWORD)((void *)&n), NULL);
#else
	result=read(serial->fd,&c,sizeof(c));
#endif
	if (result > 0)
	{
		return c;
	}
	else
	{
		if (result < 0)
		{
			return (-errno);
		}
		else
		{
			return (CSSL_ERROR_TIMEOUT);
		}
	}
}

/**
 * @brief blocking mode: reading a data buffer
 * 
 * reads a data to a buffer in blocking mode
 * 
 * @param serial 
 * @param buffer buffer for data
 * @param size buffer size
 * @return int32_t 
 * 
 * @todo Document this.
 */
int32_t cssl_getdata(cssl_t *serial, uint8_t *buffer, int size)
{
	int32_t iretn = 0;

	for (uint16_t i=0; i<size; ++i)
	{
		if ((iretn=cssl_getchar(serial)) < 0)
		{
			if (iretn == CSSL_ERROR_TIMEOUT)
			{
				return(i);
			}
			else
			{
				return iretn;
			}
		}
		else
		{
			buffer[i] = iretn;
		}
	}
	return (size);
}

/**
 * @brief Read SLIP frame.
 * 
 * Read an entire frame of SLIP encoded data from the serial port. Special SLIP 
 * characters are removed on the fly. Will stop early if supplied buffer size is
 * exceeded.
 * 
 * @param serial Handle returned from :cssl_open.
 * @param buf Byte array to store incoming data.
 * @param size Size of byte array.
 * @return int32_t Number of bytes read, up to maximum.
 */
int32_t cssl_getslip(cssl_t *serial, uint8_t *buf, uint16_t size)
{
	int16_t ch;
	uint16_t i;
	
	do
	{
		ch = cssl_getchar(serial);
		if (ch < 0)
		{
			if (ch == CSSL_ERROR_TIMEOUT)
			{
				return (CSSL_ERROR_SLIPIN);
			}
			else
			{
				return (ch);
			}
		}
	} while (ch != SLIP_FEND);

	i = 0;
	do
	{
		ch = cssl_getchar(serial);
		if (ch < 0)
		{
			if (ch == CSSL_ERROR_TIMEOUT)
			{
				return (CSSL_ERROR_SLIPOUT);
			}
			else
			{
				return (ch);
			}
		}
		if (i < size)
		{
			switch (ch)
			{
			case SLIP_FESC:
				ch = cssl_getchar(serial);
				switch (ch)
				{
				case SLIP_TFEND:
					buf[i] = SLIP_FEND;
					break;
				case SLIP_TFESC:
					buf[i] = SLIP_FESC;
					break;
				}
				++i;
				break;
			case SLIP_FEND:
				break;
			default:
				buf[i] = ch;
				++i;
				break;
			}
		}
	} while (ch != SLIP_FEND);

	return (i);
}

/**
 * @brief Read NMEA response.
 * 
 * Read an entire NMEA response from the serial port. The leading $ and trailing
 * * and checksum are removed, and only the payload of the response is returned.
 * Will stop early if supplied buffer size is exceeded.
 * 
 * @param serial Handle returned from :cssl_open.
 * @param buf Byte array to store incoming data.
 * @param size Size of byte array.
 * @return int32_t Number of bytes read, up to maximum.
 */
int32_t cssl_getnmea(cssl_t *serial, uint8_t *buf, uint16_t size)
{
	int16_t ch;
	uint16_t i;
	uint8_t cs_in, cs_out;
    string input;
	
	do
	{
		ch = cssl_getchar(serial);
        input += ch;
		if (ch < 0) return (ch);
	} while (ch != '$');

	i = 0;
	cs_in = 0;
	do
	{
		ch = cssl_getchar(serial);
        input += ch;
		if (ch < 0) return (ch);
		if (i < size)
		{
			switch (ch)
			{
			case '*':
				break;
			default:
				cs_in ^= (uint8_t)ch;
				buf[i] = ch;
				++i;
				break;
			}
		}
	} while (ch != '*');
	ch = cssl_getchar(serial);
    input += ch;
	if (ch < 0) return (ch);
	if (ch > '9')
	{
		if (ch > 'F')
		{
			cs_out = (ch - 'a' + 10) * 16;
		}
		else
		{
			cs_out = (ch - 'A' + 10) * 16;
		}
	}
	else
	{
		cs_out = (ch - '0') * 16;
	}
	ch = cssl_getchar(serial);
    input += ch;
	if (ch < 0) return (ch);
	if (ch > '9')
	{
		if (ch > 'F')
		{
			cs_out += (ch - 'a' + 10);
		}
		else
		{
			cs_out += (ch - 'A' + 10);
		}
	}
	else
	{
		cs_out += (ch - '0');
	}
	if (cs_in != cs_out)
		return (CSSL_ERROR_CHECKSUM);
	ch = cssl_getchar(serial);
    input += ch;

	return (i);
}

/**
 * @brief Read Xmodem frame.
 * 
 * Read one Xmodem block (frame) of data, removing control characters and 
 * calculating checksum. Supplied buffer is assumed to be at least 128 bytes.
 * 
 * @param serial Handle returned from :cssl_open.
 * @param buf Byte array to store incoming data.
 * @return int32_t Packet number or negative error.
 */
int32_t cssl_getxmodem(cssl_t* serial, uint8_t* buf)
{
	int16_t ch;

	ch = cssl_getchar(serial);
	if (ch != XMODEM_SOH)
	{
		if (ch == XMODEM_EOT)
		{
			return CSSL_ERROR_EOT;
		}
		else
		{
			return CSSL_ERROR_READ;
		}
	}
	uint8_t blocknum = cssl_getchar(serial);
	ch = cssl_getchar(serial);
	if (255-blocknum != ch)
	{
		return CSSL_ERROR_READ;
	}

	uint16_t i = 0;
	uint8_t csum = 0;
	do
	{
		ch = cssl_getchar(serial);
		if (ch < 0)
		{
			return (ch);
		}
		buf[i] = ch;
		++i;
		csum += ch;
	} while (i<128);

	ch = cssl_getchar(serial);
	if (ch != csum)
	{
		return CSSL_ERROR_CHECKSUM;
	}

	return blocknum;
}

/**
 * @}
 */

/*------------------------------------------*/

