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
#include "jsondef.h"

int main(int argc, char *argv[])
{
	printf("devicestruc: %u\n", sizeof(devicestruc));
	printf("allstruc: %u\n", sizeof(allstruc));
	printf("antstruc : %u\n", sizeof(antstruc));
	printf("battstruc : %u\n", sizeof(battstruc));
	printf("busstruc : %u\n", sizeof(busstruc));
	printf("camstruc : %u\n", sizeof(camstruc));
	printf("cpustruc : %u\n", sizeof(cpustruc));
    printf("diskstruc : %u\n", sizeof(diskstruc));
    printf("gpsstruc : %u\n", sizeof(gpsstruc));
	printf("htrstruc : %u\n", sizeof(htrstruc));
	printf("imustruc : %u\n", sizeof(imustruc));
	printf("mccstruc : %u\n", sizeof(mccstruc));
	printf("motrstruc : %u\n", sizeof(motrstruc));
	printf("mtrstruc : %u\n", sizeof(mtrstruc));
	printf("tcustruc : %u\n", sizeof(tcustruc));
	printf("ploadstruc : %u\n", sizeof(ploadstruc));
	printf("propstruc : %u\n", sizeof(propstruc));
	printf("psenstruc : %u\n", sizeof(psenstruc));
	printf("rotstruc : %u\n", sizeof(rotstruc));
	printf("rwstruc : %u\n", sizeof(rwstruc));
	printf("ssenstruc : %u\n", sizeof(ssenstruc));
	printf("strgstruc : %u\n", sizeof(strgstruc));
	printf("sttstruc : %u\n", sizeof(sttstruc));
	printf("suchistruc : %u\n", sizeof(suchistruc));
	printf("swchstruc : %u\n", sizeof(swchstruc));
	printf("tcvstruc : %u\n", sizeof(tcvstruc));
	printf("telemstruc : %u\n", sizeof(telemstruc));
	printf("txrstruc : %u\n", sizeof(txrstruc));
	printf("rxrstruc : %u\n", sizeof(rxrstruc));
	printf("thststruc : %u\n", sizeof(thststruc));
	printf("tsenstruc : %u\n", sizeof(tsenstruc));
}
