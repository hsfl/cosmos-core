//#include <stdlib.h>
//#include <stdio.h>
//#include <sys/types.h>
//#include <cstring>

#include "configCosmos.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "stringlib.h"
#include "timelib.h"
#include "physicslib.h"
#include "nodelib.h"
#include "datalib.h"
#include "zlib/zlib.h"
#include <sys/stat.h>
//#include "beacon.h"

#define SOH_STATE_OPEN 0
#define NTYPE AGENT_TYPE_UDP

int32_t request_reopen(char* request, char* output, void *cdata);
int32_t request_set_logperiod(char* request, char* output, void *cdata);
int32_t request_set_logstring(char* request, char* output, void *cdata);
int32_t request_get_logstring(char* request, char* output, void *cdata);
int32_t request_set_logstride(char* request, char* output, void *cdata);

//char message[AGENTMAXBUFFER];
jstring jjstring={0,0,0};
jstring myjstring={0,0,0};

int ntype = AGENT_TYPE_UDP;
int waitsec = 5;

void collect_data_loop();
thread cdthread;

int myagent();

string logstring;
vector<jsonentry*> logtable;
double logdate=0.;

vector<shorteventstruc> eventdict;
vector<shorteventstruc> events;

// variables to map to JSON
int32_t newlogperiod = 10, logperiod = 0;
double newlogstride = 900. / 86400.;
double logstride = 0.;
int pid;
int state = 0;
double cmjd;
timestruc systime;
cosmosstruc *cdata;

beatstruc iscbeat;
string node = "hiakasat";
char response[300];

