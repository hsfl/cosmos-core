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

#include "support/configCosmos.h"
#include "device/general/gige_lib.h"
#include "agent/agentclass.h"
#include "support/datalib.h"
#include "time.h"
#include "support/envi.h"

#define A35COUNT 1

string makeFilename(string argument, string extra="");
vector <uint8_t> image;

int main(int argc, char *argv[])
{
	gige_handle *handle;
	int32_t tbytes;
	int32_t expbytes;
	int32_t iretn = 0;
	uint32_t width, height;
	//uint32_t faddress, fsize;
	uint32_t bsize;
	FILE *fp;
	vector<gige_acknowledge_ack> gige_list;

	string extra = "";
    uint32_t exposure=1;
	uint32_t gain=1;
	uint32_t binning=1;
    uint32_t test = 0;
	char ipaddress[20];

	switch (argc)
	{
    case 7:
        binning=atol(argv[6]);
    case 6:
        gain = atol(argv[5]);
    case 5:
        test = atol(argv[4]);
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

	string imagename = "testimage.imgdata";
	string metafilename = "testimage.imgmeta";
	string fringename = "testimage.imgfringe";




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




    if ((handle=gige_open(ipaddress,0x02,40000,5000,40000000)) == NULL)
	{
        if((handle = gige_open(gige_value_to_address(gige_list[0].address),0x02,40000,5000,8000000)) == NULL)
		{
			printf("Couldn't open camera\n");
			exit(1);
		}
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


	// configure camera- moved up to reflect accurate data down below
    iretn = gige_readmem(handle, GIGE_REG_MODEL_NAME, GIGE_MAX_MODEL_NAME);
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
        image.resize(expbytes);
        tbytes = a35_image(handle, exposure, image.data(), bsize);

		if (expbytes == tbytes)
		{
            fringename = data_type_path("hiakasat", "temp", "gige", currentmjd(0.), "imgfringe", extra);
			FILE *fp = fopen(fringename.c_str(), "w");
			fprintf(fp, "Fringes\n");
            uint16_t *cube = (uint16_t *)image.data();
			float mean[336][256];
			float high[336];
			float fringe[256];
			for (uint32_t icol=0; icol<336; ++icol)
			{
				high[icol]=0.;
				for (uint32_t irow=0; irow<256; ++irow)
				{
                    mean[irow][icol] = 0.;
					for (uint32_t ilayer=0; ilayer<exposure; ++ilayer)
					{
                        mean[irow][icol] += cube[ilayer*336*256+irow*336+icol];
					}
                    mean[irow][icol] /= (float)exposure;
                    if (mean[irow][icol] > high[icol])
					{
                        high[icol] = mean[irow][icol];
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
                    fringe[irow] += mean[irow][icol] / mean[icol][0];
				}
				fringe[irow] /= 336.;
				fprintf(fp,"%.4g\n",fringe[irow]);
			}
			fclose(fp);

			printf("A35_TEMPERATURE: %u A35_TEMPERATUREFPA: %f\n", gige_readreg(handle, A35_TEMPERATURE), gige_readreg(handle, A35_TEMPERATUREFPA)/10.);
		}
	}
    else if (strncmp((char *)handle->cack_mem.data, "PHX050S", 7) == 0 )
    {
        width = gige_readreg(handle,PHXReg::PHXWidthReg);
        height = gige_readreg(handle,PHXReg::PHXHeightReg);

        iretn = phx_config(handle, width, height, 1, 1, 1);
        if (iretn < 0)
        {
            printf("Error configuring: %s\n", cosmos_error_string(iretn).c_str());
        }
        expbytes = width * height * exposure * handle->bpp;
        image.resize(expbytes);
        tbytes =  phx_image(handle, exposure, image.data(), bsize);

        if (expbytes <= tbytes)
        {
            FILE *fp;
            string fname = makeFilename("data", extra);
            fp = fopen(fname.c_str(),"w");
            fwrite((void *)image.data(),tbytes,1,fp);
            fclose(fp);

            envi_hdr ehdr;
            ehdr.planes = exposure;
            ehdr.columns = width;
            ehdr.rows = height;
            ehdr.offset = 0;
            //ehdr.endian;
            ehdr.datatype = DT_U_INT;
            ehdr.interleave = BSQ;
            ehdr.byteorder = BO_INTEL;
            ehdr.basename = fname;
            write_envi_hdr(ehdr);

            fname = makeFilename("stats", extra);
            fp = fopen(fname.c_str(),"w");
            if (fp == nullptr)
            {
                perror("Error opening stats file");
                exit (1);
            }
            uint16_t *array = (uint16_t *)image.data();
            vector<vector<float>> mean[3];
            for (uint16_t bin=0; bin<3; ++bin)
            {
                mean[0].resize(height/2-1);
                mean[1].resize(height/2-1);
                mean[2].resize(height/2-1);
                for (uint32_t irow=0; irow<height/2-1; ++irow)
                {
                    mean[0][irow].resize(width/2-1);
                    mean[1][irow].resize(width/2-1);
                    mean[2][irow].resize(width/2-1);
                    for (uint32_t icol=0; icol<width/2-1; ++icol)
                    {
                        mean[0][irow][icol] = 0.;
                        mean[1][irow][icol] = 0.;
                        mean[2][irow][icol] = 0.;
                        for (uint32_t ilayer=0; ilayer<exposure; ++ilayer)
                        {
                            float rvalue = array[(ilayer*width*height+2*irow*width+2*icol)];
                            mean[0][irow][icol] += rvalue;
                            float gvalue = array[(ilayer*width*height+2*irow*width+2*icol+1)];
                            gvalue += array[(ilayer*width*height+(2*irow+1)*width+2*icol)];
                            mean[1][irow][icol] += gvalue / 2.;
                            float bvalue = array[(ilayer*width*height+(2*irow+1)*width+2*icol+1)];
                            mean[2][irow][icol] += bvalue;
                        }
                        mean[bin][irow][icol] /= (float)exposure;
                    }
                    fwrite((void *)mean[0][irow].data(),width/2-1,4,fp);
                    fwrite((void *)mean[1][irow].data(),width/2-1,4,fp);
                    fwrite((void *)mean[2][irow].data(),width/2-1,4,fp);
                }
            }
            fclose(fp);

            ehdr.planes = 3;
            ehdr.columns = width/2-1;
            ehdr.rows = height/2-1;
            ehdr.offset = 0;
            //ehdr.endian;
            ehdr.datatype = DT_FLOAT;
            ehdr.interleave = BIL;
            ehdr.byteorder = BO_INTEL;
            ehdr.basename = fname;
            write_envi_hdr(ehdr);
        }

        iretn = gige_readreg(handle,PHXDeviceLinkThroughputLimit);
        printf("Read DeviceLinkThroughput %u\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXAcquisitionModeReg);
        printf("Read PHX_AcquisitionMode %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXTestImageSelectorReg);
        printf("Read PHXTESTIMAGESELECTOR %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXPixelColorFilter);
        printf("Read PHX_PixelColorFilter %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXIspBayerPattern);
        printf("Read PHX_IspBayerPattern %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXADCBitDepth);
        printf("Read PHX_ADCBitDepth %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXPixelFormatReg);
        printf("Read PHX_PixelFormat %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXSensorWidthReg);
        printf("Read PHX_SensorWidth %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXSensorHeightReg);
        printf("Read PHX_SensorHeight %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXWidthReg);
        printf("Read PHX_Width %d\n",iretn);

        iretn = gige_readreg(handle,PHXReg::PHXHeightReg);
        printf("Read PHX_Height %d\n",iretn);

    }
    else if (strncmp((char *)handle->cack_mem.data, "PT1000", 6) == 0 )
    {
        iretn = gige_readreg(handle,PT1000::PixelFormatReg);
        printf("Read PT1000PIXELFORMAT %d\n",iretn);

        width = gige_readreg(handle,PT1000::WidthReg);
        printf("Read PT1000_WIDTH %d\n",width);

        height = gige_readreg(handle,PT1000::HeightReg);
        printf("Read PT1000_HEIGHT %d\n",height);

        iretn = pt1000_config(handle, width, height);
        if (iretn < 0)
        {
            printf("Error configuring: %s\n", cosmos_error_string(iretn).c_str());
        }
        expbytes = width * height * exposure * 2;
        image.resize(expbytes);
        tbytes = pt1000_image(handle, exposure, image.data(), bsize);

        if (expbytes == tbytes)
        {
            FILE *fp;
            fp = fopen("pt1000_test.img","wb");
            fwrite((void *)image.data(),tbytes,1,fp);
            fclose(fp);

            fp = fopen("pt1000_test", "w");
            uint16_t *cube = (uint16_t *)image.data();
            vector<vector<float>> mean;
            mean.resize(width);
            vector<float> high;
            high.resize(width);
            for (uint32_t icol=0; icol<width; ++icol)
            {
                high[icol]=0.;
                mean[icol].resize(height);
                for (uint32_t irow=0; irow<height; ++irow)
                {
                    mean[irow][icol] = 0.;
                    for (uint32_t ilayer=0; ilayer<exposure; ++ilayer)
                    {
                        mean[irow][icol] += cube[ilayer*width*height+irow*width+icol];
                    }
                    mean[irow][icol] /= (float)exposure;
                    if (mean[irow][icol] > high[icol])
                    {
                        high[icol] = mean[irow][icol];
                        printf("[%d,%d] %f\r",icol,irow,high[icol]);
                    }
                }
                printf("\n");
            }

            fclose(fp);
        }

        iretn = gige_readreg(handle,PT1000::AcquisitionModeReg);
        printf("Read PT1000_AcquisitionMode %d\n",iretn);

        iretn = gige_readreg(handle,PT1000::PixelFormatReg);
        printf("Read PT1000_PixelFormat %d\n",iretn);

        iretn = gige_readreg(handle,PT1000::SensorWidthReg);
        printf("Read PT1000_SensorWidth %d\n",iretn);

        iretn = gige_readreg(handle,PT1000::SensorHeightReg);
        printf("Read PT1000_SensorHeight %d\n",iretn);

        iretn = gige_readreg(handle,PT1000::WidthReg);
        printf("Read PT1000_Width %d\n",iretn);

        iretn = gige_readreg(handle,PT1000::HeightReg);
        printf("Read PT1000_Height %d\n",iretn);

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
        image.resize(expbytes);
        tbytes = prosilica_image(handle, (exposure==0?PROSILICA_ExposureMode_Auto:PROSILICA_ExposureMode_AutoOff), exposure, gain, image.data(), bsize);

		used_exposure = gige_readreg(handle,PROSILICA_ExposureValue);
		printf("Read PROSILICA_ExposureValue %d\n",used_exposure);

		iretn = gige_readreg(handle,PROSILICA_AcquisitionMode);
		//		printf("Read PROSILICA_AcquisitionMode %d\n",iretn);

		iretn = gige_readreg(handle,PROSILICA_PayloadSize);
		//		printf("Read PROSILICA_PayloadSize %d\n",iretn);

	}

    if (expbytes <= tbytes)
	{
        // write image:

        imagename = data_type_path("lab", "temp", "gige", currentmjd(0.), "imgdata", extra);
        fp = fopen(imagename.c_str(),"wb");
        fwrite((void *)image.data(),tbytes,1,fp);
        fclose(fp);

        //sudo histogram
		printf("\nRead: %d bytes ",tbytes);
        uint32_t counts[3][4]={0,0,0,0};
        double mean[3]={0.,0.,0.};
        double std[3]={0.,0.,0.};
        double snr[3]={0.,0.,0.};
        int16_t min_value[3]={255, 255, 255};
        int16_t max_value[3]={0,0,0};



        for (int32_t i=0; i<tbytes/3; ++i)
        {
            for (uint16_t bin=0; bin<3; ++bin)
            {
                mean[bin] += image[i*3+bin];
                std[bin] += (1.*image[i*3+bin]*image[i*3+bin]);
                ++counts[bin][image[i*3+bin]/64];

                if (image[i*3+bin]<min_value[bin]){
                    min_value[bin]=image[i*3+bin];
                }

                if (image[i*3+bin]>max_value[bin])
                {
                    max_value[bin]=image[i*3+bin];
                }
            }
        }
        for (uint16_t bin=0; bin<3; ++bin)
        {
            std[bin] = sqrt((std[bin]-(mean[bin]*mean[bin]/(tbytes/3)))/(tbytes/3)-1);
            mean[bin] /= (tbytes/3);
            snr[bin] = (mean[bin]/std[bin]);
        }

		printf("%u %u %u %u %f %f %f\n\n",counts[0],counts[1],counts[2],counts[3],mean,std, snr);
		iretn = gige_readreg(handle,GIGE_REG_SCPS);
		printf("Read GIGE_REG_STREAM_CHANNEL_PACKET_SIZE %d\n",iretn);

		iretn = gige_readreg(handle,GIGE_REG_SCDA);
		printf("Read GIGE_STREAM_CHANNEL_DESTINATION_ADDRESS %x\n",iretn);

		// Separate and print out some of the image quality metrics:
		printf("___________________________ \n\n");
        printf("Mean Image Values    = %f %f %f\n",mean[0], mean[1], mean[2]);
        printf("Standard Deviations  = %f %f %f\n",std[0], std[1], std[2]);
        printf("SNRs                 = %f %f %f\n",snr[0], snr[1], snr[2]);
        printf("Min/Maxs             = %u %u %u, %u %u %u \n",min_value[0], min_value[1], min_value[2], max_value[0],  max_value[1],  max_value[2]);


		gige_close(handle);

		// write metadata file:


	}
	else
	{
		printf("Error: Expected %d bytes, Read %d bytes\n", expbytes, tbytes);
	}


	exit(0);
}

string makeFilename(string argument, string extra)
{
    string fname;
    char DateTimeStamp[16];
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);

    sprintf(DateTimeStamp, "%4d%02d%02d%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fname = "image_";
    fname.append(DateTimeStamp);
    if (extra.size())
    {
        fname.append("_");
        fname.append(extra);
    }
    fname.append("_");
    fname.append(argument);
    fname.append(".bip");

    return fname;
}

