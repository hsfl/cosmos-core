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
#include <cstdio>
#include <cstring>
#include <cmath>

#include "mathlib.h"
#include "cosmos-defs.h"
#include "jsondef.h"

rvector vert[200][10000];
rvector vcenter[200], tvert[20], con;
rvector vend[5], line[5];
rvector cross[4];
rvector norm, normal[200][3300];
double area[200][3300];
int tri[200][3300][3], face[20];
rvector tvec[3];
uint32_t ecount[2], fcount[200];
struct facearray
{
	uint32_t fcount;
	struct facestruc
	{
		rvector center;
		rvector normal;
		double area;
		uint32_t tcount;
		int tri[500][3];
		uint32_t pcount;
		rvector pnt[350];
		double angle[350];
	} face[100];
} faces[200];

int main(int argc, char *argv[])
{
	char data[500];
	FILE *fd, *pd, *cd, *dd=nullptr;
	uint32_t i, j, k, m, count, vcount[200], tcount[200], centeri, pcount, pindex, concount, tindex;
	uint32_t findex, cindex, ctype, dindex[999];
	double tarea, volume, radius, dim=.003, centerv, height, angle[2];
	double tangle, pangle, nangle, bestangle;
	uint32_t ci[4], bestci[10]={0,0,0,0,0,0,0,0,0,0};
	rvector tpnt;


	con = rv_zero();
	concount = 0;
	pcount = argc - 1;
	for (pindex=0; pindex<pcount; pindex++)
	{
		printf("Input: %s\n",argv[pindex+1]);
		fd = fopen(argv[pindex+1],"r");

		do
		{
			fgets(data,500,fd);
			if (!strncmp("point",data,5))
				break;
		} while (1);

		vcount[pindex] = 0;
		vcenter[pindex] = rv_zero();
		do
		{
			count = fscanf(fd," %lf %lf %lf,",&vert[pindex][vcount[pindex]].col[0],&vert[pindex][vcount[pindex]].col[1],&vert[pindex][vcount[pindex]].col[2]);
			if (count)
			{
				vcenter[pindex] = rv_add(vcenter[pindex],vert[pindex][vcount[pindex]]);
				con = rv_add(con,vert[pindex][vcount[pindex]]);
				vcount[pindex]++;
				concount++;
			}
		} while (count);

		vcenter[pindex] = rv_smult(1./vcount[pindex],vcenter[pindex]);

		for (i=2; i<strlen(argv[pindex+1]); i++)
		{
			if (argv[pindex+1][i] == '.')
			{
				argv[pindex+1][i] = 0;
				break;
			}
		}

		do
		{
			fgets(data,500,fd);
			if (!strncmp("coordIndex",data,5))
				break;
		} while (1);

		tcount[pindex] = 0;
		do
		{
			count = fscanf(fd," %d, %d, %d, -1,",&tri[pindex][tcount[pindex]][0],&tri[pindex][tcount[pindex]][1],&tri[pindex][tcount[pindex]][2]);
			if (count)
			{
				tcount[pindex]++;
			}
		} while (count);

		fclose(fd);
	}

	con = rv_smult(1./concount,con);

	for (pindex=0; pindex<pcount; pindex++)
	{
		printf("Process: %s\n",argv[pindex+1]);
		faces[pindex].fcount = 0;
		vcenter[pindex] = rv_sub(vcenter[pindex],con);
		for (i=0; i<vcount[pindex]; i++)
		{
			vert[pindex][i] = rv_sub(vert[pindex][i],con);
		}
		for (tindex=0; tindex<tcount[pindex]; tindex++)
		{
			vend[0] = rv_sub(vert[pindex][tri[pindex][tindex][1]],vert[pindex][tri[pindex][tindex][0]]);
			vend[1] = rv_sub(vert[pindex][tri[pindex][tindex][2]],vert[pindex][tri[pindex][tindex][1]]);
			vend[2] = rv_sub(vert[pindex][tri[pindex][tindex][0]],vert[pindex][tri[pindex][tindex][2]]);
			normal[pindex][tindex] = rv_cross(vend[0],vend[1]);
			area[pindex][tindex] = length_rv(normal[pindex][tindex])/2.;
			normal[pindex][tindex] = rv_normal(normal[pindex][tindex]);
			for (i=0; i<faces[pindex].fcount; i++)
			{
				pangle = sep_rv(faces[pindex].face[i].normal,normal[pindex][tindex]);
				if (fabs(pangle) < .00001)
				{
					faces[pindex].face[i].area += area[pindex][tindex];
					faces[pindex].face[i].tri[faces[pindex].face[i].tcount][0] = tri[pindex][tindex][0];
					faces[pindex].face[i].tri[faces[pindex].face[i].tcount][1] = tri[pindex][tindex][1];
					faces[pindex].face[i].tri[faces[pindex].face[i].tcount++][2] = tri[pindex][tindex][2];
					break;
				}
			}
			if (i == faces[pindex].fcount)
			{
				faces[pindex].face[i].normal = normal[pindex][tindex];
				faces[pindex].face[i].area = area[pindex][tindex];
				faces[pindex].face[i].tcount  = 0;
				faces[pindex].face[i].tri[faces[pindex].face[i].tcount][0] = tri[pindex][tindex][0];
				faces[pindex].face[i].tri[faces[pindex].face[i].tcount][1] = tri[pindex][tindex][1];
				faces[pindex].face[i].tri[faces[pindex].face[i].tcount++][2] = tri[pindex][tindex][2];
				faces[pindex].fcount++;
			}
		}
		for (i=0; i<faces[pindex].fcount; i++)
		{
			faces[pindex].face[i].pcount = 0;
			faces[pindex].face[i].center = rv_zero();
			for (j=0; j<faces[pindex].face[i].tcount; j++)
			{
				for (k=0; k<3; k++)
				{
					for (m=0; m<faces[pindex].face[i].pcount; m++)
					{
						if (equal_rv(faces[pindex].face[i].pnt[m],vert[pindex][faces[pindex].face[i].tri[j][k]]))
							break;
					}
					if (m == faces[pindex].face[i].pcount)
					{
						faces[pindex].face[i].pnt[m] = vert[pindex][faces[pindex].face[i].tri[j][k]];
						faces[pindex].face[i].center = rv_add(faces[pindex].face[i].center,faces[pindex].face[i].pnt[m]);
						faces[pindex].face[i].pcount++;
					}
				}
			}
			faces[pindex].face[i].center = rv_smult(1./faces[pindex].face[i].pcount,faces[pindex].face[i].center);
		}
	}

	cindex = -1;
	for (i=0; i<999; ++i)
		dindex[i] = -1;
	pd = fopen("part.txt","w");
	cd = fopen("comp.txt","w");
	for (pindex=0; pindex<pcount; pindex++)
	{
		printf("Output: %s\n",argv[pindex+1]);
		ctype = argv[pindex+1][4] + 10*argv[pindex+1][3] + 100*argv[pindex+1][2] - 111*'0';
		if (ctype != DEVICE_TYPE_NONE)
		{
			++cindex;
			++dindex[ctype];
			fprintf(cd,"%d\t%d\t%d\t%d\t%d\t%d\t%d\n",cindex,ctype,dindex[ctype],pindex,0,0,0);
			fflush(cd);
			switch(ctype)
			{
			case DEVICE_TYPE_ANT:
				dd = fopen("ant.txt","a+");
				break;
			case DEVICE_TYPE_BATT:
				dd = fopen("batt.txt","a+");
				break;
			case DEVICE_TYPE_CPU:
				dd = fopen("cpu.txt","a+");
				break;
			case DEVICE_TYPE_GPS:
				dd = fopen("gps.txt","a+");
				break;
			case DEVICE_TYPE_HTR:
				dd = fopen("htr.txt","a+");
				break;
			case DEVICE_TYPE_IMU:
				dd = fopen("imu.txt","a+");
				break;
			case DEVICE_TYPE_MOTR:
				dd = fopen("motr.txt","a+");
				break;
			case DEVICE_TYPE_MTR:
				dd = fopen("mtr.txt","a+");
				break;
			case DEVICE_TYPE_PLOAD:
				dd = fopen("pload.txt","a+");
				break;
			case DEVICE_TYPE_PROP:
				dd = fopen("prop.txt","a+");
				break;
			case DEVICE_TYPE_ROT:
				dd = fopen("rot.txt","a+");
				break;
			case DEVICE_TYPE_RW:
				dd = fopen("rw.txt","a+");
				break;
			case DEVICE_TYPE_RXR:
				dd = fopen("rxr.txt","a+");
				break;
			case DEVICE_TYPE_SSEN:
				dd = fopen("ssen.txt","a+");
				break;
			case DEVICE_TYPE_STRG:
				dd = fopen("strg.txt","a+");
				break;
			case DEVICE_TYPE_SWCH:
				dd = fopen("swch.txt","a+");
				break;
			case DEVICE_TYPE_TCV:
				dd = fopen("tcv.txt","a+");
				break;
			case DEVICE_TYPE_THST:
				dd = fopen("thst.txt","a+");
				break;
			case DEVICE_TYPE_TSEN:
				dd = fopen("tsen.txt","a+");
				break;
			case DEVICE_TYPE_TXR:
				dd = fopen("txr.txt","a+");
				break;
			}
			if (dd != nullptr)
			{
				fprintf(dd,"%d\t%d\n",dindex[ctype],cindex);
				fclose(dd);
			}

		}

		fprintf(pd,"%d\t%s\t%c\t%d\t",pindex,&argv[pindex+1][6],argv[pindex+1][0],(ctype==DEVICE_TYPE_NONE?999:cindex));
		switch (argv[pindex+1][0])
		{
		// Panels
		case '0':
		case '1':
			findex = 0;
			tarea = 0;
			for (i=0; i<faces[pindex].fcount; i++)
			{
				if (sep_rv(faces[pindex].face[i].normal,faces[pindex].face[i].center) > DPI2 && faces[pindex].face[i].area > tarea)
				{
					tarea = faces[pindex].face[i].area;
					findex = i;
				}
			}

			// findex has index of desired face
			dim = 2.*length_rv(rv_sub(faces[pindex].face[findex].center,vcenter[pindex]));

			line[0] = rv_sub(faces[pindex].face[findex].pnt[0],faces[pindex].face[findex].center);
			for (i=1; i < faces[pindex].face[findex].pcount; i++)
			{
				line[1] = rv_sub(faces[pindex].face[findex].pnt[i],faces[pindex].face[findex].center);
				line[2] = rv_cross(line[0],line[1]);
				if (sep_rv(faces[pindex].face[findex].normal,line[2]) > DPI2)
					faces[pindex].face[findex].angle[i] = D2PI-sep_rv(line[0],line[1]);
				else
					faces[pindex].face[findex].angle[i] = sep_rv(line[0],line[1]);
				for (k=i; k>0; k--)
				{
					if (faces[pindex].face[findex].angle[k] < faces[pindex].face[findex].angle[k-1])
					{
						tangle = faces[pindex].face[findex].angle[k];
						tpnt = faces[pindex].face[findex].pnt[k];
						faces[pindex].face[findex].angle[k] = faces[pindex].face[findex].angle[k-1];
						faces[pindex].face[findex].pnt[k] = faces[pindex].face[findex].pnt[k-1];
						faces[pindex].face[findex].angle[k-1] = tangle;
						faces[pindex].face[findex].pnt[k-1] = tpnt;
					}
				}
			}

			volume = 2700 * faces[pindex].face[findex].area * dim;
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t%f\t0\t0\t%d",volume,dim,faces[pindex].face[i].area,faces[pindex].face[findex].pcount);
			for (i=0; i<faces[pindex].face[findex].pcount; i++)
			{
				fprintf(pd,"\t%f\t%f\t%f",faces[pindex].face[findex].pnt[i].col[0],faces[pindex].face[findex].pnt[i].col[1],faces[pindex].face[findex].pnt[i].col[2]);
			}
			fprintf(pd,"\n");
			break;
			// Box
		case '2':
			dim = .003;
			vend[0] = rv_sub(vert[pindex][2],vert[pindex][0]);
			vend[1] = rv_sub(vert[pindex][6],vert[pindex][0]);
			vend[2] = rv_sub(vert[pindex][1],vert[pindex][0]);
			area[pindex][0] = 2. * length_rv(vend[0]) * length_rv(vend[1]);
			area[pindex][0] += 2. * length_rv(vend[1]) * length_rv(vend[2]);
			area[pindex][0] += 2. * length_rv(vend[2]) * length_rv(vend[0]);
			volume = (2700.* area[pindex][0] *dim);
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t%f\t0\t0\t8",volume,dim,area[pindex][0]);

			tvert[0] = rv_smult(.25,rv_add(vert[pindex][0],rv_add(vert[pindex][2],rv_add(vert[pindex][4],vert[pindex][6]))));
			vend[2] = rv_sub(tvert[0],vcenter[pindex]);
			vend[0] = rv_sub(vert[pindex][2],vert[pindex][0]);
			vend[1] = rv_sub(vert[pindex][4],vert[pindex][0]);
			norm = rv_cross(vend[0],vend[1]);
			nangle = sep_rv(norm,tvert[0]);
			if (nangle > DPI2)
			{
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][0].col[0],vert[pindex][0].col[1],vert[pindex][0].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][6].col[0],vert[pindex][6].col[1],vert[pindex][6].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][4].col[0],vert[pindex][4].col[1],vert[pindex][4].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][2].col[0],vert[pindex][2].col[1],vert[pindex][2].col[2]);
			}
			else
			{
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][0].col[0],vert[pindex][0].col[1],vert[pindex][0].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][2].col[0],vert[pindex][2].col[1],vert[pindex][2].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][4].col[0],vert[pindex][4].col[1],vert[pindex][4].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][6].col[0],vert[pindex][6].col[1],vert[pindex][6].col[2]);
			}

			tvert[0] = rv_smult(.25,rv_add(vert[pindex][1],rv_add(vert[pindex][3],rv_add(vert[pindex][5],vert[pindex][7]))));
			vend[2] = rv_sub(tvert[0],vcenter[pindex]);
			vend[0] = rv_sub(vert[pindex][3],vert[pindex][1]);
			vend[1] = rv_sub(vert[pindex][5],vert[pindex][1]);
			norm = rv_cross(vend[0],vend[1]);
			nangle = sep_rv(norm,tvert[0]);
			if (nangle > DPI2)
			{
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][1].col[0],vert[pindex][1].col[1],vert[pindex][1].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][3].col[0],vert[pindex][3].col[1],vert[pindex][3].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][5].col[0],vert[pindex][5].col[1],vert[pindex][5].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][7].col[0],vert[pindex][7].col[1],vert[pindex][7].col[2]);
			}
			else
			{
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][1].col[0],vert[pindex][1].col[1],vert[pindex][1].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][7].col[0],vert[pindex][7].col[1],vert[pindex][7].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][5].col[0],vert[pindex][5].col[1],vert[pindex][5].col[2]);
				fprintf(pd,"\t%f\t%f\t%f",vert[pindex][3].col[0],vert[pindex][3].col[1],vert[pindex][3].col[2]);
			}
			fprintf(pd,"\n");
			break;
			// Cylinder
		case '3':
			dim = .003;
			bestangle = 0.;
			for (ci[0]=1; ci[0] < vcount[pindex]; ci[0]++)
			{
				line[0] = rv_sub(vert[pindex][ci[0]],vert[pindex][0]);
				for (ci[1]=ci[0]+1;ci[1]<vcount[pindex]; ci[1]++)
				{
					line[1] = rv_sub(vert[pindex][ci[1]],vert[pindex][0]);
					cross[0] = rv_cross(line[0],line[1]);
					for (ci[2]=ci[1]+1;ci[2]<vcount[pindex]; ci[2]++)
					{
						line[2] = rv_sub(vert[pindex][ci[2]],vert[pindex][0]);
						cross[1] = rv_cross(line[0],line[2]);
						angle[0] = fabs(DPI2-sep_rv(cross[0],cross[1]));
						for (ci[3]=ci[2]+1;ci[3]<vcount[pindex]; ci[3]++)
						{
							line[3] = rv_sub(vert[pindex][ci[3]],vert[pindex][0]);
							cross[2] = rv_cross(line[0],line[3]);
							angle[1] = fabs(DPI2-sep_rv(cross[0],cross[2]));
							if (angle[0]+angle[1] > bestangle)
							{
								bestangle = angle[0]+angle[1];
								bestci[0] = ci[0];
								bestci[1] = ci[1];
								bestci[2] = ci[2];
								bestci[3] = ci[3];
							}
						}
					}
				}
			}
			vend[0] = vert[pindex][0];
			ecount[0] = 1;
			vend[1] = rv_zero();
			ecount[1] = 0;
			for (i=1; i<vcount[pindex]; i++)
			{
				line[0] = rv_sub(vert[pindex][bestci[0]],vert[pindex][i]);
				line[1] = rv_sub(vert[pindex][bestci[1]],vert[pindex][i]);
				cross[0] = rv_cross(line[0],line[1]);
				line[2] = rv_sub(vert[pindex][bestci[2]],vert[pindex][i]);
				cross[1] = rv_cross(line[0],line[2]);
				angle[0] = fabs(DPI2-sep_rv(cross[0],cross[1]));
				line[3] = rv_sub(vert[pindex][bestci[3]],vert[pindex][i]);
				cross[2] = rv_cross(line[0],line[3]);
				angle[1] = fabs(DPI2-sep_rv(cross[0],cross[2]));
				if ((angle[0]+angle[1]) > DPI-.001 || i == bestci[0] || i == bestci[1] || i == bestci[2] || i == bestci[3])
				{
					vend[0] = rv_add(vend[0],vert[pindex][i]);
					ecount[0]++;
				}
				else
				{
					vend[1] = rv_add(vend[1],vert[pindex][i]);
					vend[2] = vert[pindex][i];
					ecount[1]++;
				}
			}
			vend[0] = rv_smult(1./ecount[0],vend[0]);
			vend[1] = rv_smult(1./ecount[1],vend[1]);
			radius = length_rv(rv_sub(vend[0],vert[pindex][0]));
			area[pindex][0] = length_rv(rv_sub(vend[0],vend[1]));
			area[pindex][0] *= radius * radius;
			area[pindex][0] *= DPI;
			volume = 2700. * area[pindex][0] * dim;
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t%f\t0\t0\t3",volume,dim,area[pindex][0]);
			fprintf(pd,"\t%f\t%f\t%f",vend[0].col[0],vend[0].col[1],vend[0].col[2]);
			fprintf(pd,"\t%f\t%f\t%f",vend[1].col[0],vend[1].col[1],vend[1].col[2]);
			fprintf(pd,"\t%f\t%f\t%f",vend[2].col[0],vend[2].col[1],vend[2].col[2]);
			fprintf(pd,"\n");
			break;
			// Sphere
		case '4':
			dim = .003;
			radius = length_rv(rv_sub(vert[pindex][0],vcenter[pindex]));
			area[pindex][0] = 4.*DPI*radius*radius;
			volume = area[pindex][0] * (2700.*dim);
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t%f\t0\t0\t2",volume,dim,area[pindex][0]);
			fprintf(pd,"\t%f\t%f\t%f",vcenter[pindex].col[0],vcenter[pindex].col[1],vcenter[pindex].col[2]);
			fprintf(pd,"\t%f\t%f\t%f",vert[pindex][0].col[0],vert[pindex][0].col[1],vert[pindex][0].col[2]);
			fprintf(pd,"\n");
			break;
			// Dimensionless
		case '5':
			dim = volume = area[pindex][0] = 0.;
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t0\t0\t0\t2",volume,dim);
			fprintf(pd,"\t%f\t%f\t%f",vcenter[pindex].col[0],vcenter[pindex].col[1],vcenter[pindex].col[2]);
			fprintf(pd,"\n");
			break;
			// Cone
		case '6':
			dim = .003;
			// Find central vert
			centeri = 0;
			centerv = 0.;
			for (i=0; i<vcount[pindex]; i++)
			{
				radius = length_rv(rv_sub(vert[pindex][i],vcenter[pindex]));
				if (radius > centerv)
				{
					centeri = i;
					centerv = radius;
				}
			}
			// Calculate center of open end
			vcenter[pindex] = rv_zero();
			for (i=0; i<vcount[pindex]; i++)
			{
				if (i != centeri)
					vcenter[pindex] = rv_add(vcenter[pindex],vert[pindex][i]);
			}
			vcenter[pindex] = rv_smult(1./(vcount[pindex]-1),vcenter[pindex]);
			// Calculate and print final values
			radius = length_rv(rv_sub(vcenter[pindex],vert[pindex][0]));
			height = length_rv(rv_sub(vert[pindex][centeri],vert[pindex][0]));
			area[pindex][0] = DPI * radius * sqrt(radius*radius+height*height);
			volume = area[pindex][0] * (2700.*dim);
			fprintf(pd,"%f\t0.87\t0.9\t%f\t800\t237\t%f\t0\t0\t3",volume,dim,area[pindex][0]);
			fprintf(pd,"\t%f\t%f\t%f",vert[pindex][centeri].col[0],vert[pindex][centeri].col[1],vert[pindex][centeri].col[2]);
			fprintf(pd,"\t%f\t%f\t%f",vcenter[pindex].col[0],vcenter[pindex].col[1],vcenter[pindex].col[2]);
			fprintf(pd,"\t%f\t%f\t%f",vert[pindex][0].col[0],vert[pindex][0].col[1],vert[pindex][0].col[2]);
			fprintf(pd,"\n");
			fflush(pd);
			break;
		}
	}
	fclose(pd);
	fclose(cd);
}
