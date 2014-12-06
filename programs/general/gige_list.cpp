#include "gige_lib.h"
#include "agentlib.h"
#include "datalib.h"
#include "time.h"

int main(int argc, char *argv[])
{
	vector<gige_acknowledge_ack> gige_list;

	gige_list = gige_discover();
	if (!gige_list.size())
	{
		printf("Couldn't find any cameras\n");
		exit(1);
	}

	for (uint16_t i=0; i<gige_list.size(); ++i)
	{
		printf("Camera %u: %s %s %s\n", i, gige_value_to_address(gige_list[i].address), gige_list[i].serial_number, gige_list[i].manufacturer);
	}
}