int main(int argc, char *argv[])
{
	int32_t iretn;

	if (argc != 2)
	{
		printf("Usage: agent_soh nodename\n");
		exit (1);
	}

	// Establish the command channel and heartbeat
	if (!(cdata = agent_setup_server(ntype,argv[1],(char *)"soh",1.,0,AGENTMAXBUFFER)))
	{
		cout<<"agent_soh: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}

	cdata->node.utc = 0.;
	json_clone (cdata) ;
	cdata->agent[0].aprd = .5;


	if ((iretn=agent_add_request(cdata, (char *)"reopen",request_reopen)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_logperiod",request_set_logperiod)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_logstring",request_set_logstring)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"get_logstring",request_get_logstring)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_logstride",request_set_logstride)))
		exit (iretn);

	// Create default logstring
	logstring = json_list_of_soh(cdata);
	printf("logstring: %s\n", logstring.c_str());
	json_table_of_list(logtable, logstring.c_str(), cdata);
	//	agent_set_sohstring(cdata, logstring.c_str());

	load_dictionary(eventdict, cdata, (char *)"events.dict");

	// Start SOH collection thread
	cdthread = thread(collect_data_loop);
	//	printf("Started data collection thread\n");



	iretn = myagent();

}

int myagent()
{
	int sleept;
	double lmjd, dmjd, dtemp;
	//double fmjd;
	double nextmjd;

    //Beacon beacon;
    //string beaconMessage;

    //double mjd_start = currentmjd();

#if !defined(COSMOS_WIN_OS)
	FILE *fp;
	struct statfs fsbuf;
#endif

	state = 1;
	//	fmjd = 0.;

	printf("Starting main thread\n");
	nextmjd = currentmjd();
	lmjd = currentmjd();

	do
	{
		nextmjd += cdata->agent[0].aprd/86400.;
		dmjd = (cmjd-lmjd)*86400.;

		if (newlogperiod != logperiod )
		{
			logperiod = newlogperiod;
			logdate = cdata->node.utc;
			log_move(cdata->node.name, "soh");
		}

		cmjd = currentmjd();
		if (cdata->node.utc != 0.)
		{

#if !defined(COSMOS_WIN_OS)
        fp = fopen("/proc/meminfo","r");
        fscanf(fp,"MemTotal: %f kB\nMemFree: %f",
               &cdata->devspec.cpu[0]->maxmem,
                &cdata->devspec.cpu[0]->mem);
        fclose(fp);

        // get load average
        fp = fopen("/proc/loadavg","r");
        fscanf(fp,"%f",&cdata->devspec.cpu[0]->load);
        fclose(fp);

        // get percetage of disk used
        statfs("/",&fsbuf);
        cdata->devspec.cpu[0]->disk = 100.0 * (double) (fsbuf.f_blocks - fsbuf.f_bfree) / (double) (fsbuf.f_blocks - fsbuf.f_bfree + fsbuf.f_bavail);
        //cdata->devspec.cpu[0]->disk = fsbuf.f_bavail * fsbuf.f_frsize; // f_blocks; //f_bfree
        //diskfree = fsbuf.f_bfree;

        // get number of cpu reboots
        fp = fopen("/flight_software/cosmosroot/nodes/hiakasat/boot.count","r");
        fscanf(fp,"%u",&cdata->devspec.cpu[0]->boot_count);
        fclose(fp);

#endif

			if (newlogstride != logstride )
			{
				logstride = newlogstride;
				logdate = currentmjd(0.);
				log_move(cdata->node.name, "soh");
			}

			if ((dtemp=floor(cmjd/logstride)*logstride) > logdate)
			{
				logdate = dtemp;
				log_move(cdata->node.name, "soh");
			}

			loc_update(&cdata->node.loc);
			update_target(cdata);
			agent_post(cdata, AGENT_MESSAGE_SOH, json_of_table(&myjstring, logtable, cdata));
			calc_events(eventdict, cdata, events);
			for (uint32_t k=0; k<events.size(); ++k)
			{
				memcpy(&cdata->event[0].s,&events[k],sizeof(shorteventstruc));
				strcpy(cdata->event[0].l.condition,cdata->emap[events[k].handle.hash][events[k].handle.index].text);
				log_write(cdata->node.name,DATA_LOG_TYPE_EVENT,logdate, json_of_event(&jjstring,cdata));
			}
		}
		if (dmjd-logperiod > -logperiod/20.)
		{
			lmjd = cmjd;
			if (cdata->node.utc != 0. && logstring.size())
			{
				log_write(cdata->node.name,DATA_LOG_TYPE_SOH, logdate, json_of_table(&jjstring, logtable, cdata));
			}
		}

//        // send beacon every 10 second
//        if ((currentmjd() - mjd_start)*86400 > 10){
//            beacon.createBeacon(cdata);
//            cout << beacon.message << "*" << beacon.message.size() << endl;


//            // Shutdown radio link, if on
//            //if (radio_state)
//            //{
//                if (iscbeat.utc == 0)
//                {
//                    iscbeat = agent_find_server(cdata, node, "isc", 5.);
//                }
//                if (iscbeat.utc != 0)
//                {
//                    string requestString = "beacon_data_update ";
//                    requestString += beacon.message;
//                    agent_send_request(cdata, iscbeat, requestString.c_str(), response, 300, 2.);
//                }

//                // Take down tunnel interface
//             //   if_takedown("sband");
//            //}


//            mjd_start = currentmjd();
//        }

		sleept = (int)((nextmjd-currentmjd())*86400000000.);
		if (sleept < 0) sleept = 0;
		COSMOS_USLEEP(sleept);
	} while (agent_running(cdata));

	//	sprintf(tname,"%s/outgoing/%s",get_cnodedir(cdata->node.name),data_name(cdata->node.name,fmjd,(char *)"telemetry"));
	//	rename(data_base_path(cdata->node.name,fmjd,(char *)"telemetry"),tname);
	cdthread.join();
	agent_shutdown_server(cdata);

	return 0;
}

int32_t request_reopen(char* request, char* output, void *cdata)
{
	logdate = ((cosmosstruc *)cdata)->node.loc.utc;
	log_move(((cosmosstruc *)cdata)->node.name, "soh");
	return 0;
}

int32_t request_set_logperiod(char* request, char* output, void *cdata)
{
	sscanf(request,"set_logperiod %d",&newlogperiod);
	return 0;
}

int32_t request_set_logstring(char* request, char* output, void *cdata)
{
	logstring = &request[strlen("set_logstring")+1];
	json_table_of_list(logtable, logstring.c_str(), (cosmosstruc*)cdata);
	return 0;
}

int32_t request_get_logstring(char* request, char* output, void *cdata)
{
	strcpy(output, logstring.c_str());
	return 0;
}

int32_t request_set_logstride(char* request, char* output, void *cdata)
{
	sscanf(request,"set_logstride %lf",&newlogstride);
	return 0;
}

void collect_data_loop()
{
	int nbytes;
	string message;

	while (agent_running(cdata))
	{
		// Collect new data
		if((nbytes=agent_poll(cdata, message, AGENT_MESSAGE_BEAT,0)))
		{
			string tstring;
			if ((tstring=json_convert_string(json_extract_namedobject(message.c_str(), "agent_node"))) != cdata->node.name)
			{
				continue;
			}
			cdata[1].node = cdata[0].node;
			cdata[1].device = cdata[0].device;
			json_parse(message,&cdata[1]);
			cdata[0].node  = cdata[1].node ;
			cdata[0].device  = cdata[1].device ;
			loc_update(&cdata->node.loc);
			if (cdata->node.loc.utc > cdata->node.utc)
			{
				cdata->node.utc = cdata->node.loc.utc;
			}
			for (auto device: cdata->device)
			{
				if (device.gen.utc > cdata->node.utc)
				{
					cdata->node.utc = device.gen.utc;
				}
			}
			cdata->node.utc = currentmjd();
			//			update_target(cdata);
		}
	}
	return;
}

