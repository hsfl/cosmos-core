#include "configCosmos.h"
#include "agent/agent.h"
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
//#include <mqueue.h>
#include <semaphore.h>
//#include <sys/vfs.h>
#include "jsonlib.h"
#include "stringlib.h"
#include "timelib.h"
#include "gs232b_lib.h"
#include "prkx2su_lib.h"
#include "convertlib.h"
#include "elapsedtime.h"

int32_t request_debug(char *req, char* response, CosmosAgent *);
int32_t request_get_offset(char *req, char* response, CosmosAgent *);
int32_t request_get_state(char *req, char* response, CosmosAgent *);
int32_t request_set_azel(char *req, char* response, CosmosAgent *);
int32_t request_track_azel(char *req, char* response, CosmosAgent *);
int32_t request_get_azel(char *req, char* response, CosmosAgent *);
int32_t request_jog(char *req, char* response, CosmosAgent *);
int32_t request_get_horizon(char *req, char* response, CosmosAgent *);
int32_t request_enable(char *req, char* response, CosmosAgent *);
int32_t request_disable(char *req, char* response, CosmosAgent *);
int32_t request_stop(char *req, char* response, CosmosAgent *);
int32_t request_pause(char *req, char* response, CosmosAgent *);
int32_t request_set_offset(char *req, char* response, CosmosAgent *);

int32_t connect_antenna();

float gsmin = RADOF(10.);
//char tlename[20];
std::string nodename;
std::string agentname;
std::string antdevice;
size_t devindex;
size_t antindex = 9999;
antstruc target;
antstruc current;
bool antconnected = false;
bool antenabled = false;
bool debug;

// Here are internally provided functions
int json_init();
//int myinit();
//int load_gs_info(char *file);
int load_tle_info(char *file);

// Here are variables for internal use
static std::vector<tlestruc> tle;
int32_t numlines, bestn;
CosmosAgent *agent;

struct azelstruc
{
	float az;
	float el;
};
azelstruc antennaoffset = {0., 0.};

LsFit trackaz(5, 2, .001);
LsFit trackel(5, 2, .001);
bool trackflag = false;

