#include "physicslib.h"
#include "mathlib.h"
#include "jsonlib.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "socketlib.h"
#include <semaphore.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <sys/types.h>

char *output;

gj_handle gjh;
cosmosstruc *cdata;
jstring reqjstring={0,0,0};
jstring mainjstring={0,0,0};
double logperiod=30, newlogperiod=30;

char logstring[AGENTMAXBUFFER-20];
int logcount;
char node[200]="";

int main(int argc, char *argv[])
{
	uint32_t order;
	double dt, mjdnow, mjdaccel, lmjd, cmjd;
	int rflag, iretn, mode;
	FILE *fdes;
	char fname[200];
	char *ibuf;
	struct stat fstat;
	tlestruc tline;

	dt = .1;
	mjdaccel = 1.;
	mode = 0;
	switch (argc)
	{
	case 5:
		mjdaccel = atof(argv[4]);
	case 4:
		dt = atof(argv[3]);
	case 3:
		mode = atol(argv[2]);
	case 2:
		strcpy(node,argv[1]);
		break;
	default:
		printf("Usage: agent_physics node [attitude_mode [dt [mjdaccel]]]\n");
		exit(-1);
	}

	rflag = 1;

	// Initialization stuff

	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,node,(char *)"physics",2.,0,AGENTMAXBUFFER)))
	{
		printf("Failed to setup server: %d\n",AGENT_ERROR_JSON_CREATE);
		exit (AGENT_ERROR_JSON_CREATE);
	}

	order = 8;

	sprintf(fname,"%s/state.ini",get_nodedir(node).c_str());

	if ((iretn=stat(fname,&fstat)) == 0 && (fdes=fopen(fname,"r")) != NULL)
	{
		ibuf = (char *)calloc(1,fstat.st_size+1);
		fgets(ibuf,fstat.st_size,fdes);
		json_parse(ibuf,cdata);
		free(ibuf);
		mjdnow = cdata->node.loc.pos.eci.utc;
	}
	else
	{
		sprintf(fname,"%s/tle.ini",get_nodedir(node).c_str());
		if ((iretn=stat(fname,&fstat)) == 0 && (iretn=load_lines(fname,cdata->tle)) > 0)
		{
			tline = get_line(0, cdata->tle);
			mjdnow = currentmjd(0.);
			lines2eci(mjdnow, cdata->tle, &cdata->node.loc.pos.eci);
		}
		else
		{
			mjdnow = 55593.416667827405;
			cdata->node.loc.pos.eci.s = cdata->node.loc.pos.eci.v = cdata->node.loc.pos.eci.a = rv_zero();
			cdata->node.loc.pos.eci.s.col[0] = -1354152.3069408732;
			cdata->node.loc.pos.eci.s.col[1] = 6794509.033329579;
			cdata->node.loc.pos.eci.s.col[2] = 905.69207709436682;
			cdata->node.loc.pos.eci.v.col[0] = 983.48186257069437;
			cdata->node.loc.pos.eci.v.col[1] = 195.00552418716413;
			cdata->node.loc.pos.eci.v.col[2] = 7518.5307318052819;
			cdata->node.loc.pos.eci.a.col[0] = 1.625564;
			cdata->node.loc.pos.eci.a.col[1] = -8.155423;
			cdata->node.loc.pos.eci.a.col[2] = -0.000244;
		}
	}

	cdata->node.loc.utc = mjdnow;
	++cdata->node.loc.pos.eci.pass;
	cdata->physics.moi.col[0] = 1.;
	cdata->physics.moi.col[1] = 3.;
	cdata->physics.moi.col[2] = 5.;
	cdata->node.loc.att.icrf.v = rv_smult(.017453293,rv_unity());
	cdata->node.loc.att.icrf.s = q_eye();
	cdata->node.loc.att.icrf.a = rv_zero();
	pos_eci(&cdata->node.loc);

