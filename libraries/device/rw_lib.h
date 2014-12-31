
#ifndef _RWLIB_H
#define _RWLIB_H 1

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include <stdio.h>

#include "cssl_lib.h"
#include "mathlib.h"
#include "cosmos-errno.h"
#include <cstring>
#include <unistd.h>
//#include <sys/select.h>
#include <sys/time.h>
//#include <sys/types.h>

#define RW_ID 1
#define RW_BAUD 9600
#define RW_BITS 8
#define RW_PARITY 0
#define RW_STOPBITS 1
#define RW_DEVICE "/dev/ttyOS1" //"/dev/tty.usbmodem1d11"

/**
* Internal structure containing the state of the reaction wheel.
* @brief Reaction wheel state
*/
typedef struct
	{
	float velocity;
	float acceleration;
	} rw_state;

int rw_connect(char *dev);
int rw_disconnect();

float rw_get_accel();
float rw_get_vel();
int rw_set_vel(double vel);
int rw_set_accel(double acc);
int rw_getdata(char *buf, int buflen);
int rw_send(char *buf, int force);
float rw_get_stored_vel();
float rw_get_stored_accel();
float rw_get_inertia();


#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)

#endif /* _RWLIB_H */