int main(int argc, char *argv[])
{
	int iretn;
	//  rvector topo;
	//  locstruc loc;
	//  int32_t n;
	//  float taz, tel;


	// Initialization stuff

	switch (argc)
	{
	case 3:
		nodename = argv[1];
		agentname = argv[2];
		break;
	default:
        printf("Usage: agent_antenna node antenna");
		exit (1);
		break;
	}

    if (!(agent = new CosmosAgent(NetworkType::UDP, nodename, agentname)))
	{
		printf("Error %d: Setting up Agent antenna\n",JSON_ERROR_NOJMAP);
		exit (JSON_ERROR_NOJMAP);
	}

	// Add requests
    if ((iretn=agent->add_request("track_azel",request_track_azel,"track_azel", "Supply next azimuth and elevation for tracking.")))
		exit (iretn);
    if ((iretn=agent->add_request("debug",request_debug,"debug", "Toggle debug messages.")))
		exit (iretn);
    if ((iretn=agent->add_request("get_offset",request_get_offset,"get_offset", "Return a report on the offset of the agent.")))
		exit (iretn);
    if ((iretn=agent->add_request("get_state",request_get_state,"get_state", "Return a report on the state of the agent.")))
		exit (iretn);
    if ((iretn=agent->add_request("jog",request_jog,"jog {degrees aziumth} {degrees elevation}", "Jog the antenna azimuth and elevation in degrees.")))
		exit (iretn);
    if ((iretn=agent->add_request("get_horizon",request_get_horizon,"get_horizon", "Return the antennas minimu elevation in degrees.")))
		exit (iretn);
    if ((iretn=agent->add_request("get_azel",request_get_azel,"get_azel", "Return the antenna azimuth and elevation in degrees.")))
		exit (iretn);
    if ((iretn=agent->add_request("set_azel",request_set_azel,"set_azel aaa.a eee.e", "Set the antenna azimuth and elevation in degrees.")))
		exit (iretn);
    if ((iretn=agent->add_request("enable",request_enable,"enable", "Enable antenna motion.")))
		exit (iretn);
    if ((iretn=agent->add_request("disable",request_disable,"disable", "Disable antenna motion.")))
		exit (iretn);
    if ((iretn=agent->add_request("pause",request_pause,"pause", "Stop where you are and make it your new target.")))
		exit (iretn);
    if ((iretn=agent->add_request("stop",request_stop,"stop", "Stop where you are, make it your new target AND disable antenna motion.")))
		exit (iretn);
    if ((iretn=agent->add_request("set_offset",request_set_offset,"set_offset aaa.a eee.e", "Set the antenna azimuth and elevation correction in degrees.")))
		exit (iretn);

	// Look for named antenna so we can use the right one
    for (size_t i=0; i<agent->cinfo->pdata.devspec.ant_cnt; ++i)
	{
        std::string aname = agent->cinfo->pdata.piece[agent->cinfo->pdata.devspec.ant[i]->gen.pidx].name;
		if (aname == argv[2])
            //      if (!strcmp(argv[2], agent->cinfo->pdata.piece[agent->cinfo->pdata.devspec.ant[i]->gen.pidx].name))
		{
            devindex = agent->cinfo->pdata.devspec.ant[i]->gen.cidx;
			antindex = i;
			break;
		}
	}

	if (antindex == 9999)
	{
        std::cout<<"Exiting " << agentname << " for Node: " << nodename << " no antenna found." << std::endl;
        agent->shutdown();
		exit (1);
	}

	// Set SOH string
	char sohstring[200];
	sprintf(sohstring, "{\"device_ant_temp_%03lu\",\"device_ant_align_%03lu\",\"device_ant_azim_%03lu\",\"device_ant_elev_%03lu\"}", antindex, antindex, antindex, antindex);
    agent->set_sohstring(sohstring);

    antdevice = agent->cinfo->pdata.port[agent->cinfo->pdata.device[devindex].all.gen.portidx].name;

	// Connect to antenna and set sensitivity;
	iretn = connect_antenna();
    switch (agent->cinfo->pdata.device[devindex].all.gen.model)
	{
	case DEVICE_MODEL_GS232B:
		gs232b_set_sensitivity(RADOF(1.));
		break;
	case DEVICE_MODEL_PRKX2SU:
		prkx2su_set_sensitivity(RADOF(.2));
		break;
	}

	ElapsedTime et;

	// Start performing the body of the agent
    while(agent->running())
	{
		if (antconnected)
		{
			// Find most recent position
            switch (agent->cinfo->pdata.device[devindex].all.gen.model)
			{
            case DEVICE_MODEL_LOOPBACK:
                iretn = 0;
                break;
			case DEVICE_MODEL_GS232B:
				iretn = gs232b_get_az_el(current.azim, current.elev);
				break;
			case DEVICE_MODEL_PRKX2SU:
				iretn = prkx2su_get_az_el(current.azim, current.elev);
				break;
			}
			if (iretn < 0)
			{
				antconnected = false;
			}
			else
			{
                agent->cinfo->pdata.device[devindex].ant.azim = current.azim -  antennaoffset.az;
                agent->cinfo->pdata.device[devindex].ant.elev = current.elev -  antennaoffset.el;
				if (antenabled)
				{
					if (trackflag)
					{
						double utc = currentmjd();
						if (utc - trackaz.lastx() < trackaz.lastx() - trackaz.firstx())
						{
							current.azim = trackaz.eval(utc);
							current.elev = trackel.eval(utc);
						}
						else
						{
							trackflag = false;
						}
					}
					else
					{
						current.azim = target.azim;
						current.elev = target.elev;
					}
                    switch (agent->cinfo->pdata.device[devindex].all.gen.model)
					{
					case DEVICE_MODEL_GS232B:
						iretn = gs232b_goto(current.azim + antennaoffset.az, current.elev + antennaoffset.el);
						break;
					case DEVICE_MODEL_PRKX2SU:
						iretn = prkx2su_goto(current.azim + antennaoffset.az, current.elev + antennaoffset.el);
						break;
					}
					if (debug)
					{
						printf("%f: goto %f %f [%d]\n", et.lap(), DEGOF(current.azim + antennaoffset.az), DEGOF(current.elev + antennaoffset.el), iretn);
					}
					if (iretn < 0)
					{
						antconnected = false;
					}
				}
			}
			COSMOS_SLEEP(.1);
		}
		else
		{
			if (debug)
			{
				printf("%f: Connect Antenna\n", et.lap());
			}
			connect_antenna();
			COSMOS_SLEEP(.1);
		}
	}

    agent->shutdown();
}

int32_t request_get_state(char *req, char* response, CosmosAgent *)
{
    sprintf(response,"[%.6f] Cx: %u En: %u Target: %6.1f %6.1f (%6.1f %6.1f) Actual: %6.1f %6.1f Offset: %6.1f %6.1f",
            currentmjd(),
            antconnected,
            antenabled,
            DEGOF(current.azim),
            DEGOF(current.elev),
            DEGOF(current.azim-agent->cinfo->pdata.device[devindex].ant.azim),
            DEGOF(current.elev-agent->cinfo->pdata.device[devindex].ant.elev),
            DEGOF(agent->cinfo->pdata.device[devindex].ant.azim+antennaoffset.az),
            DEGOF(agent->cinfo->pdata.device[devindex].ant.elev+antennaoffset.el),
            DEGOF(antennaoffset.az), DEGOF(antennaoffset.el));
	return (0);
}

