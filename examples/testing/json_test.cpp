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
#include <stdlib.h>
#include <string.h>
#include "jsonlib.h"

cosmosstruc *cosmos_data;

int main(int argc, char *argv[])
{

double result;
json_create(&cosmos_data);
if(argc == 2) {
	const char *ptr = argv[1];
	result = json_equation(&ptr,cosmos_data);
	printf("%f \n", result);
}

else if(argc == 1) {
	double timestamp = 500.;
	double node_utc = 400.;
	char testString[128];
	const char *strPtr = &testString[0];
	const char testOperators[10] = {"+-*/&|><="};
	json_set_double_name(timestamp, (char *)"timestamp",cosmos_data);
	json_set_double_name(node_utc, (char *)"node_utc",cosmos_data);
	
	for(int i = 0; i < 9; i++) {
		strPtr = testString;
		strcpy(testString, "(\"timestamp\"");
		testString[strlen(testString)] = testOperators[i];
		strcat(testString, "\"node_utc\")");
		result = json_equation(&strPtr,cosmos_data);
		printf("%f \n", result);
		for(int j = 0; j < 128; j++) {
			testString[j] = 0;
		}
	}
	
}
else {
	printf("Too many arguments \n");
	return 0;
}
}
