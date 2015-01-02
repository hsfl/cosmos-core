// code from testbed/blank_agent.cpp
#include "configCosmos.h"

//#ifdef COSMOS_MAC_OS
//#include <sys/param.h>
//#include <sys/mount.h>
//#else
//#include <sys/vfs.h>
//#endif // COSMOS_MAC_OS

#include "agentlib.h"
#include "sinclair_lib.h"
#include "jsonlib.h"

#include <cstdio>
#include <iostream>
#include <iomanip>

//#define DOPRINT   // Print star tracker values to screen
#define MAXBUFFERSIZE 1000

char agentname[COSMOS_MAX_NAME] = "stt";
char nodename[COSMOS_MAX_NAME];

// Set up SOH string
char sohstring[]="{\"device_stt_att_000\",\"device_stt_omega_000\",\"device_stt_retcode_000\",\"node_loc_att_icrf\"}";

int ntype = SOCKET_TYPE_UDP;
beatstruc rwbeat, pbeat;
sinclair_state stth;
char request[100], result[400], bufin[500];

int32_t request_get_att(char *request, char* response, void *cdata);
int32_t request_get_omg(char *request, char* response, void *cdata);
int32_t request_soh(char *request, char* response, void *cdata);
void sendComboCommand(uint32_t i);

// Here are some variables we will map to JSON names
double mjd, initialmjd;
cosmosstruc *cdata;
bool simulated=false;

