#include "configCosmos.h"
#include "vmt35_lib.h"
#include "vn100_lib.h"
#include "agentlib.h"

cosmosstruc *cdata;
string node = "hiakasat";

int main(int argc, char *argv[])
{
	int32_t iretn;
	vmt35_handle vmt35handle;
	double moment=0.;
	char rod='x';

	switch (argc)
	{
	case 3:
		if (!(cdata=agent_setup_client(SOCKET_TYPE_UDP, node.c_str(), 1000)))
		{
			printf("Couldn't establish client for node %s\n", node.c_str());
			exit (-1);
		}

		if ((iretn=vmt35_connect(cdata->port[cdata->devspec.tcu[0]->gen.portidx].name, &vmt35handle)) < 0)
		{
			printf("Couldn't connect to VMT35\n");
			exit (1);
		}
		printf("Connected to %s\n", cdata->port[cdata->devspec.tcu[0]->gen.portidx].name);

		rod = argv[1][0];
		moment = atof(argv[2]);
		break;
	case 4:
		if ((iretn=vmt35_connect(argv[1], &vmt35handle)) < 0)
		{
			printf("Couldn't connect to VMT35\n");
			exit (1);
		}

		rod = argv[2][2];
		moment = atof(argv[3]);
		break;
	}

	vmt35_enable(&vmt35handle);
	
	float ppolys[3][7], npolys[3][7];
	for (uint16_t i=0; i<3; ++i)
        {
                for (uint16_t j=0; j<7; ++j)
                {
                        npolys[i][j] = cdata->devspec.mtr[i]->npoly[j];
                        ppolys[i][j] = cdata->devspec.mtr[i]->ppoly[j];
                }
        }

	rvector momvec = {{0.,0.,0.}};
	switch (rod)
	{
	case 'x':
		momvec.col[0] = moment;
		break;
	case 'y':
		momvec.col[1] = moment;
		break;
	case 'z':
		momvec.col[2] = moment;
		break;
	}

	if ((iretn=vmt35_set_moments(&vmt35handle, momvec, npolys, ppolys)) < 0)
	{
		printf("Error: %d\n", iretn);
	}
	else
	{
		printf("Diff: %d Currents: %f %f %f\n", iretn, vmt35handle.telem.dac[0]/1e6, vmt35handle.telem.dac[1]/1e6, vmt35handle.telem.dac[2]/1e6);
	}
	vmt35_disconnect(&vmt35handle);

}
