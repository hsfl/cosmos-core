#include "vn100_lib.h"
#include "timelib.h"

vn100_handle handle;
char device[15]="/dev/ttyUSB3";

int main(int argc, char *argv[])
{
	int32_t iretn;
	double lmjd;
	rvector imag;
	float vn100magscale[3]={16160.,16740.,16600.};
	float vn100magbias[3]={.29,-.15,.33};

	if (argc >= 2) strcpy(device,argv[1]);

	if ((iretn=vn100_connect(device, &handle)) < 0)
	{
		printf("Failed to open VN100 on %s, error %d\n",device,iretn);
		exit (iretn);
	}

	// Open calibration file for scaling of VN100
	/*
	FILE* fp;
	string filename;
	filename = cnodedir + "/vn100.ini";
	if ((fp=fopen(filename.c_str(),"r")) != NULL)
	{
		fscanf(fp, "%f %f %f\n", &vn100magscale[0], &vn100magscale[1], &vn100magscale[2]);
		fscanf(fp, "%f %f %f\n", &vn100magbias[0], &vn100magbias[1], &vn100magbias[2]);
		fclose(fp);
	}
	*/

	iretn = vn100_asynchoff(&handle);
	lmjd = currentmjd(0.);

	vn100_measurements(&handle);
	imag = handle.imu.mag;

	lmjd = currentmjd(0.);
	for (uint16_t i=0; i<2000; ++i)
	{
		imustruc simu;
		simu.mag = rv_zero();
		simu.omega = rv_zero();
		simu.accel = rv_zero();
		simu.gen.temp = 0;
		for (uint16_t j=0; j<10; ++j)
		{
			vn100_measurements(&handle);
			for (uint16_t k=0; k<3; ++k)
			{
				handle.imu.mag.col[k] -= vn100magbias[k];
				if (argc < 3)
				{
					handle.imu.mag.col[k] *= vn100magscale[k];
					/*
					handle.imu.mag.col[0] *= 14686.;
					handle.imu.mag.col[1] *= 16519.;
					handle.imu.mag.col[2] *= 15546.;
					*/
				}
			}
			simu.mag = rv_add(simu.mag, handle.imu.mag);
			simu.omega = rv_add(simu.omega, handle.imu.omega);
			simu.accel = rv_add(simu.accel, handle.imu.accel);
			simu.gen.temp += handle.imu.gen.temp;
		}

		simu.mag = rv_smult(.1, simu.mag);
		simu.omega = rv_smult(.1, simu.omega);
		simu.accel = rv_smult(.1, simu.accel);
		simu.gen.temp /= 10.;
		FILE *fp = fopen("mag_log.txt","a");
		fprintf(fp,"Time:\t%f\tMag:\t%f\t%f\t%f\t%f\tAccel:\t%f\t%f\t%f\t%f,\tOmega:\t%f\t%f\t%f\t%f\tTemp:\t%f\n",86400.*(currentmjd(0.)-lmjd),simu.mag.col[0],simu.mag.col[1],simu.mag.col[2],length_rv(simu.mag),simu.accel.col[0],simu.accel.col[1],simu.accel.col[2],length_rv(simu.accel),simu.omega.col[0],simu.omega.col[1],simu.omega.col[2],length_rv(simu.omega),simu.gen.temp);
		fclose(fp);
		printf("Time %f Mag: %f %f %f [%f], Accel: %f %f %f [%f], Omega: %f %f %f [%f], Temp: %f\n",86400.*(currentmjd(0.)-lmjd),simu.mag.col[0],simu.mag.col[1],simu.mag.col[2],length_rv(simu.mag),simu.accel.col[0],simu.accel.col[1],simu.accel.col[2],length_rv(simu.accel),simu.omega.col[0],simu.omega.col[1],simu.omega.col[2],length_rv(simu.omega),simu.gen.temp);
		COSMOS_USLEEP(100000);
		fflush(stdout);
		COSMOS_USLEEP(10000);
	}
}
