#include "gige_lib.h"
#include "agentlib.h"

uint8_t image[11000000];

int main(int argc, char *argv[])
{
	gige_handle *handle;
	int tbytes;
	uint32_t width, height;
	int32_t iretn;
	//uint32_t faddress, fsize;
	uint32_t bsize;
	FILE *fp;
	vector<gige_acknowledge_ack> gige_list;

	gige_list = gige_discover();
	if (!gige_list.size())
	{
		printf("Couldn't find any cameras\n");
		exit(1);
	}

	handle = gige_open(gige_value_to_address(gige_list[0].address),0x02,40000,5000,10000000);
	if (handle == NULL)
	{
		printf("Couldn't open camera\n");
		exit(1);
	}

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_HIGH);
	printf("Read DEVICE_MAC_HIGH: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_LOW);
	printf("Read DEVICE_MAC_LOW: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_VERSION);
	printf("Read VERSION: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MODE);
	printf("Read DEVICE_MODE: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_GVCP_HEARTBEAT_TIMEOUT);
	printf("Read GVCP_HEARTBEAT_TIMEOUT %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_PRIMARY_APPLICATION_IP_ADDRESS);
	printf("Read PRIMARY_APPLICATION_IP_ADDRESS %x\n",iretn);
/*
	iretn = gige_readmem(handle,GIGE_REG_FIRST_URL,512);
	ptr = (char *)handle->cack_mem.data;
	do
	{
		ptr++;
	} while (*ptr != ':');
	ptr1 = ++ptr;

	do
	{
		ptr++;
	} while (*ptr != ';');
	strncpy(fname,ptr1,ptr-ptr1);
	fname[ptr-ptr1] = 0;
	sscanf((char *)ptr,";%x;%x",&faddress,&fsize);
	printf("Read GIGE_REG_FIRST_URL %u %s;%u;%u\n",iretn,fname,faddress,fsize);

	iretn = gige_readmem(handle,GIGE_REG_SECOND_URL,512);
	printf("Read GIGE_REG_FIRST_SECOND %u %s\n",iretn,handle->cack_mem.data);

	fp = fopen(fname,"w");
	count=0;
	for (uint32_t i=0; i<fsize/536; ++i)
	{
		iretn = gige_readmem(handle,faddress+count,536);
		fwrite((void *)handle->cack_mem.data,iretn,1,fp);
		count += iretn;
		printf("Read GIGE_REG_XML %u %d\r",iretn,count);
	}
	iretn = gige_readmem(handle,faddress+count,fsize-count);
	fwrite((void *)handle->cack_mem.data,iretn,1,fp);
	count += iretn;
	printf("Read GIGE_REG_XML %u %d\n",iretn,count);
	fclose(fp);
*/
	iretn = gige_readreg(handle,PROSILICA_BinningXValue);
	printf("Read PROSILICA_BinningXValue %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_BinningYValue);
	printf("Read PROSILICA_BinningYValue %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_RegionX);
	printf("Read PROSILICA_RegionX %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_RegionY);
	printf("Read PROSILICA_RegionY %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_PixelFormat);
	printf("Read PROSILICA_PixelFormat %d\n",iretn);

	width = gige_readreg(handle,PROSILICA_MaxWidth);
	printf("Read PROSILICA_MaxWidth %d\n",width);

	height = gige_readreg(handle,PROSILICA_MaxHeight);
	printf("Read PROSILICA_MaxHeight %d\n",height);

	iretn = gige_readreg(handle,PROSILICA_SensorWidth);
	printf("Read PROSILICA_SensorWidth %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_Width);
	printf("Read PROSILICA_Width %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_SensorHeight);
	printf("Read PROSILICA_SensorHeight %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_Height);
	printf("Read PROSILICA_Height %d\n",iretn);

	width = gige_readreg(handle,PROSILICA_SensorWidth);
	iretn = gige_readreg(handle,PROSILICA_Width);
	printf("Read PROSILICA_Width %d\n",iretn);

	height = gige_readreg(handle,PROSILICA_SensorHeight);
	iretn = gige_readreg(handle,PROSILICA_Height);
	printf("Read PROSILICA_Height %d\n",iretn);

	iretn = prosilica_config(handle, PROSILICA_PixelFormat_BayerRG12, 1, 1, 2448, 2050, 0, 0);
	iretn = gige_readreg(handle,PROSILICA_StreamBytesPerSec);
	printf("Read PROSILICA_StreamBytesPerSec %d\n",iretn);

	bsize = handle->bestsize;

/*
	iretn = gige_writereg(handle,GIGE_REG_SCPS,bsize);
	if (iretn)
	{
		printf("Error %d writing STREAM_CHANNEL_PACKET_SIZE\n",iretn);
	}
*/
	tbytes = prosilica_image(handle, PROSILICA_ExposureMode_AutoOnce, 10000, 1, image, bsize);
	iretn = gige_readreg(handle,PROSILICA_ExposureValue);
	printf("Read PROSILICA_ExposureValue %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_AcquisitionMode);
	printf("Read PROSILICA_AcquisitionMode %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_PayloadSize);
	printf("Read PROSILICA_PayloadSize %d\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_SCPS);
	printf("Read GIGE_REG_STREAM_CHANNEL_PACKET_SIZE %d\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_SCDA);
	printf("Read GIGE_STREAM_CHANNEL_DESTINATION_ADDRESS %x\n",iretn);

	gige_close(handle);

	fp = fopen("testimage","wb");
	fwrite((void *)image,tbytes,1,fp);
	fclose(fp);

	exit(0);
}
