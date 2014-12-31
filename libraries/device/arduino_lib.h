#include "configCosmos.h"

//#include <stdio.h>    /* Standard input/output definitions */
//#include <stdlib.h>
//#include <stdint.h>   /* Standard types */
//#include <cstring>   /* String function definitions */
//#include <unistd.h>   /* UNIX standard function definitions */
//#include <fcntl.h>    /* File control definitions */
//#include <errno.h>    /* Error number definitions */
#ifndef COSMOS_WIN_OS
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#endif
#include <getopt.h>

int arduino_init(char* port, int baud);
int arduino_printstring(char* str);
int arduino_printnum(int num);
int arduino_delay(int delay);
char* arduino_setport(char* port);
int arduino_setbaud(int baud);
char* arduino_read(char* str);
int arduino_closeport();
int serialport_init(const char* serialport, int baud);
int serialport_writebyte(int fd, uint8_t b);
int serialport_write(int fd, const char* str);
int serialport_read_until(int fd, char* buf, char until);

