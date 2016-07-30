/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "gige_lib.h"
#include "agentlib.h"
#include "datalib.h"
#include "time.h"

#define A35COUNT 1


uint8_t image[110000000];

int main(int argc, char *argv[])
{
	gige_handle *handle;
	int32_t tbytes;
	int32_t expbytes;
	int32_t iretn;
	uint32_t width, height;
	//uint32_t faddress, fsize;
	uint32_t bsize;
	FILE *fp;
	std::vector<gige_acknowledge_ack> gige_list;

	std::string extra = "";
	uint32_t exposure=0;
	uint32_t gain=1;
	uint32_t binning=1;
	char ipaddress[20];

	switch (argc)
	{
	case 6:
		binning=atol(argv[5]);
	case 5:
		gain = atol(argv[4]);
	case 4:
		exposure = atol(argv[3]);
	case 3:
		extra = argv[2];
	case 2:
		strcpy(ipaddress, argv[1]);
		break;
	default:
		printf("Usage: gigesnap ipaddress [extraname [exposure [gain [binning]]]]\n");
		exit (1);
		break;
	}

	gige_list = gige_discover();
	if (!gige_list.size())
	{
		printf("Couldn't find any cameras\n");
		exit(1);
	}


	//define variables to be read by the prosilica - e.g., to check exposure, etc.
	uint32_t used_exposure=0;
	uint32_t used_imagewidth=0;
	uint32_t used_imageheight=0;

	std::string imagename = "testimage.imgdata";
	std::string metafilename = "testimage.imgmeta";
	std::string fringename = "testimage.imgfringe";




	//Time stuff.  Probably superfluous.
	time_t current_time;
	char* c_time_string;

	/* Obtain current time as seconds elapsed since the Epoch. */
	current_time = time(NULL);


	if (current_time == ((time_t)-1))

	{
		(void) fprintf(stderr, "Failure to compute the current time.");
		return EXIT_FAILURE;
	}
	/* Convert to local time format. */
	c_time_string = ctime(&current_time);

	if (c_time_string == NULL)
	{
		(void) fprintf(stderr, "Failure to convert the current time.");
		return EXIT_FAILURE;
	}

	/* Print to stdout. */
	// (void) printf("Current time is %s", c_time_string);
	//    return EXIT_SUCCESS;




	if ((handle=gige_open(ipaddress,0x02,40000,5000,7500000)) == NULL)
	{
		if((handle = gige_open(gige_value_to_address(gige_list[0].address),0x02,40000,5000,7500000)) == NULL)
		{
			printf("Couldn't open camera\n");
			exit(1);
		}
	}

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	//	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_HIGH);
	//	printf("Read DEVICE_MAC_HIGH: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_LOW);
	//	printf("Read DEVICE_MAC_LOW: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	//	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_VERSION);
	//	printf("Read VERSION: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MODE);
	//	printf("Read DEVICE_MODE: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_GVCP_HEARTBEAT_TIMEOUT);
	//	printf("Read GVCP_HEARTBEAT_TIMEOUT %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_PRIMARY_APPLICATION_IP_ADDRESS);
	//	printf("Read PRIMARY_APPLICATION_IP_ADDRESS %x\n",iretn);

	/*
	iretn = gige_readmem(handle,GIGE_REG_FIRST_URL,512);
	char *ptr = (char *)handle->cack_mem.data;
	do
	{
		ptr++;
	} while (*ptr != ':');
	char *ptr1 = ++ptr;

	do
	{
		ptr++;
	} while (*ptr != ';');

	char fname[100];
	strncpy(fname,ptr1,ptr-ptr1);
	fname[ptr-ptr1] = 0;
	uint32_t faddress, fsize;
	sscanf((char *)ptr,";%x;%x",&faddress,&fsize);
	printf("Read GIGE_REG_FIRST_URL %u %s;%u;%u\n",iretn,fname,faddress,fsize);

	iretn = gige_readmem(handle,GIGE_REG_SECOND_URL,512);
	printf("Read GIGE_REG_FIRST_SECOND %u %s\n",iretn,handle->cack_mem.data);

	fp = fopen(fname,"w");
	uint32_t count=0;
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


	// configure camera- moved up to reflect accurate data down below
	iretn = gige_readmem(handle,GIGE_REG_MODEL_NAME,GIGE_MAX_MODEL_NAME);
    printf("Model: %s\n", (char *)handle->cack_mem.data);
	bsize = handle->bestsize;
	if( strncmp((char *)handle->cack_mem.data, "FLIR AX5", 8) == 0 )
	{
		iretn = gige_readreg(handle,A35_PIXELFORMAT);
		//		printf("Read A35_PIXELFORMAT %d\n",iretn);

		width = 336;
		printf("Read A35_WIDTH %d\n",width);

		height = 256;
		printf("Read A35_HEIGHT %d\n",height);

        iretn = a35_config(handle, width, height, A35_SENSORVIDEOSTANDARD_30HZ);
		expbytes = width * height * exposure * 2;
		tbytes = a35_image(handle, exposure, image, bsize);

		if (expbytes == tbytes)
		{
            fringename = data_type_path("hiakasat", "temp", "gige", currentmjd(0.), extra, "imgfringe");
			FILE *fp = fopen(fringename.c_str(), "w");
			fprintf(fp, "Fringes\n");
			uint16_t *cube = (uint16_t *)image;
			float mean[336][256];
			float high[336];
			float fringe[256];
			for (uint32_t icol=0; icol<336; ++icol)
			{
				high[icol]=0.;
				for (uint32_t irow=0; irow<256; ++irow)
				{
					mean[icol][irow] = 0.;
					for (uint32_t ilayer=0; ilayer<exposure; ++ilayer)
					{
						mean[icol][irow] += cube[ilayer*336*256+irow*336+icol];
					}
					mean[icol][irow] /= (float)exposure;
					if (mean[icol][irow] > high[icol])
					{
						high[icol] = mean[icol][irow];
						printf("[%d,%d] %f\r",icol,irow,high[icol]);
					}
				}
				printf("\n");
			}

			for (uint32_t irow=0; irow<256; ++irow)
			{
				fringe[irow] = 0.;
				for (uint32_t icol=0; icol<336; ++icol)
				{
					fringe[irow] += mean[icol][irow] / mean[icol][0];
				}
				fringe[irow] /= 336.;
				fprintf(fp,"%.4g\n",fringe[irow]);
			}
			fclose(fp);

			printf("A35_TEMPERATURE: %u A35_TEMPERATUREFPA: %f\n", gige_readreg(handle, A35_TEMPERATURE), gige_readreg(handle, A35_TEMPERATUREFPA)/10.);
		}
	}
	else
	{


		iretn = prosilica_config(handle, PROSILICA_PixelFormat_BayerRG12, binning, binning, 2448, 2050, 0, 0);
		iretn = gige_readreg(handle,PROSILICA_StreamBytesPerSec);
		//		printf("Read PROSILICA_StreamBytesPerSec %d\n",iretn);






		iretn = gige_readreg(handle,PROSILICA_BinningXValue);
		//		printf("Read PROSILICA_BinningXValue %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_BinningYValue);
		//		printf("Read PROSILICA_BinningYValue %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_RegionX);
		//		printf("Read PROSILICA_RegionX %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_RegionY);
		//		printf("Read PROSILICA_RegionY %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_PixelFormat);
		//		printf("Read PROSILICA_PixelFormat %d\n",iretn);

		width = gige_readreg(handle,PROSILICA_MaxWidth);
		printf("Read PROSILICA_MaxWidth %d\n",width);

		height = gige_readreg(handle,PROSILICA_MaxHeight);
		printf("Read PROSILICA_MaxHeight %d\n",height);

		iretn = gige_readreg(handle,PROSILICA_SensorWidth);
		//		printf("Read PROSILICA_SensorWidth %d\n",iretn);

		used_imagewidth = gige_readreg(handle,PROSILICA_Width);
		//		printf("Read PROSILICA_Width %d\n",used_imagewidth);

		iretn = gige_readreg(handle,PROSILICA_SensorHeight);
		//		printf("Read PROSILICA_SensorHeight %d\n",iretn);

		used_imageheight = gige_readreg(handle,PROSILICA_Height);
		//		printf("Read PROSILICA_Height %d\n",used_imageheight);

		width = gige_readreg(handle,PROSILICA_SensorWidth);

		height = gige_readreg(handle,PROSILICA_SensorHeight);

		expbytes = width * height * 2;
		tbytes = prosilica_image(handle, (exposure==0?PROSILICA_ExposureMode_Auto:PROSILICA_ExposureMode_AutoOff), exposure, gain, image, bsize);

		used_exposure = gige_readreg(handle,PROSILICA_ExposureValue);
		printf("Read PROSILICA_ExposureValue %d\n",used_exposure);

		iretn = gige_readreg(handle,PROSILICA_AcquisitionMode);
		//		printf("Read PROSILICA_AcquisitionMode %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_PayloadSize);
		//		printf("Read PROSILICA_PayloadSize %d\n",iretn);

	}

	if (expbytes == tbytes)
	{
		//sudo histogram
		printf("\nRead: %d bytes ",tbytes);
		uint32_t counts[4]={0,0,0,0};
		uint16_t *array=(uint16_t *)image;
		double mean=0.;
		double std=0.;
		double snr=0.;
		int16_t min_value=4095.;
		int16_t max_value=0.;



		for (int32_t i=0; i<tbytes/2; ++i)
		{
			mean += array[i];
			std += (array[i]*array[i]);
			++counts[array[i]/1024];

			if (array[i]<min_value){
				min_value=array[i];
			}

			if (array[i]>max_value)
			{
				max_value=array[i];
			}
		}
		std = sqrt((std-(mean*mean/(tbytes/2)))/(tbytes/2)-1);
		mean /= (tbytes/2);
		snr = (mean/std);

		printf("%u %u %u %u %f %f %f\n\n",counts[0],counts[1],counts[2],counts[3],mean,std, snr);
		iretn = gige_readreg(handle,GIGE_REG_SCPS);
		printf("Read GIGE_REG_STREAM_CHANNEL_PACKET_SIZE %d\n",iretn);

		iretn = gige_readreg(handle,GIGE_REG_SCDA);
		printf("Read GIGE_STREAM_CHANNEL_DESTINATION_ADDRESS %x\n",iretn);

		// Separate and print out some of the image quality metrics:
		printf("___________________________ \n\n");
		printf("Mean Image Value    = %f\n",mean);
		printf("Standard Deviation  = %f\n",std);
		printf("SNR                 = %f\n",snr);
		printf("Min/Max             = %d, %d \n",min_value, max_value);


		gige_close(handle);

		// write image:

        imagename = data_type_path("hiakasat", "temp", "gige", currentmjd(0.), extra, "imgdata");
		fp = fopen(imagename.c_str(),"wb");
		fwrite((void *)image,tbytes,1,fp);
		fclose(fp);

		// write metadata file:


        metafilename = data_type_path("hiakasat", "temp", "gige", currentmjd(0.), extra, "imgmeta");
		FILE *f = fopen(metafilename.c_str(), "wb");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}

		/* print image metrics to file */
		fprintf(f, "Image Name    = %s\n",imagename.c_str());
		fprintf(f, "IP Address    = %s\n",ipaddress);
		fprintf(f, "Acq Timestamp    = %f\n",currentmjd(0.));
		fprintf(f, "Acq Date    = %s",c_time_string);
		fprintf(f, "Exposure    = %d\n",used_exposure);
		fprintf(f, "Pixel Binning    = %d\n",binning);
		fprintf(f, "Image Width    = %d\n",used_imagewidth);
		fprintf(f, "Image Height    = %d\n",used_imageheight);
		fprintf(f, "Mean Image Value    = %f\n",mean);
		fprintf(f, "Standard Deviation  = %f\n",std);
		fprintf(f, "SNR                 = %f\n",snr);
		fprintf(f, "Min/Max             = %d, %d \n",min_value, max_value);

		fclose(f);
	}
	else
	{
		printf("Error: Expected %d bytes, Read %d bytes\n", expbytes, tbytes);
	}


	exit(0);
}
