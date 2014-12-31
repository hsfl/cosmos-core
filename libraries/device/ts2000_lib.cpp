#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include "ts2000_lib.h"

/**
* Internal descriptor for cssl serial control of Kenwood TS-2000 radio.
* @brief TS-2000 serial handle
*/
static cssl_t *ts2000_serial = NULL;

static ts2000_state ts_state;

/**
* Connects to a Kenwood TS-2000 radio.
* @param dev pointer to a character string with the serial port it is
* connected to.
* @see cssl_start
* @see cssl_open
* @see cssl_setflowcontrol
*/
int ts2000_connect(char *dev)
{
cssl_start();
if (ts2000_serial != NULL)
	return(TS2000_ERROR_OPEN);

ts2000_serial = cssl_open(dev,TS2000_BAUD,TS2000_BITS,TS2000_PARITY,TS2000_STOPBITS);
cssl_setflowcontrol(ts2000_serial,1,0);
if (ts2000_serial == NULL)
	return (-errno);
cssl_settimeout(ts2000_serial,0,1.);

return 0;
}

/**
* Close currently open TS-2000.
*/
int ts2000_disconnect()
{
if (ts2000_serial == NULL)
	return (TS2000_ERROR_CLOSED);

cssl_close(ts2000_serial);
return 0;
}

/**
* Routine to use in blocking mode. Reads the serial port until a 
* Semi Colon is received, then returns entire buffer.
* @param buf Pointer to a char buffer
* @param buflen ::int indicating the maximum size of the buffer
* @return ::int containing the number of bytes read.
*/
int ts2000_getdata(char *buf, int buflen)
{
int i,j;

i = 0;
while ((j=cssl_getdata(ts2000_serial,(uint8_t *)&buf[i],buflen-i)) > 0)
	{
	i += j;
	if (buf[i-1] == ';')
		{
		buf[i] = 0;
		return (i);
		}
	}
buf[i] = 0;
return (-i);
}

/**
* Get radio frequency.
* @param channel ::int , 0 = VFO A, 1 = VFO B
* @return ::double containing the frequency of the radio, otherwise -1.
*/
double ts2000_get_frequency(int channel)
{
char buf[30];

switch (channel)
	{
case 0:
	cssl_putstring(ts2000_serial,(char *)"FA;");
	if (ts2000_getdata(buf,20) > 0)
		{
		sscanf(buf,"FA%lf;",&ts_state.frequency[0]);
		return (ts_state.frequency[0]);
		}
	break;
case 1:
	cssl_putstring(ts2000_serial,(char *)"FB;");
	if (ts2000_getdata(buf,20) > 0)
		{
		sscanf(buf,"FB%lf;",&ts_state.frequency[1]);
		return (ts_state.frequency[1]);
		}
	break;
	}
return (-1.);
}

/**
* Set radio frequency.
* @param channel ::int , 0 = VFO A, 1 = VFO B
* @param frequency Frequency, in Hertz
* @return ::double containing the frequency of the radio, otherwise -1.
*/
double ts2000_set_frequency(int channel, double frequency)
{
char buf[30];

switch (channel)
	{
case 0:
	sprintf(buf,"FA%011d;",(int)frequency);
	cssl_putstring(ts2000_serial,buf);
	return (ts2000_get_frequency(0));
	break;
case 1:
	sprintf(buf,"FB%011d;",(int)frequency);
	cssl_putstring(ts2000_serial,buf);
	return (ts2000_get_frequency(1));
	break;
	}
return (-1.);
}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
