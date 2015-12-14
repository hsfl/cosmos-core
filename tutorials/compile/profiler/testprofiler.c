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

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

void test(){
    int i=0;
    printf("HAAAAAA!\n");
    for (i=0; i<1e4; i++) {
        printf("I love number: %d\n",i);
    }
}

int main()
{
    int i=0;

    printf("This c program will exit in 10 seconds.\n");        
 
    //delay(1000);    
    usleep(1000000);
    test();

    for (i=0; i<1e6; i++) {
        printf("I love number: %d\n",i);
    }
 
    return 0;
}
