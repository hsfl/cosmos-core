//! \file cssl_lib.h
//! \brief Simple Serial library header file

//! \ingroup devices
//! \defgroup cssl_lib Simple Serial library support

/* Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details */

#include "configCosmos.h"
#include "sliplib.h"

#ifndef __CSSL_H__
#define __CSSL_H__

//#include <stdint.h>
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
#include <termios.h>
#endif

//! \ingroup cssl_lib
//! \defgroup cssl_lib_constants
//! @{
#define XMODEM_SOH 0x01
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18

//! @}
//! \ingroup cssl_lib
//! \defgroup cssl_lib_typedefs
//! @{
typedef struct __cssl_t {

    uint8_t buffer[255];      /* input buffer */

    int fd;                   /* tty file descriptor */

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	struct termios tio;       /* termios structure for the port */
    struct termios oldtio;    /* old termios structure */
#else
	struct _DCB dcb;
	struct _DCB olddcb;
	HANDLE handle;
#endif

	int baud;

    struct __cssl_t *next;

} cssl_t;

typedef enum {
	CSSL_OK=0,                 /* everything is all right */
	CSSL_ERROR_NOSIGNAL=CSSL_ERROR_BASE,     /* there's no free signal */
    CSSL_ERROR_NOTSTARTED,   /* you should first start cssl */
    CSSL_ERROR_NULLPOINTER,  /* you gave a null pointer to the function */
    CSSL_ERROR_OOPS,         /* internal error, something's erong */
    CSSL_ERROR_MEMORY,       /* there's no memory for cssl_t structure */
    CSSL_ERROR_OPEN          /* file doesnt exist or you aren't good user */
} cssl_error_t;

//! @}
//! \ingroup cssl_lib
//! \defgroup cssl_lib_functions
//! @{
/* get the error message */
const char *cssl_geterrormsg();

/* get the error code */
int32_t cssl_geterror();

/* start the cssl */
int32_t cssl_start();

/* finish all jobs, clear memory, etc. */
int32_t cssl_stop();

/* alloc new cssl_t struct and open the port */
cssl_t *cssl_open(const char *fname, /* pathname of port file,
				      * for example "/dev/ttyS0" */
		  int baud,   /* baudrate, integer, for example 19200 */
		  int bits,   /* data bits: 7 or 8 */
		  int parity, /* parity: 0 - none, 1-odd, 2-even */
		  int stop);  /* stop bits: 1 or 2 */

/* closes the port, and frees its cssl_t struct */
int32_t cssl_close(cssl_t *serial);

/* setups the port, look at cssl_open */
int32_t cssl_setup(cssl_t *serial,
		   int baud,
		   int bits,
		   int parity,
		   int stop);

int32_t cssl_setflowcontrol(cssl_t *serial,
			 int rtscts,   /* Boolean: 
					* 0 - no rts/cts control,
					* 1 - rts/cts control 
					*/
			 int xonxoff); /* Boolean: 
					* 0 - no xon/xoff, 
					* 1 - xon/xoff 
					*/

/* sends a char via serial port */
int32_t cssl_putchar(cssl_t *serial, uint8_t c);

/* sends a null terminated string */
int32_t cssl_putstring(cssl_t *serial,
		       char *str);

/* sends a data of known size */
int32_t cssl_putdata(cssl_t *serial,
		  uint8_t *data, /* data */
		  uint32_t datalen);  /* length of data */

int32_t cssl_putslip(cssl_t *serial, uint8_t *buf, uint16_t size);
int32_t cssl_putnmea(cssl_t *serial, uint8_t *buf, uint16_t size);

/* waits until all data has been transmited */

int32_t  cssl_drain(cssl_t *serial);

/*====================================== 
 * Blocking mode 
 */

int32_t cssl_settimeout(cssl_t *serial, int minchar, double timeout);

/* reads a char in blocking mode */
int32_t cssl_getchar(cssl_t *serial);

/* reads a data to a buffer in blocking mode*/
int32_t cssl_getdata(cssl_t *serial,
		 uint8_t *buffer,  /* buffer for data */
		 int size);        /* buffer size */

int32_t cssl_getslip(cssl_t *serial, uint8_t *buffer, uint16_t size);
int32_t cssl_getnmea(cssl_t *serial, uint8_t *buf, uint16_t size);
int32_t cssl_getxmodem(cssl_t* serial, uint8_t* buffer);

//! @}

#endif /* __CSSL_H__ */
