#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include "gs232b_lib.h"
#include <cstring>

/**
* Internal descriptor for cssl serial control of GS-232B.
* @brief GS-232B serial handle
*/
static cssl_t *gs232b_serial = NULL;

static gs232b_state gs_state;

/**
* Connects to a Yaesu GS-232B computer controller, which in turn
* drives a Yaesu G-5500 antenna controller.
* @param dev pointer to a character string with the serial port it is
* connected to.
* @see cssl_start
* @see cssl_open
* @see cssl_setflowcontrol
*/
int gs232b_connect(char *dev)
{
cssl_start();
if (gs232b_serial != NULL)
	return(GS232B_ERROR_OPEN);

gs232b_serial = cssl_open(dev,GS232B_BAUD,GS232B_BITS,GS232B_PARITY,GS232B_STOPBITS);

return 0;
}

/**
* Close currently open GS-232B.
*/
int gs232b_disconnect()
{
if (gs232b_serial == NULL)
	return (GS232B_ERROR_CLOSED);

cssl_close(gs232b_serial);
return 0;
}

/**
* Routine to use in blocking mode. Reads the serial port until a New
* Line is received, then returns entire buffer.
* @param buf Pointer to a char buffer
* @param buflen ::int indicating the maximum size of the buffer
* @return ::int containing the number of bytes read.
*/
int gs232b_getdata(char *buf, int buflen)
{
int i,j;

i = 0;
while ((j=cssl_getdata(gs232b_serial,(uint8_t *)&buf[i],buflen-i)))
	{
	i += j;
	if (buf[i-1] == '\n')
		break;
	}
buf[i] = 0;
return (i);
}

/**
* Routine to be called when data is present on the serial port.
* @param id ::int containing the unit number of the serial port
* @param buf pointer to buffer containing the data
* @param buflen ::int containing the number of characters in the
* buffer.
*/
/*
void gs232b_callback(int id, uint8_t *buf, int buflen)
{

gs_state.data_received = 0;
printf("%d[%s]\n",buflen,buf);
switch (buf[0])
	{
case 'A':
	if (buf[1] != 'Z')
		break;
	switch (buf[2])
		{
	case 'o':
		switch (buf[8])
			{
		case '\r':
			gs_state.currentaz_offset_waiting = 1;
			gs_state.in_command = 1;
			gs_state.data_received = 1;
			break;
		case '=':
			gs_state.currentaz_offset = strtod(&buf[9],NULL);
			gs_state.data_received = 1;
			break;
			}
		break;
	case '=':
		gs_state.currentaz = strtod(&buf[3],NULL);
		if (buf[10] == 'E')
			gs_state.currentel = strtod(&buf[13],NULL);
		gs_state.data_received = 1;
		break;
		}
	break;
case 'C':
	if (buf[1] == 'o')
		{
		gs_state.in_command = 0;
		gs_state.currentaz_offset_waiting = 0;
		gs_state.currentel_offset_waiting = 0;
		gs_state.data_received = 1;
		}
	break;
case 'E':
	if (buf[1] != 'L')
		break;
	switch (buf[2])
		{
	case 'o':
		switch (buf[8])
			{
		case '\r':
			gs_state.currentel_offset_waiting = 1;
			gs_state.in_command = 1;
			gs_state.data_received = 1;
			break;
		case '=':
			gs_state.currentel_offset = strtod(&buf[9],NULL);
			gs_state.data_received = 1;
			break;
			}
		break;
	case '=':
		gs_state.currentel = strtod(&buf[3],NULL);
		gs_state.data_received = 1;
		break;
		}
	break;
case 'm':
	switch (buf[5])
		{
	case '3':
		gs_state.currentaz_mode = 360;
		gs_state.data_received = 1;
		break;
	case '4':
		gs_state.currentaz_mode = 450;
		gs_state.data_received = 1;
		gs_state.currentaz_center = 0;
		break;
		}
	break;
case 'N':
	if (buf[1] != ' ')
		break;
	gs_state.currentaz_center = 1;
	gs_state.data_received = 1;
	break;
case '?':
	if (buf[1] != '>')
		break;
	gs_state.data_received = 2;
	break;
case 'S':
	if (buf[1] != ' ')
		break;
	gs_state.currentaz_center = -1;
	gs_state.data_received = 1;
	break;
	}
}
*/