/*
	cdata->node.loc.att.lvlh.v = rv_zero();
	cdata->node.loc.att.lvlh.s = q_eye();
	cdata->node.loc.att.lvlh.a = rv_zero();
	++cdata->node.loc.att.lvlh.pass;
	att_lvlh(&cdata->node.loc);
	loc_update(&cdata->node.loc);
*/

	gauss_jackson_init_eci(gjh, order, mode, dt, mjdnow,cdata->node.loc.pos.eci, cdata->node.loc.att.icrf, *cdata);
	mjdnow = cdata->node.loc.utc;

	cdata->node.utcoffset = mjdnow - currentmjd(0);
	lmjd = currentmjd(0);
	mjdaccel -= 1.;

	do
	{
		mjdnow = currentmjd(0.);
		gauss_jackson_propagate(gjh, *cdata, mjdnow + cdata->node.utcoffset);

		/*
		if (cdata->node.state == 1)
		{
			ipos.s.col[0] = -6605464.484;
			ipos.s.col[1] = -2233537.982;
			ipos.s.col[2] = 270957.434;
			ipos.v.col[0] = -603.693;
			ipos.v.col[1] = 877.7741;
			ipos.v.col[2] = -7482.349;
			orbit_init_eci(mode,mjdnow,1./dt,ipos,cdata);
		}
		*/
		if (cdata->node.state == 2)
			rflag = 0;

		COSMOS_USLEEP(1);
		cmjd = currentmjd(0);
		cdata->node.utcoffset += mjdaccel * (cmjd-lmjd);
		lmjd = cmjd;
	} while (agent_running(cdata) && rflag);
}

//sem_wait(&data_sem);

char* request_utc(char* request, char* output)
{
	strcpy(output,json_of_utc(&reqjstring,cdata));
	return (output);
}

char* request_soh(char* request, char* output)
{
	strcpy(output,json_of_soh(&reqjstring,cdata));
	//strcpy(output,json_of_beacon(&reqjstring,cdata));
	return (output);
}

char* request_beacon(char* request, char* output)
{
	strcpy(output,json_of_beacon(&reqjstring,cdata));
	return (output);
}

char* request_statevec(char* request, char* output)
{
	strcpy(output,json_of_statevec(&reqjstring,cdata));
	return (output);
}

char* request_set_bus(char* request, char* output)
{
	int j, k;

	sscanf(request,"set_bus %d %d",&j, &k);
	if (k)
	{
		cdata->devspec.bus[j]->flag |= DEVICE_FLAG_ON;
	}
	else
	{
		cdata->devspec.bus[j]->flag &= ~DEVICE_FLAG_ON;
	}
	return (output);
}   

char* request_imu(char* request, char* output)
{
	int j;

	sscanf(request,"imu %d",&j);
	strcpy(output,json_of_imu(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}   

char* request_get_rw(char* request, char* output)
{
	int j;

	sscanf(request,"get_rw %d",&j);
	strcpy(output,json_of_rw(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}   

char* request_set_rw_moment(char* request, char* output)
{
	int j;
	double value[3];

	sscanf(request,"set_rw_moment %d %lf %lf %lf",&j,&value[0],&value[1],&value[2]);

	cdata->devspec.rw[j]->mom.col[0] = value[0];
	cdata->devspec.rw[j]->mom.col[1] = value[1];
	cdata->devspec.rw[j]->mom.col[2] = value[2];
	strcpy(output,json_of_rw(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_rw_omega(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"%*s %d %lf",&j,&value);
	cdata->devspec.rw[j]->omg = value;
	strcpy(output,json_of_rw(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_rw_alpha(char* request, char* output)
{   
	int j;
	double value;

	sscanf(request,"%*s %d %lf",&j,&value);
	cdata->devspec.rw[j]->alp = value;
	strcpy(output,json_of_rw(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_get_mtr(char* request, char* output)
{   
	int j;

	sscanf(request,"get_mtr %d",&j);
	strcpy(output,json_of_mtr(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_moment(char* request, char* output)
{
	int j;
	double value[3];

	sscanf(request,"set_mtr_moment %d %lf %lf %lf",&j,&value[0],&value[1],&value[2]);

	//cdata->devspec.mtr[j]->mom.col[0] = value[0];
	//cdata->devspec.mtr[j]->mom.col[1] = value[1];
	//cdata->devspec.mtr[j]->mom.col[2] = value[2];
	strcpy(output,json_of_mtr(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_current(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"set_mtr_current %d %lf",&j,&value);

	cdata->device[cdata->devspec.mtr[j]->cidx].gen.amp = value;
	strcpy(output,json_of_mtr(&reqjstring,j,cdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_field(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"set_mtr_field %d %lf",&j,&value);

	cdata->device[cdata->devspec.mtr[j]->cidx].gen.amp = value*(4.838e-3+value*(-3.958e-5+value*3.053e-6));
	strcpy(output,json_of_rw(&reqjstring,j,cdata));
	return (output);
}

void soh_cpu()
{
	FILE *fp;

	fp = fopen("/proc/meminfo","r");
	fscanf(fp,"MemTotal: %f kB\n",&cdata->devspec.cpu[0]->mem);
	fclose(fp);
	fp = fopen("/proc/loadavg","r");
	fscanf(fp,"%f",&cdata->devspec.cpu[0]->load);
	fclose(fp);
}
