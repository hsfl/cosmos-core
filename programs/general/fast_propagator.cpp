#include "physicslib.h"
#include "mathlib.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include <sys/stat.h>
#include <iostream>
#include <iomanip>

gj_handle gjh;
cosmosstruc* cdata;
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
	double mjdend = -1.;
	double dt = 1.;
	double logperiod = 1.;
	double logstride = 3600./86400.;
	int32_t iretn;
	//	bool master_timer = false;

	switch (argc)
	{
	case 5:
		mjdend = atof(argv[4]);
	case 4:
		logperiod = atof(argv[3]);
	case 3:
		mjdstart = atof(argv[2]);
	case 2:
		node = argv[1];
		break;
	default:
		cout << "Usage: fast_propagator nodename [mjdstart|0 [logperiod [mjdend]]]" << endl;
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
		loc_update(&cdata->node.loc);
		iloc = cdata->node.loc;
//		iloc.pos.eci = cdata->node.loc.pos.eci;
//		iloc.att.icrf = cdata->node.loc.att.icrf;
//		iloc.utc = cdata->node.loc.pos.eci.utc;

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

//	gj_kernel gjk = gauss_jackson_kernel(gjh, order,step);
//	gj_instance3d gji = gauss_jackson_instance(&gjk, 3, acceleration);
//	gauss_jackson_preset(&gji);
//	gauss_jackson_extrapolate(&gji, mjdnow);

	gauss_jackson_init_eci(gjh, order, mode, step, iloc.utc ,iloc.pos.eci, iloc.att.icrf, *cdata);
	gauss_jackson_propagate(gjh, *cdata, mjdnow);
	pos_clear(iloc);
	iloc.pos.eci = cdata->node.loc.pos.eci;
	iloc.att.icrf = cdata->node.loc.att.icrf;
	iloc.utc = cdata->node.loc.pos.eci.utc;
	gauss_jackson_init_eci(gjh, order, mode, dt, iloc.utc ,iloc.pos.eci, iloc.att.icrf, *cdata);
	mjdnow = currentmjd(cdata->node.utcoffset);

	vector <gj_handle> tgjh(cdata->target.size());
	vector <cosmosstruc *> tcdata(cdata->target.size());

	for (uint16_t i=0; i<cdata->target.size(); ++i)
	{
		tcdata[i] = json_create();
		gauss_jackson_init_eci(tgjh[i], order, 0, dt, cdata->target[i].loc.utc, cdata->target[i].loc.pos.eci, cdata->target[i].loc.att.icrf, *tcdata[i]);
	}

	string sohstring = json_list_of_soh(cdata);
	agent_set_sohstring(cdata, sohstring.c_str());
	vector<jsonentry*> logtable;
	json_table_of_list(logtable, sohstring.c_str(), cdata);

	double logdate = floor(mjdnow/logstride)*logstride;

	while (mjdend < 0. || mjdend-mjdstart > 0)
	{
		mjdnow += logperiod/86400.;
		gauss_jackson_propagate(gjh, *cdata, mjdnow);
		if (cdata->node.loc.utc > cdata->node.utc)
		{
			cdata->node.utc = cdata->node.loc.utc;
		}

		double dtemp;
		if ((dtemp=floor(mjdnow/logstride)*logstride) > logdate)
		{
			logdate = dtemp;
			log_move(cdata->node.name, "soh");
		}

		for (uint16_t i=0; i<cdata->target.size(); ++i)
		{
			gauss_jackson_propagate(tgjh[i], *tcdata[i], mjdnow);
		}
		update_target(cdata);
		calc_events(eventdict, cdata, events);
		for (uint32_t k=0; k<events.size(); ++k)
		{
			memcpy(&cdata->event[0].s,&events[k],sizeof(shorteventstruc));
			strcpy(cdata->event[0].l.condition,cdata->emap[events[k].handle.hash][events[k].handle.index].text);
			log_write(cdata->node.name,DATA_LOG_TYPE_EVENT,logdate, json_of_event(mainjstring, cdata));
		}

		if (cdata->node.utc != 0. && sohstring.size())
		{
			log_write(cdata->node.name,DATA_LOG_TYPE_SOH, logdate, json_of_table(mainjstring,  logtable, cdata));
		}
		//		agent_post(cdata, AGENT_MESSAGE_SOH,json_of_table(mainjstring,  cdata->agent[0].sohtable, cdata));
	}
	agent_shutdown_server(cdata);
}

//void acceleration(double time, double *pos, double *acc, int32_t axes)
//{
//	locstruc loc;
//	physicsstruc physics;

//	loc.pos.eci.utc = time;
//	for (uint16_t i=0; i<axes; ++i)
//	{
//		loc.pos.eci.s.col[i] = pos[i];
//	}
//	++loc.pos.eci.pass;
//	pos_eci(&loc);

//	pos_accel(physics, &loc);

//	for (uint16_t i=0; i<axes; ++i)
//	{
//		acc[i] = loc.pos.eci.a.col[i];
//	}
//}
