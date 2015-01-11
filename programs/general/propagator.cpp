#include "physicslib.h"
#include "mathlib.h"
#include "jsonlib.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "datalib.h"

#include <sys/stat.h>
#include <iostream>
#include <iomanip>

cosmosstruc* cdata;
gj_handle gjh;
vector<shorteventstruc> eventdict;
vector<shorteventstruc> events;
string mainjstring;

void endline(){
    cout << endl;
}

int main(int argc, char* argv[])
{
	string node;
	int32_t order = 6;
    int32_t mode = 1; // attitude mode (0 - propagate?, 1-LVLH, ...)
	double mjdnow;
	double mjdstart = -1.;
	double dt = .1;
	int32_t iretn;
//	bool master_timer = false;

	switch (argc)
	{
	case 3:
		mjdstart = atof(argv[2]);
	case 2:
		node = argv[1];
		break;
	default:
        cout << "Usage: propogator nodename [mjd|0]" << endl;
		exit (-1);
		break;
	}

	if (!(cdata = agent_setup_client(SOCKET_TYPE_BROADCAST, node.c_str(), 1000)))
	{
		printf("Failed to setup client for node %s: %d\n", node.c_str(), AGENT_ERROR_JSON_CREATE);
		exit (AGENT_ERROR_JSON_CREATE);
	}

	cdata->physics.mode = mode;
	json_clone(cdata);

	load_dictionary(eventdict, cdata, (char *)"events.dict");

	// Set initial state
	locstruc iloc;

	pos_clear(iloc);

	struct stat fstat;
	FILE* fdes;
	string fname = get_nodedir((node.c_str()));
	fname += "/state.ini";
	if ((iretn=stat(fname.c_str(), &fstat)) == 0 && (fdes=fopen(fname.c_str(),"r")) != NULL)
	{
		char* ibuf = (char *)calloc(1,fstat.st_size+1);
		size_t nbytes = fread(ibuf, 1, fstat.st_size, fdes);
//		fgets(ibuf,fstat.st_size,fdes);
		if (nbytes)
		{
			json_parse(ibuf,cdata);
		}
		free(ibuf);
		iloc.pos.eci = cdata->node.loc.pos.eci;
		iloc.att.icrf = cdata->node.loc.att.icrf;
		iloc.utc = cdata->node.loc.pos.eci.utc;

//        print_vector("Initial State Vector Position: ", iloc.pos.eci.s.col[0], iloc.pos.eci.s.col[1], iloc.pos.eci.s.col[2], "km");
        //cout << "Initial State Vector Pos: [" << iloc.pos.eci.s.col[0] << ", " << iloc.pos.eci.s.col[1] <<  ", " << iloc.pos.eci.s.col[2] << "] km " << endl;
        cout << "Initial State Vector Vel: [" << iloc.pos.eci.v.col[0] << ", " << iloc.pos.eci.v.col[1] <<  ", " << iloc.pos.eci.v.col[2] << "] km" << endl;
        cout << "Initial MJD: " << setprecision(10) << iloc.utc << endl;
	}
	else
	{
		printf("Unable to open state.ini\n");
		exit (-1);
	}

#define POLLBUFSIZE 20000
	string pollbuf;

	iretn = agent_poll(cdata, pollbuf, AGENT_MESSAGE_ALL,1);
	switch (iretn)
	{
	case AGENT_MESSAGE_SOH:
	case AGENT_MESSAGE_BEAT:
		{
			string tbuf = json_convert_string(json_extract_namedobject(pollbuf, "agent_name"));
			if (!tbuf.empty() && tbuf == "physics")
			{
				tbuf = json_convert_string(json_extract_namedobject(pollbuf, "node_utcoffset"));
				if (!tbuf.empty())
				{
					cdata->node.utcoffset = atof(tbuf.c_str());
					printf("slave utcoffset: %f\n", cdata->node.utcoffset);
				}
			}
			else
			{
				if (mjdstart == -1.)
				{
					cdata->node.utcoffset = cdata->node.loc.utc - currentmjd(0.);
				}
				else if (mjdstart == 0.)
				{
					cdata->node.utcoffset = 0.;
				}
				else
				{
					cdata->node.utcoffset = mjdstart - currentmjd(0.);
				}
                //printf("master utcoffset: %f\n", cdata->node.utcoffset);
                cout << "master utcoffset: " << setprecision(5) << cdata->node.utcoffset << endl;
//				master_timer = true;
			}
			break;
		}
	default:
		if (mjdstart == -1.)
		{
			cdata->node.utcoffset = cdata->node.loc.utc - currentmjd(0.);
		}
		else if (mjdstart == 0.)
		{
			cdata->node.utcoffset = 0.;
		}
		else
		{
			cdata->node.utcoffset = mjdstart - currentmjd(0.);
		}
        //printf("master utcoffset: %f\n", cdata->node.utcoffset);
        cout << "master utcoffset: " << cdata->node.utcoffset << endl;
//		master_timer = true;
		break;
	}

	mjdnow =  currentmjd(cdata->node.utcoffset);
	double sohtimer = mjdnow;

	if (mjdnow < iloc.utc)
	{
		gauss_jackson_init_eci(gjh, order ,mode, -dt, iloc.utc,iloc.pos.eci, iloc.att.icrf, *cdata);

        //printf("Initialize backwards %f days\n", (cdata->node.loc.utc-mjdnow));
        cout << "Initialize backwards " << cdata->node.loc.utc-mjdnow << "days" << endl;

		gauss_jackson_propagate(gjh, *cdata, mjdnow);
		iloc.utc = cdata->node.loc.utc;
		iloc.pos.eci = cdata->node.loc.pos.eci;
		iloc.att.icrf = cdata->node.loc.att.icrf;
	}
	
	double step = 8.64 * (mjdnow-iloc.utc);
	if (step > 60.)
	{
		step = 60.;
	}
	if (step < .1)
	{
		step = .1;
	}

    //printf("Initialize forwards %f days, steps of %f\n", (mjdnow-iloc.utc), step);
    cout << "Initialize forwards " << (mjdnow-iloc.utc) << " days, steps of " << step << endl;

	gauss_jackson_init_eci(gjh, order, mode, step, iloc.utc ,iloc.pos.eci, iloc.att.icrf, *cdata);
	gauss_jackson_propagate(gjh, *cdata, mjdnow);
	pos_clear(iloc);
	iloc.pos.eci = cdata->node.loc.pos.eci;
	iloc.att.icrf = cdata->node.loc.att.icrf;
	iloc.utc = cdata->node.loc.pos.eci.utc;
	gauss_jackson_init_eci(gjh, order, mode, dt, iloc.utc ,iloc.pos.eci, iloc.att.icrf, *cdata);
	mjdnow = currentmjd(cdata->node.utcoffset);

	if (!(cdata = agent_setup_server(cdata, (char *)"physics", .1, 0, AGENTMAXBUFFER, AGENT_SINGLE)))
	{
		printf("Failed to setup server for node %s: %d\n", node.c_str(), AGENT_ERROR_JSON_CREATE);
		exit (AGENT_ERROR_JSON_CREATE);
	}

	string sohstring = json_list_of_soh(cdata);
	agent_set_sohstring(cdata, sohstring.c_str());

	while (agent_running(cdata))
	{
		sohtimer += 1./86400.;
		mjdnow = currentmjd(cdata->node.utcoffset);
		gauss_jackson_propagate(gjh, *cdata, mjdnow);

		update_target(cdata);
		calc_events(eventdict, cdata, events);
		agent_post(cdata, AGENT_MESSAGE_SOH,json_of_table(mainjstring,  cdata->agent[0].sohtable, cdata));
		double dsleep = 1000000. * 86400.*(sohtimer - mjdnow);
		if (dsleep > 0.)
		{
			COSMOS_USLEEP(dsleep);
		}
	}
	agent_shutdown_server(cdata);
}