int main(int argc, char *argv[])
{
	int32_t iretn;

	switch (argc)
	{
	case 3:
		simulated = true;
	case 2:
		strcpy(nodename,argv[1]);
		break;
	default:
		printf("Usage: stt_agent node {simulated}\n");
		exit(-99);
	}


	// Initialize the Agent
	if (!(cdata = agent_setup_server(SOCKET_TYPE_BROADCAST,(char *)nodename,agentname,1.,0,MAXBUFFERSIZE))) {
		cout<<"agent_rw: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}
	printf("STT Server Agent is set up.\n");

	// Initialization stuff - start real unless indicated
	cdata->devspec.stt[0]->att.w =   -.1;
	cdata->devspec.stt[0]->att.d.x = -.1;
	cdata->devspec.stt[0]->att.d.y = -.1;
	cdata->devspec.stt[0]->att.d.z = -.1;

	if (simulated)
	{
		//Attempt to contact physics agent
		if ((iretn=agent_get_server(cdata,nodename,(char *)"physics",3,&pbeat)) < 0)
		{
			printf("Failed to find physics engine: %d\n",iretn);
			exit (iretn);
		}
	}
	else
	{
		// Attempt to initialize hardware.
		if ((iretn = sinclair_stt_connect(cdata->port[cdata->devspec.stt[0]->portidx].name,(uint8_t)0x11,(uint8_t)0x0E, &stth)) < 0)
		{
			//if ((&stth = sinclair_stt_connect("/dev/ttyUSB0",(uint8_t)0x11,(uint8_t)0x0E)) < 0)
			printf("Connection Error: Star tracker not found for [%s]:%d\nAgent will continue\n\n",cdata->node.name, iretn);
		}
		else
		{
			printf("Connected to Star tracker .\n");
		}
	}


	if ((iretn=agent_add_request(cdata, (char *)"get_att",request_get_att)))
		exit (iretn);

	if ((iretn=agent_add_request(cdata, (char *)"get_omg",request_get_omg)))
		exit (iretn);

	if ((iretn=agent_add_request(cdata, (char *)"soh",request_soh)))
		exit (iretn);

	printf("Requests added\n");



	// Set what values will be sent in State Of Health
	agent_set_sohstring(cdata, sohstring);

	// Start performing the body of the agent
	while(agent_running(cdata))
	{
		// Get next set of readings
		if (simulated)
		{
			if ((iretn=agent_send_request(cdata,pbeat,(char *)"getvalue {\"device_stt_att_000\",\"device_stt_omega_000\",\"node_loc_att_icrf\"}",bufin,500,3)) >= 0)
			{
				json_parse(bufin,cdata);
			}

		}
		else
		{
			for (uint32_t i=0; i<cdata->devspec.stt_cnt; ++i)
			{
				cdata->node.loc.utc = currentmjd( cdata->node.utcoffset);
				sinclair_stt_result_operational result;
				sinclair_stt_combo(&stth, &result);
			}
		}

		COSMOS_USLEEP(100000);
	}


	for (uint32_t i=0; i<cdata->devspec.stt_cnt; ++i) {
		if (!(cdata->devspec.stt[0]->flag&DEVICE_FLAG_SIMULATED)) {
			// Close the star tracker on exit.
			sinclair_disconnect(&stth);
		}
	}
	printf("\n\nDisconnecting...\n\n      GoodBye\n\n");

	return 0;
}



int32_t request_soh(char *request, char* response, void *)
{
	jstring rjstring={0,0,0};
//	strcpy(response,json_of_list(&rjstring,sohstring,cdata));
	strcpy(response,json_of_table(&rjstring, cdata->agent[0].sohtable, cdata));
	return 0;
}



int32_t request_get_att(char *request, char* response, void *)
{
	sprintf(response,"mjd=%.15g  w=%.6g x=%.6g y=%.6g z=%.6g", mjd,
			cdata->devspec.stt[0]->att.w,
			cdata->devspec.stt[0]->att.d.x,
			cdata->devspec.stt[0]->att.d.y,
			cdata->devspec.stt[0]->att.d.z);

	return 0;
}

int32_t request_get_omg(char *request, char* response, void *)
{
	sprintf(response,"mjd=%.15g  col0=%.6g col1=%.6g col2=%.6g", mjd,
			cdata->devspec.stt[0]->omega.col[0],
			cdata->devspec.stt[0]->omega.col[1],
			cdata->devspec.stt[0]->omega.col[2]);

	return 0;
}


/*
void sendComboCommand(uint32_t stt_num)
{
	int i;
	char buf[4096];
	//	int size;
	uint8_t command[4];
	double AttQ[4];
	double VelAng[3];
	int8_t nspOffset = -2;  // The nsp_get_data seems to be off by 2 bytes.  If that routine changes, this may be changed
	quaternion sttTimesAlign;
	rvector velTimesAlign;
	//    int16_t DetTemp;
	uint16_t ProcTemp;
	uint32_t statusBits;
	//    uint16_t darkOffsets[16];
	//    uint8_t VddCoreSetPoint;
	//    uint8_t MPUSetPoint;
	char ReturnCode;
	//    double EpochTime;

	// Send COMBINATION command 0x00001E0B
	command[0] = 0x0B;
	command[1] = 0x7E;
	command[2] = 0x00;
	command[3] = 0x00;

	// Get the result of the Combination Command
	if(stth.serial >= 0) {     // If there is a valid star tracker handle
		//        size =
		nsp_stt_combination(&stth, buf, 0x0b, 0x7e);

		//        EpochTime = uint32from((uint8_t*)&buf[STT_EPOCH_TIME+nspOffset],ORDER_LITTLEENDIAN);
		ReturnCode = uint32from((uint8_t*)&buf[STT_RETURN_CODE+nspOffset],ORDER_LITTLEENDIAN);

		for(i=0 ; i<4 ; i++)
			AttQ[i] = doublefrom((uint8_t*)&buf[STT_ATT_OFFSET + i*8 + nspOffset],ORDER_LITTLEENDIAN);

		for(i=0 ; i<3 ; i++)
			VelAng[i] = doublefrom((uint8_t*)&buf[STT_VEL_OFFSET + i*8 + nspOffset],ORDER_LITTLEENDIAN);

		// Decode and print detector temperature.
		//        DetTemp = int16from((uint8_t*)&buf[STT_DETTEMP_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);

		// Decode and print processor temperature.
		ProcTemp = uint16from((uint8_t*)&buf[STT_PROCTEMP_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);

		// Get status bitfield.
		statusBits = uint32from((uint8_t*)&buf[STT_STATUS_BITFIELD+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);

		// Get Vdd core set point
		//        VddCoreSetPoint = buf[STT_VDDCORE_SETPOINT_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset];

		// Get MPU set point
		//        MPUSetPoint = buf[STT_MPU_SETPOINT_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset];


		// Get 16 dark offset values
		//        for(int i=0 ; i<16 ; i++)
		//            darkOffsets[i] = uint16from((uint8_t*)&buf[STT_DARKOFFSETS_OFFSET + i*2 + STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);

	} else {
		// When no star tracker is connected, fill in with -1
		//        size = 0;
		//        EpochTime = 0;
		ReturnCode = 0;

		for(i=0 ; i<4 ; i++)
			AttQ[i] = 0;

		for(i=0 ; i<3 ; i++)
			VelAng[i] = 0;

		//        DetTemp = 0;
		ProcTemp = 0;
		statusBits = 0;
		//        VddCoreSetPoint = 0;
		//        MPUSetPoint = 0;

		//        for(int i=0 ; i<16 ; i++)
		//            darkOffsets[i] = 0;
	}

#ifdef DOPRINT
	// Print out the data buufer read from the start tracker in HEX
	printf("Bytes read=%d\n " , size);
	for(int i=0 ; i<size ; i++) printf("%02X," , buf[i]) ; printf("\n\n");

	// Print Atitude, Velocity, and status.
	printf("Attitude Vel, Stat\n-----------\n");
	printf("Time: %f\n" , EpochTime);
	printf("ReturnCode: 0x%08X\n" , ReturnCode);
	printf("Att Quaternion: ");
	for(i=0 ; i<4 ; i++)
		printf("%f, " , AttQ[i]);
	printf("\nVelocity rvector: ");
	for(i=0 ; i<3 ; i++)
		printf("%f, " , VelAng[i]);

	printf("\nHardware telemetry\n-----------\n");
	// Print detector temperature.
	printf("DetTemp = %02X = %d dec --> DetTemp = %6.1f\n" , DetTemp, DetTemp, (double)(DetTemp>>4)/16.0);
	// Print processor temperature.
	printf("ProcTemp = %02X = %d dec --> ProcTemp = %6.1f\n" , ProcTemp, ProcTemp, 25.0+(ProcTemp-0x32) * 1.5);
	// Print status bitfield.
	printf("Status Bitfield = 0x%08X\n" , statusBits);
	// Print Vdd core set point
	printf("VddCoreSetPoint = 0x%02X\n" , VddCoreSetPoint);
	// Print MPU set point
	printf("MPUSetPoint = 0x%02X\n" , MPUSetPoint);
	printf("Reserved:%02X,%02X\n" , buf[0x16+70], buf[0x17+70]);
	// Print 16 dark offset values
	printf("darkOffsets:\n");
	for(int i=0 ; i<16 ; i++) {
		printf("%d, " , darkOffsets[i]);
		if( (i==4)||(i==9) ) printf("\n");
	}
	printf("\n\n");

#endif



	// Fill in Attitude structure
	cdata->devspec.stt[stt_num]->att.w = AttQ[0];
	cdata->devspec.stt[stt_num]->att.d.x = AttQ[1];
	cdata->devspec.stt[stt_num]->att.d.y = AttQ[2];
	cdata->devspec.stt[stt_num]->att.d.z = AttQ[3];

	// multiply stt quat by align
	sttTimesAlign = q_mult( cdata->devspec.stt[stt_num]->att, cdata->devspec.stt[stt_num]->align );

	// Fill in satellite node att structure
	cdata->node.loc.att.geoc.s.w = sttTimesAlign.w;
	cdata->node.loc.att.geoc.s.d.x = sttTimesAlign.d.x;
	cdata->node.loc.att.geoc.s.d.y = sttTimesAlign.d.y;
	cdata->node.loc.att.geoc.s.d.z = sttTimesAlign.d.z;



	// Fill in satellite node velocity structure
	cdata->node.loc.att.geoc.v.col[0] = VelAng[0];
	cdata->node.loc.att.geoc.v.col[1] = VelAng[1];
	cdata->node.loc.att.geoc.v.col[2] = VelAng[2];

	// multiply Velocity by align quat
	velTimesAlign = transform_q( cdata->devspec.stt[stt_num]->align, cdata->devspec.stt[stt_num]->omega );

	// Fill in Velocity structure
	cdata->devspec.stt[stt_num]->omega.col[0] = velTimesAlign.col[0];
	cdata->devspec.stt[stt_num]->omega.col[1] = velTimesAlign.col[1];
	cdata->devspec.stt[stt_num]->omega.col[2] = velTimesAlign.col[2];


	cdata->devspec.stt[stt_num]->retcode = ReturnCode;  // return code
	cdata->devspec.stt[stt_num]->status = statusBits;   // Status bits
	cdata->devspec.stt[stt_num]->temp = ProcTemp;       // Start Tracker processor temperature

	//    Return Code
	//    Bit 0    Image 1 output quality
	//    Bit 1    Image 2 output quality
	//    Bit 2    Image 1 processing success
	//    Bit 3    Image 2 processing success
	//    Bit 4    Full processing
	//    Bit 5    Detector image
	//    Bit 6    Consistent image solutions
	//    Bit 7    Reserved
	//    Bit 8    Master Return
	//    Bits 9-10    Image 1 Status
	//    Bits 11-12    Image 2 Status

	//    Status Bits
	//    Bit 19	POP memory overtemperature, set at approximately 85 C
	//    Bit 31	Thermal shutdown, set at approximately 160 C
	//    All other bits	Reserved
}
*/
