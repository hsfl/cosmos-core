/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "support/configCosmos.h"

//#include <stdio.h>    /* Standard input/output definitions */
//#include <stdlib.h>
//#include <stdint.h>   /* Standard types */
//#include <cstring>   /* String function definitions */
//// #include <unistd.h>   /* UNIX standard function definitions */
//#include <fcntl.h>    /* File control definitions */
//#include <errno.h>    /* Error number definitions */
#ifndef COSMOS_WIN_OS
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#endif
//#include <getopt.h>

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