int32_t request_stop(char *req, char* response, CosmosAgent *)
{

    target = agent->cinfo->pdata.device[devindex].ant;
	antenabled = false;
    switch (agent->cinfo->pdata.device[devindex].all.gen.model)
	{
	case DEVICE_MODEL_GS232B:
		gs232b_stop();
		break;
	case DEVICE_MODEL_PRKX2SU:
		prkx2su_stop(PRKX2SU_AXIS_AZ);
		prkx2su_stop(PRKX2SU_AXIS_EL);
		break;
	}

	return 0;
}

int32_t request_pause(char *req, char* response, CosmosAgent *)
{

    target = agent->cinfo->pdata.device[devindex].ant;

	return 0;
}

int32_t request_set_offset(char *req, char* response, CosmosAgent *)
{
	float targetaz;
	float targetel;

	sscanf(req,"%*s %f %f",&targetaz, &targetel);
	antennaoffset.az = RADOF(targetaz);
	antennaoffset.el = RADOF(targetel);
	return (0);
}

int32_t request_get_offset(char *req, char* response, CosmosAgent *)
{
	float az = antennaoffset.az;
	float el = antennaoffset.el;
	sprintf(response,"%f %f",DEGOF(az), DEGOF(el));
	return (0);
}

int32_t request_set_azel(char *req, char* response, CosmosAgent *)
{
	float targetaz;
	float targetel;

	sscanf(req,"%*s %f %f",&targetaz, &targetel);
	target.azim = RADOF(targetaz);
	target.elev = RADOF(targetel);
	trackflag = false;
	return (0);
}

int32_t request_get_azel(char *req, char* response, CosmosAgent *)
{
    double az = agent->cinfo->pdata.device[devindex].ant.azim;
    double el = agent->cinfo->pdata.device[devindex].ant.elev;
	sprintf(response,"%f %f",DEGOF(az), DEGOF(el));
	return (0);
}

int32_t request_jog(char *req, char* response, CosmosAgent *)
{
	float az, el;
	sscanf(req,"%*s %f %f", &az, &el);
	target.azim += RADOF(az);
	target.elev += RADOF(el);
	return (0);
}

int32_t request_get_horizon(char *req, char* response, CosmosAgent *)
{
	sscanf(req,"%*s %f",&gsmin);
	return (0);
}

int32_t request_enable(char *req, char* response, CosmosAgent *)
{
	antenabled = true;
	return 0;
}

int32_t request_disable(char *req, char* response, CosmosAgent *)
{
	antenabled = false;
	return 0;
}

int32_t connect_antenna()
{
	int32_t iretn;
	antconnected = false;

    switch (agent->cinfo->pdata.device[devindex].all.gen.model)
	{
    case DEVICE_MODEL_LOOPBACK:
        antconnected = true;
        break;
	case DEVICE_MODEL_GS232B:
		iretn = gs232b_connect(antdevice);

		// Initialize values if we are connected
		if (iretn == 0)
		{
            iretn = gs232b_get_az_el(agent->cinfo->pdata.device[devindex].ant.azim, agent->cinfo->pdata.device[devindex].ant.elev);
			if (iretn >= 0)
			{
                target.azim = agent->cinfo->pdata.device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->pdata.device[devindex].ant.elev - antennaoffset.el;
				antconnected = true;
			}
		}
		break;
	case DEVICE_MODEL_PRKX2SU:
		iretn = prkx2su_connect(antdevice);

		// Initialize values if we are connected
		if (iretn == 0)
		{
            iretn = prkx2su_get_az_el(agent->cinfo->pdata.device[devindex].ant.azim, agent->cinfo->pdata.device[devindex].ant.elev);
			if (iretn >= 0)
			{
                target.azim = agent->cinfo->pdata.device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->pdata.device[devindex].ant.elev - antennaoffset.el;
				antconnected = true;
			}
		}
		break;
	}

	return 0;

}

int32_t request_debug(char *req, char* response, CosmosAgent *)
{
    if (debug)
	{
		debug = false;
}
    else
    {
        debug = true;
	}

	return 0;
}

int32_t request_track_azel(char *req, char* response, CosmosAgent *)
{
	float az;
	float el;
	double utc;

	sscanf(req,"%*s %lf %f %f", &utc, &az, &el);
	trackaz.update(utc, RADOF(az));
	trackel.update(utc, RADOF(el));
	trackflag = true;
	return 0;
}
