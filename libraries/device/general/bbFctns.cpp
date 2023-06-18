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

// #include <unistd.h> //for COSMOS_USLEEP
#include "support/configCosmos.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <math.h>
#include <time.h>       /* for mktime()       */
//#include <sys/time.h>   /* for settimeofday() */

#include "device/general/cssl_lib.h"
#include "device/general/bbFctns.h"


extern cssl_t *SerialBb;



void FindBbPort(int *BbPortNum)
{
    int i, nBytes = 255;
    char strPort[32];
    char comString[256];
    cssl_t *SerialCheck;

    // Find Temperature Controller
    for(i=0 ; i<16 ; i++) {
        sprintf(strPort, "/dev/ttyUSB%d", i);
		SerialCheck=cssl_open(strPort, 2400,8,0,1);

		COSMOS_USLEEP(10000);
        if(SerialCheck) {
            cssl_settimeout(SerialCheck, 0, 1);  // 1 second timeout!

            // Is this a Temperature (BB) Controller
            strcpy(comString, "#00R00\r\n");
            cssl_putstring(SerialCheck,comString);
			COSMOS_USLEEP(50000);
            memset(comString, 0, 255);
            cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);


            // Takes a second time to get a reply if we did Slide first.
            strcpy(comString, "#00R00\r\n");
            cssl_putstring(SerialCheck,comString);
			COSMOS_USLEEP(50000);
            memset(comString, 0, 255);
            cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);


            if(strstr(comString, "#00R00=") != NULL) {
                // It is!
                *BbPortNum = i;
                cssl_drain(SerialCheck);
                cssl_close(SerialCheck);
                break;
            }
            cssl_drain(SerialCheck);
            cssl_close(SerialCheck);
        }
    }
}


bool OpenBb(cssl_t **SerialBb, int BbPortNum)
{
    int nBytes = 255;
    char comString[256];
    char strPort[32];

    // ***Open BB
    sprintf(strPort, "/dev/ttyUSB%d", BbPortNum);
	*SerialBb=cssl_open(strPort, 2400,8,0,1);
    if (!*SerialBb) {
        printf("%s\n",cssl_geterrormsg());
        return(false);
    } else {

        cssl_settimeout(*SerialBb, 0, 0.5);  // .05 second timeout.
        // Start by requesting the Black Body temperature (so it will be ready later).
        sprintf (comString, "#00R00\r\n");
        cssl_putstring(*SerialBb,comString);

        // Throw away response
		COSMOS_USLEEP(40000);
        cssl_getdata(*SerialBb, (uint8_t*)comString, nBytes);

        return(true);
    }
}


// *****************************************************************
// Set the black body temperature
// *****************************************************************
void runBbUnit1(float DesiredTemperature, cssl_t *SerialBb)
{
    char comStringIn[256];
    char comStringOut[256];
    int nBytes=255;


    // ***** Set the temperature.
    if(DesiredTemperature < 0) {
        sprintf (comStringOut, "#00M01-%.1lfC\r\n", fabs(DesiredTemperature) );
    }
    else {
        sprintf (comStringOut, "#00M01+%.1lfC\r\n", DesiredTemperature);
    }

    // ***** Send the command.
    cssl_putstring(SerialBb,comStringOut);
	COSMOS_USLEEP(100000);


    // ***** Get Athena's reply.
    memset (comStringIn, 0, 40);
    cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);
    if ((strstr (comStringIn, "M")) == NULL) {

        //Try again if Athena didn't respond.
        cssl_putstring(SerialBb,comStringOut);
		COSMOS_USLEEP(100000);

        //Read Athena's reply.
        memset (comStringIn, 0, 40);
        cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);
        if ((strstr (comStringIn, "M")) == NULL) {

            //Try a third time if Athena didn't respond.
            cssl_putstring(SerialBb,comStringOut);
			COSMOS_USLEEP(100000);

            //Read Athena's reply.
            memset (comStringIn, 0, 40);
            cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);
            if ((strstr (comStringIn, "M")) == NULL) {
                printf("Black Body not responding");
            } else {
                printf("\nTemperature set to %3.1fC" , DesiredTemperature);
            }
        } else {
            printf("\nTemperature set to %3.1fC" , DesiredTemperature);
        }
    } else {
        printf("\nTemperature set to %3.1fC" , DesiredTemperature);
    }



    // ***** Send command to get temperature (So it will be ready for next request.)
    strcpy(comStringOut, "#00R00\r\n");
    cssl_putstring(SerialBb,comStringOut);
	COSMOS_USLEEP(200000);

    return;
}
