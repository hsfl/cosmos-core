#include "pic_lib.h"

	char device[150] = "/dev/ttyUSB0";

int main(int argc, char *argv[])
{
	pic_handle *handle=NULL;
//	int32_t iretn;

	if (argc == 2)
		strcpy(device, argv[1]);

	pic_connect(device, handle);

	
}
