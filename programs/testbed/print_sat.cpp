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

#include "physicslib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

svector azel;
std::vector<nodestruc> track;
cosmosstruc *cdata;
char outputBuffer[3][400];//holds blocks of text so they can be generated in series and then displayed side by side.
void printBufferLeftToRight(int blockCount, const char *indent, const char *spacer);
int rvector2buffer (char *buffer, rvector *rv, const char *name);
int gvector2buffer (char *buffer, gvector *gv, const char *name);
int quat2buffer (char *buffer, quaternion *qu, const char *name);
int rmatrix2buffer (char *buffer, rmatrix *rm, const char *name);
/*How to add two things to the same block in the buffer:
	n = rvector2buffer(outputBuffer[0], &cdata[0].physics.moment,  "moment");
	n += rvector2buffer(&outputBuffer[0][n], &cdata[0].physics.com, "com");*/

/*Usage: Can be called with 0, 1 or 2 arguments: a section name to display only 1 section, or a filename to read a certain file.
arguments can be entered in any order, filenames are identified by .ini extension*/
int main(int argc, char *argv[]){
int i, j, n, jumpto = -1;

//The various sections of nodestruc.
char sections[][8] = {"general","info","gs","rw","tsen","piece","comp","strg","batt","ssen","imu","mtr","gps","cpu","pload"};
/*^Note: if, in addition to the battstruc_s array 'stat.batt[]' (for example), there is an integer 'stat.batt_cnt', then
the "batt" section will display the value of 'stat.batt_cnt' as well as all the values in all the battstruc_s array.*/

char filepath[34] = {"node.ini"};//default filepath, can be changed by arguments.

j=1;
while (j<=2&&j<argc) { //If there's more than 1 argument, loop through the arguments until we're out of range or have seen the first 2.
    i=0;
    while(i<15) {//loop through the sections and look for a match.
        if (strcmp(argv[j], sections[i]) == 0) { //if a match is found,
            jumpto = i;//...jump to that section.
            i=0;
            break;//stop scanning through the sections.
        }
        i++;
    }
    if (i==15) {//If i reached 14 that means no matching section was found, this might be a filepath,
        int len = strlen(argv[j]);//get the length,
        if (len>4) {//a filepath must be at least 5 characters because of the extension
            if (strcmp(&argv[j][len-4], ".ini")==0) {//filenames will end with the .ini extension,
                strcpy(filepath, argv[j]); //override the default filepath with the user specified one.
            }
        }
    }
    j++;//continue to next argument...
}


/*FILE *satellite;
satellite = fopen(filepath, "r");
if (satellite==NULL) {
    printf("\n<Error opening file>\nMake sure there's really a file at:\n");
    printf("%s\n",filepath);
    printf("or type a different filename.\n");
    return(0);
} else {
    fgets(jsonfile,AGENTMAXBUFFER,satellite);
    json_parse(jsonfile);
    fclose(satellite);
}*/
cdata = json_create();
json_setup_node(argv[1],cdata);
json_clone(cdata);

switch (jumpto) {
case -1:
    printf("\n-------------------------------------All sections-------------------------------------------\n");
case 0:
{
	printf("\ncdata:\n");
	printf("Name: %s\n",cdata[0].node.name);
	printf("gs_cnt: %d\trw_cnt: %d\ttsen_cnt: %d\tpiece_cnt: %d\n",cdata[0].node.target_cnt,cdata[0].devspec.rw_cnt,cdata[0].devspec.tsen_cnt,cdata[0].node.piece_cnt);
	printf("device_cnt: %d\tbus_cnt: %d\tstrg_cnt: %d\tbatt_cnt: %d\tssen_cnt: %d\n",cdata[0].node.device_cnt,cdata[0].devspec.bus_cnt,cdata[0].devspec.strg_cnt,cdata[0].devspec.batt_cnt,cdata[0].devspec.ssen_cnt);
	printf("imu_cnt: %d\tmtr_cnt: %d\tgps_cnt: %d\tcpu_cnt: %d\tpload_cnt: %d\n",cdata[0].devspec.imu_cnt,cdata[0].devspec.mtr_cnt,cdata[0].devspec.gps_cnt,cdata[0].devspec.cpu_cnt,cdata[0].devspec.pload_cnt);
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 1:
{
    printf("\nInfo:\n");
	printf("\thcap: %11.5g, mass: %11.5g, area: %11.5g, battcap: %11.5g\n",cdata[0].physics.hcap,cdata[0].physics.mass,cdata[0].physics.area,cdata[0].node.battcap);
	rvector2buffer(outputBuffer[0], &cdata[0].physics.moi,  "mom");
	rvector2buffer(outputBuffer[1], &cdata[0].physics.com, "com");
    printBufferLeftToRight(2,"\t", "\t| ");
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 2:
{
	printf("\nGS:\t%d\n",cdata[0].node.target_cnt);
	for (i=0;i<cdata[0].node.target_cnt;i++) {
	azel = groundstation(cdata[0].node.loc,track[i].loc);
        printf("\tgs %d:\n",i);
        printf("\t\tName: %s\n", track[i].name);
        sprintf(outputBuffer[0], "az: %11.5g, el: %11.5g\n", azel.lambda, azel.phi);
//        gvector2buffer(outputBuffer[1], &track[i].loc, "loc");
        printBufferLeftToRight(2, "\t\t", "\t| ");
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 3:
{
	printf("\nRW:\t%d\n",cdata[0].devspec.rw_cnt);
	for (i=0;i<cdata[0].devspec.rw_cnt;i++) {
        printf("\trw %d:\n",i);
		sprintf(outputBuffer[0], "cidx: %d  \n\nmax:\n%11.5g", cdata[0].devspec.rw[i]->gen.cidx, cdata[0].devspec.rw[i]->mxomg);
		n = rvector2buffer(outputBuffer[1], &cdata[0].devspec.rw[i]->mom, "mom");
		n += quat2buffer(&outputBuffer[1][n], &cdata[0].devspec.rw[i]->align, "align");
        printBufferLeftToRight(2, "\t\t", "\t| ");
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 4:
{
	printf("\nTsen:\t%d\n",cdata[0].devspec.tsen_cnt);
	for (i=0;i<cdata[0].devspec.tsen_cnt;i++) printf("\ttsen %d: \tcidx: %d\n",i,cdata[0].devspec.tsen[i]->gen.cidx);
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 5:
{
	printf("\nPart:\t%d\n",cdata[0].node.piece_cnt);
	for (i=0;i<cdata[0].node.piece_cnt;i++) {
        printf("\tpiece %d:\n",i);
		printf("\t\tName: %s\ttype: %d\n",cdata[0].piece[i].name,cdata[0].piece[i].type);
		printf("\t\temi: %11.5g, abs: %11.5g hcap: %11.5g, hcon: %11.5g\n",cdata[0].piece[i].emi,cdata[0].piece[i].abs,cdata[0].piece[i].hcap,cdata[0].piece[i].hcon);
		sprintf(outputBuffer[0],"mass: %11.5g, area: %11.5g\ndim: %11.5g",cdata[0].piece[i].mass,cdata[0].piece[i].area,cdata[0].piece[i].dim);
		rvector2buffer(outputBuffer[1], &cdata[0].piece[i].normal, "normal");
		rvector2buffer(outputBuffer[2], &cdata[0].piece[i].centroid, "centroid");
        printBufferLeftToRight(3, "\t\t", "\t| ");
		rvector2buffer(outputBuffer[0], &cdata[0].piece[i].shove, "shove");
		rvector2buffer(outputBuffer[1], &cdata[0].piece[i].twist, "twist");
        printBufferLeftToRight(2, "\t\t", "\t| ");
		printf("\t\t%d Point%c%c\n",cdata[0].piece[i].pnt_cnt,((cdata[0].piece[i].pnt_cnt==1)?' ':'s'),((cdata[0].piece[i].pnt_cnt==0)?'.':':'));
		if (cdata[0].piece[i].pnt_cnt>0) {
            int col, row = 0, items;
			items = (cdata[0].piece[i].pnt_cnt/3)+(cdata[0].piece[i].pnt_cnt%3>0?1:0);
			while (row<items&&row<cdata[0].piece[i].pnt_cnt) {
                col = 0;
                j = row;
				while(col<3&&j<cdata[0].piece[i].pnt_cnt) {
                    char nums[5];
                    sprintf(nums, "%d", j+1);
					rvector2buffer(outputBuffer[col], &cdata[0].piece[i].points[j], nums);
                    col++;
                    j += items;
                }
                printBufferLeftToRight(3, "\t\t  ", " | ");
                row++;
            }
        }
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 6:
{
	printf("\nComp:\t%d\n", cdata[0].node.device_cnt);
	for (i=0; i<cdata[0].node.device_cnt;i++) {
		printf("\tcomp %d:\tdidx: %d\tpidx: %d\tbidx: %d\n",i,cdata[0].device[i].all.gen.didx,cdata[0].device[i].all.gen.pidx,cdata[0].device[i].all.gen.bidx);
		printf("\t\tcurrent: %11.5g, volt: %11.5g\n",cdata[0].device[i].all.gen.amp,cdata[0].device[i].all.gen.volt);
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 7:
{
	printf("\nStrings:\t%d\n", cdata[0].devspec.strg_cnt);
	for(i=0; i<cdata[0].devspec.strg_cnt; i++) {
		printf("\tstrg %d:\tcidx: %d\n", i, cdata[0].devspec.strg[i]->gen.cidx);
		printf("\t\teffbase: %11.5g, effslope: %11.5g, maxpower: %11.5g\n",cdata[0].devspec.strg[i]->effbase,cdata[0].devspec.strg[i]->effslope,cdata[0].devspec.strg[i]->maxpower);
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 8:
{
	printf("\nBatteries:\t%d\n", cdata[0].devspec.batt_cnt);
	for(i=0; i<cdata[0].devspec.batt_cnt; i++) {
		printf("\tbatt %d:\tcidx: %d,\tcapacity: %11.5g, efficiency: %11.5g\n", i, cdata[0].devspec.batt[i]->gen.cidx, cdata[0].devspec.batt[i]->capacity, cdata[0].devspec.batt[i]->efficiency);
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 9:
{
	printf("\nSsen:\t%d\n", cdata[0].devspec.ssen_cnt);
	for(i=0; i<cdata[0].devspec.ssen_cnt; i++) {
        printf("\tssen %d:\n", i);
		sprintf(outputBuffer[0], "cidx: %d", cdata[0].devspec.ssen[i]->gen.cidx);
		quat2buffer(outputBuffer[1], &cdata[0].devspec.ssen[i]->align, "align");
        printBufferLeftToRight(2, "\t\t", "\t| ");
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 10:
{
	printf("\nIMU:\t%d\n", cdata[0].devspec.imu_cnt);
	for(i=0; i<cdata[0].devspec.imu_cnt; i++) {
        printf("\timu %d:\n", i);
		sprintf(outputBuffer[0], "cidx: %d", cdata[0].devspec.imu[i]->gen.cidx);
		quat2buffer(outputBuffer[1], &cdata[0].devspec.imu[i]->align, "align");
        printBufferLeftToRight(2, "\t\t", "\t| ");
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 11:
{
	printf("\nMTR:\t%d\n", cdata[0].devspec.mtr_cnt);
	for(i=0; i<cdata[0].devspec.mtr_cnt; i++) {
        printf("\tmtr %d:\n", i);
		sprintf(outputBuffer[0], "cidx: %d", cdata[0].devspec.mtr[i]->gen.cidx);
//        rvector2buffer(outputBuffer[1], &cdata[0].devspec.mtr[i]->mom, "mom");
        printBufferLeftToRight(2, "\t\t", "\t| ");
		quat2buffer(outputBuffer[0], &cdata[0].devspec.mtr[i]->align, "align");
        printBufferLeftToRight(1, "\t\t", "");
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 12:
{
	printf("\nGPS:\t%d\n", cdata[0].devspec.gps_cnt);
	for(i=0; i<cdata[0].devspec.gps_cnt; i++) {
		printf("\tgps %d:\tcidx: %d\n", i, cdata[0].devspec.gps[i]->gen.cidx);
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 13:
{
	printf("\nCPU:\t%d\n", cdata[0].devspec.cpu_cnt);
	for(i=0; i<cdata[0].devspec.cpu_cnt; i++) {
		printf("\tcpu %d:\tcidx: %d\n", i, cdata[0].devspec.cpu[i]->gen.cidx);
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
case 14:
{
	printf("\nPayloads:\t%d\n", cdata[0].devspec.pload_cnt);
	for(i=0; i<cdata[0].devspec.pload_cnt; i++) {
		printf("\tpload %d:\tcidx: %d\tkey_cnt: %d\n", i, cdata[0].devspec.pload[i]->gen.cidx, cdata[0].devspec.pload[i]->key_cnt);
		for(j=0; j<cdata[0].devspec.pload[i]->key_cnt; j++) {
//			printf("\t\t%s\n", cdata[0].devspec.pload[i]->key[j]);
        }
    }
    printf("______________________________________________________________________________________________\n");
    if (jumpto!=-1) break;
}
}
}

//Prints the blocks of text in outputBuffer[] side by side in the terminal.
void printBufferLeftToRight(int blockCount, const char *indent, const char *spacer) {//(The first line of each block must be the longest for good formatting.)
    int i,j,nullchars=0;
    int cursor[3],lineLength[2];
    for (i=0;i<blockCount;i++) {
        cursor[i] = 0;
        if (i!=0) lineLength[i-1] = 0;
    }
    char c;
    while(nullchars<blockCount) {
        i=0;
        printf("%s", indent);
        while(i<blockCount) {
            j=lineLength[i];
            while(1) {
                if ((c=outputBuffer[i][cursor[i]]) != '\n') {
                    if (c != '\0') {
                        putchar(c);
                        j--;
                        cursor[i]++;
                        nullchars=0;
                    } else {
                        nullchars++;
                        break;
                    }
                } else {
                    cursor[i]++;
                    nullchars=0;
                    break;
                }
            }
            i++;
            if (i<blockCount) {
                if (lineLength[i-1]==0) {
                    lineLength[i-1]=cursor[i-1]-1;
                    if (c!='\n') lineLength[i-1]++;
                } else for(j=1;j>0;j--) putchar(' ');
                printf("%s", spacer);
            }
        }
        putchar('\n');
    }
    //NULLify the buffers:
    outputBuffer[0][0] = outputBuffer[1][0] = outputBuffer[2][0] = '\0';
}

int rvector2buffer (char *buffer, rvector *rv, const char *name){
    int n;
    for(n=sprintf(buffer, "%s:", name);n<37;n++) buffer[n]=' ';
    buffer[(n++)] = '\n';
    n += sprintf(buffer+n, "%11.5g, %11.5g, %11.5g\n", rv->col[0], rv->col[1], rv->col[2]);
    return(n);
}

int gvector2buffer (char *buffer, gvector *gv, const char *name){
    int n;
    for(n=sprintf(buffer, "%s,", name);n<50;n++) buffer[n]=' ';
    buffer[(n++)] = '\n';
    n += sprintf(buffer+n, "lat: %11.5g, lon: %11.5g, h: %11.5g\n", gv->lat, gv->lon, gv->h);
    return(n);
}

int quat2buffer (char *buffer, quaternion *qu, const char *name){
    int n;
    for(n=sprintf(buffer, "%s:", name);n<37;n++) buffer[n]=' ';
    buffer[(n++)] = '\n';
    n += sprintf(buffer+n, "%11.5g, %11.5g, %11.5g w:%11.5g\n", qu->d.x, qu->d.y, qu->d.z, qu->w);
    return(n);
}

int rmatrix2buffer (char *buffer, rmatrix *rm, const char *name){
    int i,n;
    for(n=sprintf(buffer, "%s:", name);n<37;n++) buffer[n]=' ';
    buffer[(n++)] = '\n';
    for (i=0;i<3;i++) n += sprintf(buffer+n, "%11.5g, %11.5g, %11.5g\n", rm->row[i].col[0], rm->row[i].col[1], rm->row[i].col[2]);
    return(n);
}
