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

#include <stdio.h>
#include <sys/time.h>
#include "timelib.h"

using namespace std;

int main()
{
struct timeval first, last, delta;
struct timezone x;

int num=rand();
int flag=0;

float timeDiff;

printf("Calculating...\n");

//Get the initial time
gettimeofday(&first, &x);

//Do the process
for(int i;i<1000000;i++)
{
    if(num==i)
    {
        printf("Found: %d ", num);
        flag=1;
    }
}

if(flag==0)
{
    printf("No results found ");
}

//Get the final time
gettimeofday(&last, &x);

//Caculate the difference
timeDiff=elapsed_time(delta,first,last);
printf("in %f milliseconds\n", timeDiff); 
}
