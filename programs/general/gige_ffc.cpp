#include "support/configCosmos.h"
#include "device/general/gige_lib.h"

int main(int argc, char *argv[])
{
	gige_handle *handle;
	char ipaddress[20] = "192.168.200.32";
	uint16_t ffc_state = 0;
//	uint32_t iretn;

	switch (argc)
	{
	case 3:
		strcpy(ipaddress, argv[2]);
	case 2:
		switch(argv[1][1])
		{
		case 'x':
			ffc_state = 2;
		case 'f':
			ffc_state = 0;
			break;
		case 'u':
			ffc_state = 1;
			break;
		case 'n':
			ffc_state = 10;
			break;
		}
		break;
	default:
		printf("Usage: a35_ffc {off|auto|once} [ip_address]\n");
		exit (1);
		break;
	}

	if ((handle=gige_open(ipaddress,0x02,40000,5000,10000000)) == NULL)
	{
		printf("Couldn't open camera\n");
		exit(1);
	}

	printf("Current State: %d\n", gige_readreg(handle, A35_FFCMODE));
	switch(ffc_state)
	{
	case 0:
	case 1:
	case 2:
		gige_writereg(handle, A35_FFCMODE, ffc_state); // Set FFC to manual or auto
		break;
	case 10:
		gige_writereg(handle, A35_FFCMODE, A35_FFCMODE_MANUAL); // Set FFC to manual or auto
		gige_writereg(handle, A35_COMMAND_DOFFC, 0); // Command FFC
		break;
	}
	printf("Set To: %d\n", ffc_state);

	gige_close(handle);

}


