/*! \file rw_lib.cpp
	\brief Support routines for generic reaction wheel
*/

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include "rw_lib.h"

#if !defined(COSMOS_WIN_OS)
#include <sys/select.h>
#endif

/**
* Internal descriptor for cssl serial control of reaction wheel.
* @brief Reaction wheel serial handle
*/
static cssl_t *rw_serial = NULL;
static rw_state rwh_state;

/**
* Connects to a controller which drives a reaction wheel.
* @param dev pointer to a character string with the serial port it is
* connected to.
* @see cssl_start
* @see cssl_open
* @see cssl_setflowcontrol
*/
int rw_connect(char *dev)
{
cssl_start();
if (rw_serial != NULL)
	return(RW_ERROR_OPEN);
rw_serial = cssl_open(dev,RW_BAUD,RW_BITS,RW_PARITY,RW_STOPBITS);
if (rw_serial == NULL)
	return(RW_ERROR_FAILED);
return 0;
}

/**
* Close currently open reaction wheel.
*/
int rw_disconnect()
{
if (rw_serial == NULL)
	return (RW_ERROR_CLOSED);

cssl_close(rw_serial);
return 0;
}

/**
* Routine to use in blocking mode. Reads the serial port until a New
* Line is received, then returns entire buffer. 
* @param buf Pointer to a char buffer
* @param buflen ::int indicating the maximum size of the buffer
* @return ::int containing the number of bytes read.
*/
int rw_getdata(char *buf, int buflen)
{
int fd = rw_serial->fd;
int i,retThisVal,retval;
char inChar;
fd_set fds;
struct timeval tv;
i = 0;
retThisVal = 0;
while(retThisVal == 0) {
	FD_ZERO(&fds); //resets file descriptor
	FD_SET(fd, &fds); //sets fds as the serial's file descriptor
	tv.tv_sec = 1; //will wait one second
	tv.tv_usec = 0;

	retval = select((fd)+1, &fds, NULL, NULL, &tv); //monitors the serial port

	if(retval >= 0) {
		inChar = (char) cssl_getchar(rw_serial);
		if(inChar != '\n' && i < buflen - 1) {
			buf[i] = inChar;
			i++;
			buf[i] = 0;
		}
		else
			retThisVal = i;
	}
	else
		retThisVal = RW_ERROR_FAILED;
	
}
return retThisVal;
}
/*
while ((j=cssl_getdata(rw_serial,(uint8_t *)&buf[i],buflen-i)))
	{
	i += j;
	if (buf[i-1] == '*')
		break;
	}
	
buf[i] = 0;
return (i);
}*/


//moved to arduino_lib
/** 
* Instructs the reaction wheel to move at a certain velocity.
* @param vel ::double indicating the velocity that the wheel is to move at (rad/s)
* @return 0 if successful, else RW_ERROR_OUTOFRANGE
*/
int rw_set_vel(double vel)
{
char out[50];
sprintf(out,"$SET,RW,SPD,%f*",vel); //should print "$v[velocity]*
rw_send(out,0);
return 0;
}

/** 
* Instructs the reaction wheel to move at a certain acceleration.
* @param accel ::double indicating the acceleration that the wheel is to move at (rad/s)
* @return 0 if successful
*/
int rw_set_accel(double accel)
{
char out[50];
sprintf(out, "$SET,RW,ACC,%f*",accel);
rw_send(out,0);
return 0;
}

/**
* Gets and stores the reaction wheel's acceleration.
* @return Reaction wheel's acceleration (::float)
*/
float rw_get_accel() {
char buf[20];
int i;
float accel;
i = rw_send((char *)"$A*",0);
if(i == RW_ERROR_SEND) {
	return RW_ERROR_SEND;
}
rw_getdata(buf, 20);
sscanf(buf,"$a %f *", &accel);
rwh_state.acceleration = accel;
return accel;
}

/**
* Gets and stores the reaction wheel's velocity.
* @return Reaction wheel's velocity (::float)
*/
float rw_get_vel() {
char buf[20];
int i;
float vel;
i = rw_send((char *)"$V*",0);
if(i == RW_ERROR_SEND) {
	return RW_ERROR_SEND;
}
rw_getdata(buf, 20);
sscanf(buf,"$v %f *",&vel);
rwh_state.velocity = vel;
return vel;
}

float rw_get_stored_vel() {
	return rwh_state.velocity;
}

float rw_get_stored_accel() {
	return rwh_state.acceleration;
}

/**
* Stores a reaction wheel state.
* @param state pointer to a rw_state
*/
void rw_get_state(rw_state *state)
{
rwh_state = *state;
}

/**
* Sends a message to the reaction wheel through the serial port.
* @param buf pointer to buffer containing string to be sent to reaction wheel
* @param force ::int if 1, forces buf to be written to the serial port.
* @return RW_ERROR_FAILED if message could not be sent, else 0
*/
int rw_send(char *buf, int force)
{
int i;
static char lastbuf[256];

if (strcmp(lastbuf,buf) || force)
	{
	printf("%s\n",buf);
	cssl_putstring(rw_serial,buf);
	i = cssl_geterror();
	if(i > 0) {
		return RW_ERROR_SEND;
	}
	strncpy(lastbuf,buf,256);
	}
return 0;
}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
