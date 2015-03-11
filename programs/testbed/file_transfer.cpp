#include "configCosmos.h"
#include <sys/stat.h>
//#include <sys/types.h>
#include <stdio.h>
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif

#include "agentlib.h"
#include "jsonlib.h"

int main(int argc, char *argv[])
{
char direc[20] = "telemetry";
char yearname[5], dayname[4];
char fromname[150], toname[150];
int i;
DIR *cdir;
struct dirent *nextdir;
beatstruc hbeat;
cosmosstruc *cdata;

cdata = json_create();

if ((i = agent_get_server(cdata, NULL,(char *)"soh",2,&hbeat)) > 0)
	{
	i = agent_send_request(cdata, hbeat,(char *)"reopen",fromname,150,1);
	COSMOS_USLEEP(2000000);
	}

cdir = opendir(direc);

while ((nextdir=readdir(cdir)) != NULL)
	{
	if (nextdir->d_name[0] == '.')
		continue;
	for (i=0; i<20; i++)
		{
		if (nextdir->d_name[i] == '.')
			break;
		}
	strncpy(yearname,&nextdir->d_name[i+1],4);
	yearname[4] = 0;
	strncpy(dayname,&nextdir->d_name[i+5],3);
	dayname[3] = 0;
	sprintf(fromname,"%s/%s",direc,nextdir->d_name);
#if defined(COSMOS_WIN_OS)
	mkdir("data");
#else
	mkdir("data",00775);
#endif
	sprintf(toname,"data/%s",yearname);
#if defined(COSMOS_WIN_OS)
	mkdir(toname);
#else
	mkdir(toname,00775);
#endif
	sprintf(toname,"data/%s/%s",yearname,dayname);
#if defined(COSMOS_WIN_OS)
	mkdir(toname);
#else
	mkdir(toname,00775);
#endif
	sprintf(toname,"data/%s/%s/%s",yearname,dayname,nextdir->d_name);
	rename(fromname,toname);
	}
}
