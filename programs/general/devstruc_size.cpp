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
#include "support/jsondef.h"

int main(int argc, char *argv[])
{
    printf("devicestruc: %lu\n", sizeof(devicestruc));
    printf("allstruc: %lu\n", sizeof(allstruc));
    printf("antstruc : %lu\n", sizeof(antstruc));
    printf("battstruc : %lu\n", sizeof(battstruc));
    printf("busstruc : %lu\n", sizeof(busstruc));
    printf("camstruc : %lu\n", sizeof(camstruc));
    printf("cpustruc : %lu\n", sizeof(cpustruc));
    printf("diskstruc : %lu\n", sizeof(diskstruc));
    printf("gpsstruc : %lu\n", sizeof(gpsstruc));
    printf("htrstruc : %lu\n", sizeof(htrstruc));
    printf("imustruc : %lu\n", sizeof(imustruc));
    printf("mccstruc : %lu\n", sizeof(mccstruc));
    printf("motrstruc : %lu\n", sizeof(motrstruc));
    printf("mtrstruc : %lu\n", sizeof(mtrstruc));
    printf("tcustruc : %lu\n", sizeof(tcustruc));
    printf("ploadstruc : %lu\n", sizeof(ploadstruc));
    printf("propstruc : %lu\n", sizeof(propstruc));
    printf("psenstruc : %lu\n", sizeof(psenstruc));
    printf("rotstruc : %lu\n", sizeof(rotstruc));
    printf("rwstruc : %lu\n", sizeof(rwstruc));
    printf("ssenstruc : %lu\n", sizeof(ssenstruc));
    printf("strgstruc : %lu\n", sizeof(pvstrgstruc));
    printf("sttstruc : %lu\n", sizeof(sttstruc));
    printf("suchistruc : %lu\n", sizeof(suchistruc));
    printf("swchstruc : %lu\n", sizeof(swchstruc));
    printf("tcvstruc : %lu\n", sizeof(tcvstruc));
    printf("telemstruc : %lu\n", sizeof(telemstruc));
    printf("txrstruc : %lu\n", sizeof(txrstruc));
    printf("rxrstruc : %lu\n", sizeof(rxrstruc));
    printf("thststruc : %lu\n", sizeof(thststruc));
    printf("tsenstruc : %lu\n", sizeof(tsenstruc));
    printf("tncstruc : %lu\n", sizeof(tncstruc));
}
