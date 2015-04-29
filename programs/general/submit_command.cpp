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

#include "configCosmos.h"
#include "datalib.h"
#include "jsonlib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
	longeventstruc com;

	com.type = EVENT_TYPE_COMMAND;
	com.flag = 0;
	com.data[0] = 0;
	com.condition[0] = 0;
	com.utc = 0;
	com.utcexec = 0.;

	switch (argc)
	{
	case 6:
		{
			com.flag |= EVENT_FLAG_REPEAT;
		}
	case 5:
		{
			strcpy(com.condition, argv[4]);
			com.flag |= EVENT_FLAG_CONDITIONAL;
		}
	case 4:
		{
			switch (argv[3][0])
			{
			case '+':
				{
					double seconds = atof(&argv[3][1]);
					com.utc = currentmjd() + seconds / 86400.;
					break;
				}
			default:
				{
					com.utc = atof(argv[3]);
					break;
				}
			}
		}
	case 3:
		{
			strcpy(com.data, argv[2]);
		}
	case 2:
		{
			strcpy(com.name, argv[1]);
			break;
		}
	default:
		{
			printf("Usage: submit_command name command_string [time [condition [repeat_flag]]]\n");
			break;
		}
	}

	string jsp;

	json_out_commandevent(jsp, com);
	printf("%s\n", jsp.c_str());
}
