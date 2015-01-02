#include "configCosmos.h"
#include "vmt35_lib.h"
#include "vn100_lib.h"
#include "agentlib.h"

cosmosstruc *cdata;
uint32_t terror=0;
vn100_handle vn100handle;
vmt35_handle vmt35handle;
vmt35_telemetry set_amp(uint16_t channel, double mom);
//uint32_t get_mag(rvector &mag);

typedef struct
{
	double mom;
	double amp[3];
	double dac[3];
	rvector mag;
	uint32_t count;
	uint32_t invalidcount;
	uint32_t errorcount;
	uint32_t stablecount;
} rowstruc;

vector<rowstruc> rows;

int main(int argc, char *argv[])
{
	int32_t iretn;

	if (!(cdata=agent_setup_client(SOCKET_TYPE_UDP, argv[1], 1000)))
	{
		printf("Couldn't establish client\n");
		exit (-1);
	}

	if ((iretn=vmt35_connect(cdata->port[cdata->devspec.tcu[0]->portidx].name, &vmt35handle)) < 0)
	{
		printf("Couldn't connect to VMT35\n");
		exit (1);
	}

	if ((iretn=vn100_connect(cdata->port[cdata->devspec.imu[0]->portidx].name, &vn100handle)) != 0)
	{
		printf("Couldn't connect to VN100\n");
		exit (1);
	}

	vmt35_enable(&vmt35handle);


	rvector vn100_zero;

	set_amp(0, .01);
	set_amp(1, .01);
	set_amp(2, .01);
	set_amp(0, 0.);
	set_amp(1, 0.);
	set_amp(2, 0.);
	iretn = vn100_get_mag(&vn100handle, .01, 100);
	vn100_zero = vn100handle.imu.mag;
	set_amp(0, -.01);
	set_amp(1, -.01);
	set_amp(2, -.01);
	set_amp(0, 0.);
	set_amp(1, 0.);
	set_amp(2, 0.);
	iretn = vn100_get_mag(&vn100handle, .01, 100);
	vn100_zero = rv_smult(.5, rv_add(vn100_zero, vn100handle.imu.mag));
	fprintf(stderr,"Zero: %d %f %f %f\n",iretn, vn100_zero.col[0], vn100_zero.col[1], vn100_zero.col[2]);
	fflush(stderr);

	float known_moment[3]={19.48,19.83,20.17};
	float vn100_scale[2][3][3];
	for (uint16_t i=0; i<3; ++i)
	{
		rvector vn100_temp;

		set_amp(i, .04);
		set_amp(i, .05);
		iretn = vn100_get_mag(&vn100handle, .01, 5);
		vn100_temp = vn100handle.imu.mag;
		for (uint16_t j=0; j<3; ++j)
		{
			vn100_scale[1][i][j] = known_moment[i] / (vn100_temp.col[j]-vn100_zero.col[j]);
		}
		set_amp(i, .06);
		set_amp(i, .05);
		iretn = vn100_get_mag(&vn100handle, .01, 5);
		vn100_temp = vn100handle.imu.mag;
		for (uint16_t j=0; j<3; ++j)
		{
			vn100_scale[1][i][j] += known_moment[i] / (vn100_temp.col[j]-vn100_zero.col[j]);
			vn100_scale[1][i][j] /= 2.;
		}
		set_amp(i, 0.);
		set_amp(i, -.04);
		set_amp(i, -.05);
		iretn = vn100_get_mag(&vn100handle, .01, 5);
		vn100_temp = vn100handle.imu.mag;
		for (uint16_t j=0; j<3; ++j)
		{
			vn100_scale[0][i][j] = -known_moment[i] / (vn100_temp.col[j]-vn100_zero.col[j]);
		}
		set_amp(i, -.06);
		set_amp(i, -.05);
		iretn = vn100_get_mag(&vn100handle, .01, 5);
		vn100_temp = vn100handle.imu.mag;
		for (uint16_t j=0; j<3; ++j)
		{
			vn100_scale[0][i][j] -= known_moment[i] / (vn100_temp.col[j]-vn100_zero.col[j]);
			vn100_scale[0][i][j] /= 2.;
		}
		fprintf(stderr,"Scale: %d %f %f %f ",iretn, vn100_scale[0][i][0], vn100_scale[0][i][1], vn100_scale[0][i][2]);
		fprintf(stderr,"%f %f %f\n", vn100_scale[1][i][0], vn100_scale[1][i][1], vn100_scale[1][i][2]);
		fflush(stderr);

		set_amp(0, .01);
		set_amp(1, .01);
		set_amp(2, .01);
		set_amp(0, 0.);
		set_amp(1, 0.);
		set_amp(2, 0.);
		iretn = vn100_get_mag(&vn100handle, .01, 100);
		vn100_zero = vn100handle.imu.mag;
		set_amp(0, -.01);
		set_amp(1, -.01);
		set_amp(2, -.01);
		set_amp(0, 0.);
		set_amp(1, 0.);
		set_amp(2, 0.);
		iretn = vn100_get_mag(&vn100handle, .01, 100);
		vn100_zero = rv_smult(.5, rv_add(vn100_zero, vn100handle.imu.mag));
		fprintf(stderr,"Zero: %d %f %f %f\n",iretn, vn100_zero.col[0], vn100_zero.col[1], vn100_zero.col[2]);
		fflush(stderr);
	}

	for (uint16_t i=0; i<3; ++i)
	{
		set_amp(0, 0.);
		set_amp(1, 0.);
		set_amp(2, 0.);
		for (int16_t direction=1; direction>-2; direction-=2)
		{
			for (double amp=-.086; amp<.087; amp+=.001)
			{
				rowstruc trow;
				vmt35handle.telem = set_amp(i, amp*direction);
				trow.stablecount = vn100_get_mag(&vn100handle, .02, 2);
				trow.mag = vn100handle.imu.mag;
				trow.mag = rv_sub(trow.mag, vn100_zero);
				trow.amp[0] = trow.amp[1] = trow.amp[2] = 0.;
				trow.amp[i] = amp*direction;
				trow.mom = vmt35_calc_moment(trow.amp[i], cdata->devspec.mtr[i]->npoly, cdata->devspec.mtr[i]->ppoly);
				trow.dac[0] = vmt35handle.telem.dac[0];
				trow.dac[1] = vmt35handle.telem.dac[1];
				trow.dac[2] = vmt35handle.telem.dac[2];
				trow.count = vmt35handle.telem.count;
				trow.invalidcount = vmt35handle.telem.invalidcount;
				trow.errorcount = terror;
				rows.push_back(trow);
				fprintf(stderr,"%" PRIu32 " %8f\t",(uint32_t)rows.size(),trow.mom);
				fprintf(stderr,"%8f\t%8f\t%8f\t",trow.amp[0],trow.amp[1],trow.amp[2]);
				fprintf(stderr,"%8f\t%8f\t%8f\t%8f\t%8f\t%8f\t",trow.dac[0]/1e6,trow.dac[1]/1e6,trow.dac[2]/1e6,trow.mag.col[0],trow.mag.col[1],trow.mag.col[2]);
				fprintf(stderr,"%u\t%u\t%u\t%u\n", trow.stablecount, trow.count, trow.invalidcount,  trow.errorcount);
				fflush(stderr);
			}
		}
	}

	set_amp(0, .01);
	set_amp(1, .01);
	set_amp(2, .01);
	set_amp(0, 0.);
	set_amp(1, 0.);
	set_amp(2, 0.);
	iretn = vn100_get_mag(&vn100handle, .01, 100);
	vn100_zero = vn100handle.imu.mag;
	set_amp(0, -.01);
	set_amp(1, -.01);
	set_amp(2, -.01);
	set_amp(0, 0.);
	set_amp(1, 0.);
	set_amp(2, 0.);
	iretn = vn100_get_mag(&vn100handle, .01, 100);
	vn100_zero = rv_smult(.5, rv_add(vn100_zero, vn100handle.imu.mag));

	for (double mom=-30.; mom<30.003; mom+=.3)
	{
		rowstruc trow;
		trow.amp[0] = vmt35_calc_amp(mom, cdata->devspec.mtr[0]->npoly, cdata->devspec.mtr[0]->ppoly);
		vmt35handle.telem = set_amp(0, trow.amp[0]);
		trow.amp[1] = vmt35_calc_amp(mom, cdata->devspec.mtr[1]->npoly, cdata->devspec.mtr[1]->ppoly);
		vmt35handle.telem = set_amp(1, trow.amp[1]);
		trow.amp[2] = vmt35_calc_amp(mom, cdata->devspec.mtr[2]->npoly, cdata->devspec.mtr[2]->ppoly);
		vmt35handle.telem = set_amp(2, trow.amp[2]);
		trow.stablecount = vn100_get_mag(&vn100handle, .02, 2);
		trow.mag = vn100handle.imu.mag;
		trow.mag = rv_sub(trow.mag, vn100_zero);
		trow.dac[0] = vmt35handle.telem.dac[0];
		trow.dac[1] = vmt35handle.telem.dac[1];
		trow.dac[2] = vmt35handle.telem.dac[2];
		trow.count = vmt35handle.telem.count;
		trow.invalidcount = vmt35handle.telem.invalidcount;
		trow.mom = mom;
		/*
		for (uint16_t imtr=0; imtr<=2; ++imtr)
		{
			for (uint16_t icoord=0; icoord<3; ++icoord)
			{
				trow.mag.col[icoord] -= trow.mom / vn100_scale[imtr][icoord];
			}
		}
		*/
		trow.errorcount = terror;
		rows.push_back(trow);
		fprintf(stderr,"%" PRIu32 " %8f\t",(uint32_t)rows.size(),trow.mom);
		fprintf(stderr,"%8f\t%8f\t%8f\t",trow.amp[0],trow.amp[1],trow.amp[2]);
		fprintf(stderr,"%8f\t%8f\t%8f\t%8f\t%8f\t%8f\t",trow.dac[0]/1e6,trow.dac[1]/1e6,trow.dac[2]/1e6,trow.mag.col[0],trow.mag.col[1],trow.mag.col[2]);
		fprintf(stderr,"%u\t%u\t%u\t%u\n", trow.stablecount, trow.count, trow.invalidcount,  trow.errorcount);
		fflush(stderr);
	}

	set_amp(0, 0.);
	set_amp(1, 0.);
	set_amp(2, 0.);

	printf("Moment\tAmpX\tAmpY\tAmpZ\tDACa\tDACb\tDACc\tMagX\tMagY\tMaxZ\t#Stable\t#Commands\t#Invalid\t#Error\n");
	for (uint32_t i=0; i<rows.size(); ++i)
	{
		printf("%8f\t",rows[i].mom);
		printf("%8f\t%8f\t%8f\t",rows[i].amp[0],rows[i].amp[1],rows[i].amp[2]);
		printf("%8f\t%8f\t%8f\t%8f\t%8f\t%8f\t",rows[i].dac[0]/1e6,rows[i].dac[1]/1e6,rows[i].dac[2]/1e6,rows[i].mag.col[0],rows[i].mag.col[1],rows[i].mag.col[2]);
		printf("%u\t%u\t%u\t%u\n", rows[i].stablecount, rows[i].count, rows[i].invalidcount,  rows[i].errorcount);
	}

	vmt35_disable(&vmt35handle);
	vmt35_disconnect(&vmt35handle);

}

vmt35_telemetry set_amp(uint16_t channel, double amp)
{
	int32_t iretn;
	uint16_t tcount=0;
	double setamp=0.;

	do
	{
		++tcount;
		if ((iretn=vmt35_set_amps(&vmt35handle, channel, amp)) < 0)
		{
			fprintf(stderr, "vmt35_set_amps Error: %d\n", iretn);
			++terror;
		}
		while ((iretn=vmt35_get_telemetry(&vmt35handle)) < 0)
		{
			fprintf(stderr, "vmt35_get_telemetry Error: %d\n", iretn);
			++terror;
		}
		switch (channel)
		{
		case 0:
			{
				setamp = vmt35handle.telem.dac[0]/1e6;
				break;
			}
		case 1:
			{
				setamp = vmt35handle.telem.dac[1]/1e6;
				break;
			}
		case 2:
			{
				setamp = vmt35handle.telem.dac[2]/1e6;
				break;
			}
		}
	} while (tcount < 100 && fabs(amp-setamp) > .001);

	return (vmt35handle.telem);

}
