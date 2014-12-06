/*
 * arduinolib
 * adapted from the arduino-serial program written by Tod E. Kurt
 * https://github.com/todbot/arduino-serial
 * only works for Mac and Linux, Windows support must be added
 */

#include "configCosmos.h"
#include "arduino_lib.h"
#include "cssl_lib.h"

#if defined(COSMOS_WIN_OS)

#include <windows.h>

#endif

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
#include <termios.h>  // POSIX terminal control definitions
#include <sys/ioctl.h>
#endif

#include <stdio.h>    // Standard input/output definitions
#include <unistd.h>   // UNIX standard function definitions
#include <fcntl.h>    // File control definitions
#include <errno.h>    // Error number definitions
#include <cstring>   // String function definitions

char serialport[256];
//int baudrate = B9600;
int baudrate = 9600;
int fd = 0;
char buf[256];
cssl_t *serial;

int arduino_init(char* port, int baud) {
	arduino_setport(port);
	arduino_setbaud(baud);
	if((fd = serialport_init(serialport, baudrate)) == -1)
		return -1;
	return fd;
}
int arduino_printstring(char* str) {
	int32_t iretn;
	strcpy(buf,str);
	if((iretn = serialport_write(fd, buf)) == -1)
		return -1;
	return iretn;
}
int arduino_printnum(int num) {
	int32_t iretn;
	if((iretn = serialport_writebyte(fd, (uint8_t)num)) == -1)
		return -1;
	return iretn;
}
int arduino_delay(int delay) {
	usleep(delay * 1000);
	return delay;
}
char* arduino_setport(char* port) {
	strcpy(serialport, port);
	return serialport;
}
int arduino_setbaud(int baud) {
	baudrate = baud;
	return baudrate;
}
char* arduino_read(char* str) {
    serialport_read_until(fd, str, '\n');
    //printf("read: %s\n",str);
	COSMOS_SLEEP(1) ;
	return str;
}

int arduino_closeport() {
	if(fd < 0)
		return -1;
	if(close(fd) < 0)
		return -1;
	return fd;
}
int serialport_writebyte( int fd, uint8_t b)
{
    int n = write(fd,&b,1);
    if( n!=1)
        return -1;
    return 0;
}

int serialport_write(int fd, const char* str)
{
    int len = strlen(str);
    int n = write(fd, str, len);
    if( n!=len ) 
        return -1;
    return 0;
}

int serialport_read_until(int fd, char* buf, char until)
{
    char b[1];
    int i=0;
    do { 
        int n = read(fd, b, 1);  // read a char at a time
        if( n==-1) return -1;    // couldn't read
        if( n==0 ) {
            usleep( 10 * 1000 ); // wait 10 msec try again
            continue;
        }
        buf[i] = b[0]; i++;
    } while( b[0] != until );

    buf[i] = 0;  // null terminate the string
    return 0;
}

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
int serialport_init(const char* serialport, int baud)
{
	serial = cssl_open(serialport, baud, 8, 0, 1);
	cssl_settimeout(serial, 0, 1.96);
	cssl_setflowcontrol(serial, 0, 0);

/*
	struct termios toptions;
    int fd;
    
    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        perror("init_serialport: Unable to open port ");
        return -1;
    }
    
    if (tcgetattr(fd, &toptions) < 0) {
        perror("init_serialport: Couldn't get term attributes");
        return -1;
    }
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
#ifdef B14400
    case 14400:  brate=B14400;  break;
#endif
    case 19200:  brate=B19200;  break;
#ifdef B28800
    case 28800:  brate=B28800;  break;
#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
    
    tcflush (fd, TCIFLUSH) ;
    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }
	*/

	return serial->fd;
}
