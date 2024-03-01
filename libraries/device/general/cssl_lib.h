/**
 * @file cssl_lib.h
 * @brief Simple Serial library header file
 * 
 * @todo This is not HSFL code. Verify the appropriate attributions are 
 * included.
 * 
 * Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details
 * 
 * @ingroup devices
 * @defgroup cssl_lib Simple Serial library support
 */

#include "support/configCosmos.h"
#include "support/sliplib.h"

#ifndef __CSSL_H__
#define __CSSL_H__

//#include <stdint.h>
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
#include <termios.h>
#endif

/**
 * @ingroup cssl_lib
 * @defgroup cssl_lib_constants Simple Serial Library Constants
 * @{
 */
/** @todo Document this. */
#define XMODEM_SOH 0x01
/** @todo Document this. */
#define XMODEM_EOT 0x04
/** @todo Document this. */
#define XMODEM_ACK 0x06
/** @todo Document this. */
#define XMODEM_NAK 0x15
/** @todo Document this. */
#define XMODEM_CAN 0x18

/**
 * @}
 * @ingroup cssl_lib
 * @defgroup cssl_lib_typedefs Smple Serial Library Typedefs
 * @{
 */

/**
 * @brief 
 * 
 * @todo Document this.
 */
typedef struct __cssl_t {

    uint8_t buffer[255];      /* input buffer */

    int fd;                   /* tty file descriptor */

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
	struct termios tio;       /* termios structure for the port */
    struct termios oldtio;    /* old termios structure */
#else // windows
	struct _DCB dcb;
	struct _DCB olddcb;
	HANDLE handle;
#endif

	int baud;

    struct __cssl_t *next;

} cssl_t;

/**
 * @brief 
 * 
 * @todo Document this.
 */
typedef enum {
	CSSL_OK=0,                 /* everything is all right */
	CSSL_ERROR_NOSIGNAL=CSSL_ERROR_BASE,     /* there's no free signal */
    CSSL_ERROR_NOTSTARTED,   /* you should first start cssl */
    CSSL_ERROR_NULLPOINTER,  /* you gave a null pointer to the function */
    CSSL_ERROR_OOPS,         /* internal error, something's erong */
    CSSL_ERROR_MEMORY,       /* there's no memory for cssl_t structure */
    CSSL_ERROR_OPEN          /* file doesnt exist or you aren't good user */
} cssl_error_t;

/**
 * @}
 * @ingroup cssl_lib
 * @defgroup cssl_lib_functions Simple Serial Library Functions
 * @{
 */

/**************************************
 * Public functions
 **************************************/

/** Error handling */

const char *cssl_geterrormsg();
int32_t cssl_geterror();

/** Starting/stopping cssl */

int32_t cssl_start();
int32_t cssl_stop();

/** Basic port operation - open/close */

cssl_t *cssl_open(const char *fname, int baud, int bits, int parity, int stop);
int32_t cssl_close(cssl_t *serial);

/** Port setup */

int32_t cssl_setup(cssl_t *serial, int baud,int bits, int parity, int stop);
int32_t cssl_setflowcontrol(cssl_t *serial, int rtscts, int xonxoff);
/** @todo This has multiple declarations based on the OS */
int32_t cssl_settimeout(cssl_t *serial, int minchar, double timeout);

/** Serial communication */

int32_t cssl_putchar(cssl_t *serial, uint8_t c);
int32_t cssl_putstring(cssl_t *serial, char *str);
int32_t cssl_putdata(cssl_t *serial, uint8_t *data, uint32_t datalen);
int32_t cssl_putslip(cssl_t *serial, uint8_t *buf, size_t size);
int32_t cssl_putnmea(cssl_t *serial, uint8_t *buf, size_t size);
int32_t cssl_drain(cssl_t *serial);

/** Blocking mode */

int32_t cssl_getchar(cssl_t *serial);
int32_t cssl_getdata(cssl_t *serial, uint8_t *buffer, int size);
int32_t cssl_getslip(cssl_t *serial, uint8_t *buffer, uint16_t size);
int32_t cssl_getnmea(cssl_t *serial, uint8_t *buf, uint16_t size);
int32_t cssl_getxmodem(cssl_t* serial, uint8_t* buffer);

/**
 * @}
 */

#endif /* __CSSL_H__ */