/**
* Routine to help calibrate rotor offset on specified axis. Turns
* calibration mode on in GS-232B.
* @param axis ::int , 0 = Azimuth, 1 = Elevation
*/
void gs232b_offset_wait(int axis)
{
switch (axis)
	{
case 0:
	gs232b_send((char *)"O\r",0);
	break;
case 1:
	gs232b_send((char *)"O2\r",0);
	break;
	}
}

/**
* Accept rotor offset calibration for currently calibrating axis.
*/
void gs232b_offset_accept()
{
cssl_putchar(gs232b_serial,'y');
}

void gs232b_full_scale_calibration(int axis)
{
}

void gs232b_rotate(int axis, int direction)
{
}

int gs232b_az_speed(int speed)
{
char out[50];

if (speed < 1 || speed > 4)
	return (GS232B_ERROR_OUTOFRANGE);
sprintf(out,"X%1d\r",speed);
gs232b_send(out,0);
return (speed);
}

int gs232b_goto(double az, double el)
{
char out[50];
double mel, daz, del, sep;
static int taz = 500;
static int tel = 500;
int iaz, iel;

if (az < 0 || az > RADOF(450) || el < 0 || el > DPI)
	return (GS232B_ERROR_OUTOFRANGE);

gs232b_get_az_el(&gs_state.currentaz,&gs_state.currentel);
if (az < DPI2 && az < gs_state.currentaz)
	{
	az += D2PI;
	}
else
	if (az > D2PI && az > gs_state.currentaz)
		{
		az -= D2PI;
		}
	
//maz = (az+gs_state.currentaz)/2.;
mel = .001+(el+gs_state.currentel)/2.;
del = el-gs_state.currentel;
daz = (az-gs_state.currentaz)/cos(mel);
sep = sqrt(daz*daz+del*del);
printf("az: %7.2f-%7.2f el: %7.2f-%7.2f sep: %8.3f \n",DEGOF(gs_state.currentaz),DEGOF(az),DEGOF(gs_state.currentel),DEGOF(el),DEGOF(sep));
if (sep > RADOF(1.))
	{
switch ((int)(4.5*sep/DPI))
	{
case 0:
	break;
case 1:
	gs232b_send((char *)"X1\r",0);
	break;
case 2:
	gs232b_send((char *)"X2\r",0);
	break;
case 3:
	gs232b_send((char *)"X3\r",0);
	break;
case 4:
default:
	gs232b_send((char *)"X4\r",0);
	break;
	}
	iaz = (int)(DEGOF(az)+.5);
	iel = (int)(DEGOF(el)+.5);
	if (iaz != taz || iel != tel)
		{
		sprintf(out,"W%03d %03d\r",iaz,iel);
		gs232b_send(out,0);
		taz = iaz;
		tel = iel;
		}
	}
return 0;
}

double gs232b_get_az()
{
char buf[20];
gs232b_send((char *)"C\r",1);
gs232b_getdata(buf,20);
sscanf(buf,"AZ=%03lf\r\n",&gs_state.currentaz);
return (RADOF(gs_state.currentaz));
}

double gs232b_get_el()
{
char buf[20];
gs232b_send((char *)"B\r",1);
gs232b_getdata(buf,20);
sscanf(buf,"EL=%03lf\r\n",&gs_state.currentel);
return (RADOF(gs_state.currentel));
}

int gs232b_get_az_el(double *az, double *el)
{
char buf[20];
gs232b_send((char *)"C2\r",1);
gs232b_getdata(buf,20);
sscanf(buf,"AZ=%03lf  EL=%03lf\r\n",&gs_state.currentaz,&gs_state.currentel);
*az = RADOF(gs_state.currentaz);
*el = RADOF(gs_state.currentel);
return 0;
}

double gs232b_get_az_offset()
{
return (gs_state.az_offset);
}

double gs232b_get_el_offset()
{
return (gs_state.el_offset);
}

void gs232b_get_state(gs232b_state *state)
{
*state = gs_state;
}

void gs232b_send(char *buf, int force)
{
static char lastbuf[256];

if (strcmp(lastbuf,buf) || force)
	{
	printf("%s\n",buf);
	cssl_putstring(gs232b_serial,buf);
	strncpy(lastbuf,buf,256);
	}
}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
