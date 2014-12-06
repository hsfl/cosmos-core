#include "nodelib.h"
#include "jsonlib.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static cosmosstruc cdata;

//! Initialize Node configuration
/*! Load initial Node configuration file. Then calculate all derivative values (eg. COM)
	\param node Node to be initialized using node.ini. Node must be a directory in
	::nodedir. If NULL, node.ini must be in current directory.
	\return 0, or negative error.
*/
int32_t node_init(const char *node, cosmosstruc *cdata)
{
	int32_t iretn;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	iretn = json_setup_node(node, cdata);
	if (iretn < 0)
	{
		return iretn;
	}

	node_calc(cdata);

	//! Load targeting information
	cdata->node.target_cnt = load_target(cdata);

	return 0;
}

//! Calculate Satellite configuration values.
/*! Using the provided satellite structure, populate the derivative static quantities and initialize any
 * reasonable dynamic quantities.
	\param sat Pointer to satellite structure
	\return 0
*/
int32_t node_calc(cosmosstruc *cdata)
{
	uint16_t n, i, j, k;
	double dm, ta, tb, tc;
	rvector tv0, tv1, tv2, tv3, dv, sv;

	cdata->physics.mass = cdata->physics.hcap = cdata->physics.heat = 0.;
	cdata->physics.moi.col[0] = cdata->physics.moi.col[1] =
	cdata->physics.moi.col[2] = 0.;
	cdata->physics.com = rv_zero();

	for (n=0; n<cdata->piece.size(); n++)
	{
		if (cdata->piece[n].mass == 0.)
			cdata->piece[n].mass = .001;
		cdata->piece[n].temp = 300.;
		cdata->piece[n].heat = 300. * cdata->piece[n].hcap;
		cdata->physics.heat += cdata->piece[n].heat;
		cdata->physics.mass += cdata->piece[n].mass;
		cdata->physics.hcap += cdata->piece[n].hcap * cdata->piece[n].mass;
		cdata->piece[n].area = 0.;
		if (cdata->piece[n].pnt_cnt)
		{
			dm = cdata->piece[n].mass / cdata->piece[n].pnt_cnt;
			cdata->piece[n].centroid = rv_zero();
			for (i=0; i<cdata->piece[n].pnt_cnt; i++)
			{
				cdata->piece[n].centroid.col[0] += cdata->piece[n].points[i].col[0] * dm;
				cdata->piece[n].centroid.col[1] += cdata->piece[n].points[i].col[1] * dm;
				cdata->piece[n].centroid.col[2] += cdata->piece[n].points[i].col[2] * dm;
			}
			cdata->physics.com.col[0] += cdata->piece[n].centroid.col[0];
			cdata->physics.com.col[1] += cdata->piece[n].centroid.col[1];
			cdata->physics.com.col[2] += cdata->piece[n].centroid.col[2];
			cdata->piece[n].centroid.col[0] /= cdata->piece[n].mass;
			cdata->piece[n].centroid.col[1] /= cdata->piece[n].mass;
			cdata->piece[n].centroid.col[2] /= cdata->piece[n].mass;
			switch (cdata->piece[n].type)
			{
			case PIECE_TYPE_EXTERNAL_PANEL:
				for (i=0; i<cdata->piece[n].pnt_cnt-1; i++)
				{
					tv0 = rv_sub(cdata->piece[n].points[i],cdata->piece[n].centroid);
					tv1 = rv_sub(cdata->piece[n].points[i+1],cdata->piece[n].centroid);
					dv = rv_sub(tv1,tv0);
					sv = rv_add(tv1,tv0);
					cdata->piece[n].area += sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.;
				}
				tv0 = rv_sub(cdata->piece[n].points[cdata->piece[n].pnt_cnt-1],cdata->piece[n].centroid);
				tv1 = rv_sub(cdata->piece[n].points[0],cdata->piece[n].centroid);
				dv = rv_sub(tv1,tv0);
				sv = rv_add(tv1,tv0);
				cdata->piece[n].area += sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.;
				tv0 = cdata->piece[n].points[0];
				tv1 = cdata->piece[n].points[1];
				tv2 = cdata->piece[n].points[2];
				ta = tv0.col[1]*(tv1.col[2]-tv2.col[2])+tv1.col[1]*(tv2.col[2]-tv0.col[2])+tv2.col[1]*(tv0.col[2]-tv1.col[2]);
				tb = tv0.col[2]*(tv1.col[0]-tv2.col[0])+tv1.col[2]*(tv2.col[0]-tv0.col[0])+tv2.col[2]*(tv0.col[0]-tv1.col[0]);
				tc = tv0.col[0]*(tv1.col[1]-tv2.col[1])+tv1.col[0]*(tv2.col[1]-tv0.col[1])+tv2.col[0]*(tv0.col[1]-tv1.col[1]);
//				td = -(tv0.col[0]*(tv1.col[1]*tv2.col[2]-tv2.col[1]*tv1.col[2])+tv1.col[0]*(tv2.col[1]*tv0.col[2]-tv0.col[1]*tv2.col[2])+tv2.col[0]*(tv0.col[1]*tv1.col[2]-tv1.col[1]*tv0.col[2]));
				cdata->piece[n].normal.col[0] = ta;
				cdata->piece[n].normal.col[1] = tb;
				cdata->piece[n].normal.col[2] = tc;
				break;
			case PIECE_TYPE_INTERNAL_PANEL:
				for (i=0; i<cdata->piece[n].pnt_cnt-1; i++)
				{
					tv0 = rv_sub(cdata->piece[n].points[i],cdata->piece[n].centroid);
					tv1 = rv_sub(cdata->piece[n].points[i+1],cdata->piece[n].centroid);
					dv = rv_sub(tv1,tv0);
					sv = rv_add(tv1,tv0);
					cdata->piece[n].area += sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]));
				}
				tv0 = rv_sub(cdata->piece[n].points[cdata->piece[n].pnt_cnt-1],cdata->piece[n].centroid);
				tv1 = rv_sub(cdata->piece[n].points[0],cdata->piece[n].centroid);
				dv = rv_sub(tv1,tv0);
				sv = rv_add(tv1,tv0);
				cdata->piece[n].area += sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.;
				break;
			case PIECE_TYPE_BOX:
				tv0 = cdata->piece[n].points[0];
				tv1 = cdata->piece[n].points[1];
				tv2 = cdata->piece[n].points[2];
				tv3 = cdata->piece[n].points[6];
				ta = length_rv(rv_sub(tv1,tv0));
				tb = length_rv(rv_sub(tv2,tv1));
				tc = length_rv(rv_sub(tv3,tv0));
				cdata->piece[n].area = 2. * ta * tb + 4. * ta * tc;
				break;
			case PIECE_TYPE_CYLINDER:
				tv0 = cdata->piece[n].points[0];
				tv1 = cdata->piece[n].points[1];
				ta = length_rv(rv_sub(tv1,tv0));
				cdata->piece[n].area = DPI * cdata->piece[n].dim * cdata->piece[n].dim * ta;
				break;
			case PIECE_TYPE_SPHERE:
				cdata->piece[n].area = (4./3.) *DPI * pow(cdata->piece[n].dim,3.);
				break;
			}
		}
	}

	// Turn on power buses
	for (n=0; n<cdata->devspec.bus_cnt; n++)
	{
		cdata->devspec.bus[n]->flag |= DEVICE_FLAG_ON;
	}


	for (n=0; n<cdata->node.device_cnt; n++)
	{
		/*
	if (cdata->device[n].gen.pidx >= 0)
		{
		cdata->node.com.col[0] += cdata->piece[cdata->device[n].gen.pidx].centroid.col[0] * cdata->device[n].gen.mass;
		cdata->node.com.col[1] += cdata->piece[cdata->device[n].gen.pidx].centroid.col[1] * cdata->device[n].gen.mass;
		cdata->node.com.col[2] += cdata->piece[cdata->device[n].gen.pidx].centroid.col[2] * cdata->device[n].gen.mass;
		}
	if (cdata->device[n].gen.pidx >= 0)
		{
		cdata->piece[cdata->device[n].gen.pidx].heat += 300. * cdata->piece[cdata->device[n].gen.pidx].hcap * cdata->device[n].gen.mass;
		cdata->node.heat += 300. * cdata->piece[cdata->device[n].gen.pidx].hcap * cdata->device[n].gen.mass;
		}
	cdata->node.mass += cdata->device[n].gen.mass;
	*/
		cdata->device[n].gen.temp = 300.;
		cdata->device[n].gen.flag |= DEVICE_FLAG_ON;
		cdata->device[n].gen.amp = cdata->device[n].gen.amp;
		cdata->device[n].gen.volt = cdata->device[n].gen.volt;
		if (cdata->device[n].gen.bidx < cdata->devspec.bus_cnt && cdata->devspec.bus[cdata->device[n].gen.bidx]->volt < cdata->device[n].gen.volt)
			cdata->devspec.bus[cdata->device[n].gen.bidx]->volt = cdata->device[n].gen.volt;
	}

	cdata->physics.com = rv_smult(1./cdata->physics.mass,cdata->physics.com);
	cdata->physics.hcap /= cdata->physics.mass;

	for (n=0; n<cdata->piece.size(); n++)
	{
		if (cdata->piece[n].pnt_cnt)
		{
			cdata->piece[n].centroid = rv_sub(cdata->piece[n].centroid,cdata->physics.com);
			cdata->piece[n].normal = rv_sub(cdata->piece[n].normal,cdata->physics.com);
			tv0 = cdata->piece[n].centroid;
			tv0.col[0] = 0.;
			ta = length_rv(tv0);
			cdata->physics.moi.col[0] += cdata->piece[n].mass * ta * ta;
			tv0 = cdata->piece[n].centroid;
			tv0.col[1] = 0.;
			ta = length_rv(tv0);
			cdata->physics.moi.col[1] += cdata->piece[n].mass * ta * ta;
			tv0 = cdata->piece[n].centroid;
			tv0.col[2] = 0.;
			ta = length_rv(tv0);
			cdata->physics.moi.col[2] += cdata->piece[n].mass * ta * ta;
			for (i=0; i<cdata->piece[n].pnt_cnt; i++)
			{
				cdata->piece[n].points[i] = rv_sub(cdata->piece[n].points[i],cdata->physics.com);
			}
			if (cdata->piece[n].type == PIECE_TYPE_EXTERNAL_PANEL)
			{
				cdata->piece[n].twist = rv_cross(rv_sub(cdata->piece[n].centroid,cdata->piece[n].normal),cdata->piece[n].normal);
				cdata->piece[n].twist = rv_smult(-cdata->piece[n].area/(length_rv(cdata->piece[n].normal)*length_rv(cdata->piece[n].normal)),cdata->piece[n].twist);
				cdata->piece[n].shove = rv_zero();
				for (i=0; i<cdata->piece[n].pnt_cnt-1; i++)
				{
					tv0 = rv_sub(cdata->piece[n].points[i],cdata->piece[n].centroid);
					tv1 = rv_sub(cdata->piece[n].points[i+1],cdata->piece[n].centroid);
					ta = length_rv(tv0);
					for (j=0; j<=ta*100; j++)
					{
						tv2 = rv_smult(.01*j/ta,tv0);
						tv3 = rv_smult(.01*j/ta,tv1);
						dv = rv_sub(tv3,tv2);
						tb = length_rv(dv);
						for (k=0; k<tb*100; k++)
						{
							sv = rv_add(cdata->piece[n].centroid,rv_add(tv2,rv_smult(.01*k/tb,dv)));
							tc = 1./(length_rv(sv)*length_rv(sv));
							cdata->piece[n].shove = rv_add(cdata->piece[n].shove,rv_smult(tc,sv));
						}
					}
				}
				cdata->piece[n].shove = rv_smult(-1./10000.,cdata->piece[n].shove);
			}
		}
	}

	// Turn all CPU's on
	for (n=0; n<cdata->devspec.cpu_cnt; n++)
	{
		cdata->device[cdata->devspec.cpu[n]->cidx].gen.flag |= DEVICE_FLAG_ON;
	}

	// Turn on all IMU's
	for (n=0; n<cdata->devspec.imu_cnt; n++)
	{
		cdata->device[cdata->devspec.imu[n]->cidx].gen.flag |= DEVICE_FLAG_ON;
	}

	// Turn on all GPS's
	for (n=0; n<cdata->devspec.gps_cnt; n++)
	{
		cdata->device[cdata->devspec.gps[n]->cidx].gen.flag |= DEVICE_FLAG_ON;
	}

	cdata->node.battcap = 0.;
	for (n=0; n<cdata->devspec.batt_cnt; n++)
	{
		cdata->node.battcap += cdata->devspec.batt[n]->capacity;
		cdata->devspec.batt[n]->charge = cdata->devspec.batt[n]->capacity;
	}
	cdata->node.battlev = cdata->node.battcap;

	// Turn off reaction wheels
	for (i=0; i<cdata->devspec.rw_cnt; i++)
	{
		cdata->devspec.rw[i]->alp = cdata->devspec.rw[i]->omg = 0.;
	}

	// Set fictional torque to zero
	cdata->physics.ftorque = rv_zero();

	return 0;
}

//! Dump tab delimited database files
/*! Create files that can be read in to a relational database representing the various elements of
 * the satellite. Tables are created for Parts, Components, Devices, Temperature Sensors and
 * Power Buses.
*/
void create_databases(cosmosstruc *cdata)
{
	FILE *op;
	uint32_t i, j;
	piecestruc s;
	genstruc cs;
	rwstruc rws;
	imustruc ims;
	sttstruc sts;
	int32_t iretn;

	/*
 *	op = fopen("target.txt","w");
	fprintf(op,"gs_idx	gs_name	gs_pos_lat	gs_pos_lon	gs_pos_alt	gs_min gs_az	gs_el\n");
	for (i=0; i<cdata->node.target_cnt; i++)
	{
		fprintf(op,"%d\t%s\t%u\n",i,cdata->target[i].name,cdata->target[i].type);
	}
	fclose(op);
*/

	op = fopen("piece.txt","w");
	fprintf(op,"PartIndex\tName\tType\tTemperatureIndex\tComponentIndex\tMass\tEmissivity\tAbsorptivity\tDimension\tHeatCapacity\tHeatConductivity\tArea\tTemp\tHeat\tPointCount\tPoint1X\tPoint1Y\tPoint1Z\tPoint2X\tPoint2Y\tPoint2Z\tPoint3X\tPoint3Y\tPoint3Z\tPoint4X\tPoint4Y\tPoint4Z\tPoint5X\tPoint5Y\tPoint5Z\tPoint6X\tPoint6Y\tPoint6Z\tPoint7X\tPoint7Y\tPoint7Z\tPoint8X\tPoint8Y\tPoint8Z\n");
	for (i=0; i<cdata->piece.size(); i++)
	{
		s = cdata->piece[i];
		fprintf(op,"%d\t%s\t%d\t%d\t%.4f\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.6f\t%u",i,s.name,s.type,s.cidx,s.mass,s.emi,s.abs,s.dim,s.hcap,s.hcon,s.area,s.pnt_cnt);
		for (j=0; j<s.pnt_cnt; j++)
		{
			fprintf(op,"\t%.6f\t%.6f\t%.6f",s.points[j].col[0],s.points[j].col[1],s.points[j].col[2]);
		}
		fprintf(op,"\n");
	}
	fclose(op);

	op = fopen("comp.txt","w");
	fprintf(op,"comp_idx\tcomp_type\tcomp_didx\tcomp_pidx\tcomp_bidx\tcomp_namp\tcomp_nvolt\tcomp_amp\tcomp_volt\tcomp_temp\tcomp_on\n");
	for (i=0; i<cdata->node.device_cnt; i++)
	{
		cs = cdata->device[i].gen;
		fprintf(op,"%d\t%d\t%d\t%d\t%d\t%.15g\t%.15g\n",i,cs.type,cs.didx,cs.pidx,cs.bidx,cs.amp,cs.volt);
	}
	fclose(op);

	op = fopen("rw.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMomentX\tMomentY\tMomentZ\tMaxAngularSpeed\tAngularSpeed\tAngularAcceleration\n");
	for (i=0; i<cdata->devspec.rw_cnt; i++)
	{
		rws = *cdata->devspec.rw[i];
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,rws.cidx,rws.align.d.x,rws.align.d.y,rws.align.d.z,rws.align.w,rws.mom.col[0],rws.mom.col[1],rws.mom.col[2],rws.mxomg,rws.mxalp);
	}
	fclose(op);

	op = fopen("tsen.txt","w");
	fprintf(op,"TemperatureIndex\tCompIndex\tTemperature\n");
	for (i=0; i<cdata->devspec.tsen_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\n",i,cdata->devspec.tsen[i]->cidx,cdata->devspec.tsen[i]->temp);
	}
	fclose(op);

	op = fopen("strg.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tEfficiencyB\tEfficiencyM\tMaxPower\tPower\n");
	for (i=0; i<cdata->devspec.strg_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.strg[i]->cidx,cdata->devspec.strg[i]->effbase,cdata->devspec.strg[i]->effslope,cdata->devspec.strg[i]->maxpower,cdata->devspec.strg[i]->power);
	}
	fclose(op);

	op = fopen("batt.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tCapacity\tEfficiency\tCharge\n");
	for (i=0; i<cdata->devspec.batt_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.batt[i]->cidx,cdata->devspec.batt[i]->capacity,cdata->devspec.batt[i]->efficiency,cdata->devspec.batt[i]->charge);
	}
	fclose(op);

	op = fopen("ssen.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tQuadrantVoltageA\tQuadrantVoltageB\tQuadrantVoltageC\tQuadrantVoltageD\tAzimuth\tElevation\n");
	for (i=0; i<cdata->devspec.ssen_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.ssen[i]->cidx,cdata->devspec.ssen[i]->align.d.x,cdata->devspec.ssen[i]->align.d.y,cdata->devspec.ssen[i]->align.d.z,cdata->devspec.ssen[i]->align.w,cdata->devspec.ssen[i]->qva,cdata->devspec.ssen[i]->qvb,cdata->devspec.ssen[i]->qvc,cdata->devspec.ssen[i]->qvd,cdata->devspec.ssen[i]->azimuth,cdata->devspec.ssen[i]->elevation);
	}
	fclose(op);

	op = fopen("imu.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tPositionX\tPositionY\tPositionZ\tVelocityX\tVelocityY\tVelocityZ\tAccelerationX\tAccelerationY\tAccelerationZ\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tMagneticFieldX\tMagneticFieldY\tMagneticFieldZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
	for (i=0; i<cdata->devspec.imu_cnt; i++)
	{
		ims = *cdata->devspec.imu[i];
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,ims.cidx,ims.align.d.x,ims.align.d.y,ims.align.d.z,ims.align.w);
	}
	fclose(op);

	op = fopen("stt.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
	for (i=0; i<cdata->devspec.stt_cnt; i++)
	{
		sts = *cdata->devspec.stt[i];
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,sts.cidx,sts.align.d.x,sts.align.d.y,sts.align.d.z,sts.align.w);
	}
	fclose(op);

	op = fopen("mtr.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMagneticMomentX\tMagneticMomentY\tMagneticMomentZ\tMagneticField\n");
	for (i=0; i<cdata->devspec.mtr_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.mtr[i]->cidx,cdata->devspec.mtr[i]->align.d.x,cdata->devspec.mtr[i]->align.d.y,cdata->devspec.mtr[i]->align.d.z,cdata->devspec.mtr[i]->align.w,cdata->devspec.mtr[i]->npoly[0],cdata->devspec.mtr[i]->npoly[1],cdata->devspec.mtr[i]->npoly[2],cdata->devspec.mtr[i]->mom);
	}
	fclose(op);

	op = fopen("gps.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tLatitude\tLongitude\tAltitude\tVelocityX\tVelocityY\tVelocityZ\n");
	for (i=0; i<cdata->devspec.gps_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.gps[i]->cidx,cdata->devspec.gps[i]->position.col[0],cdata->devspec.gps[i]->position.col[1],cdata->devspec.gps[i]->position.col[2],cdata->devspec.gps[i]->velocity.col[0],cdata->devspec.gps[i]->velocity.col[1],cdata->devspec.gps[i]->velocity.col[2]);
	}
	fclose(op);

	op = fopen("cpu.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tLoad\tMemoryUse\tMemoryFree\tDiskUse\n");
	for (i=0; i<cdata->devspec.cpu_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.8g\t%.8g\t%.8g\t%.8g\t%.8g\n",i,cdata->devspec.cpu[i]->cidx,cdata->devspec.cpu[i]->maxmem,cdata->devspec.cpu[i]->maxdisk,cdata->devspec.cpu[i]->load,cdata->devspec.cpu[i]->mem,cdata->devspec.cpu[i]->disk);
	}
	fclose(op);

	op = fopen("pload.txt","w");
	fprintf(op,"DeviceIndex\tComponentIndex\tKeyCount");
	for (i=0; i<MAXPLOADKEYCNT; i++)
	{
		fprintf(op,"\tKey%d",i);
	}
	for (i=0; i<MAXPLOADKEYCNT; i++)
	{
		fprintf(op,"\tValue%d",i);
	}
	fprintf(op,"\n");
	for (i=0; i<cdata->devspec.pload_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%d",i,cdata->devspec.pload[i]->cidx,cdata->devspec.pload[i]->key_cnt);
		for (j=0; j<MAXPLOADKEYCNT; j++)
		{
			fprintf(op,"\t%d",cdata->devspec.pload[i]->keyidx[j]);
		}
		for (j=0; j<MAXPLOADKEYCNT; j++)
		{
			fprintf(op,"\t%.15g",cdata->devspec.pload[i]->keyval[j]);
		}
		fprintf(op,"\n");
	}
	fclose(op);

	op = fopen("motr.txt","w");
	fprintf(op,"motr_idx\tmotr_cidx\tmotr_spd\tmotr_rat\tmotr_rat\n");
	for (i=0; i<cdata->devspec.motr_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cdata->devspec.motr[i]->cidx,cdata->devspec.motr[i]->max,cdata->devspec.motr[i]->rat,cdata->devspec.motr[i]->spd);
	}
	fclose(op);

	op = fopen("swch.txt","w");
	fprintf(op,"swch_idx\tswch_cidx\n");
	for (i=0; i<cdata->devspec.swch_cnt; i++)
	{
		iretn = fscanf(op,"%*d\t%hu\n",&cdata->devspec.swch[i]->cidx);
		if (iretn < 1)
		{
			break;
		}
	}
	fclose(op);

	op = fopen("thst.txt","w");
	fprintf(op,"thst_idx\tthst_cidx\tthst_idx\tthst_isp\tthst_flw\n");
	for (i=0; i<cdata->devspec.thst_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cdata->devspec.thst[i]->cidx,cdata->devspec.thst[i]->isp,cdata->devspec.thst[i]->flw);
	}
	fclose(op);

	op = fopen("prop.txt","w");
	fprintf(op,"prop_idx\tprop_cidx\tprop_cap\tprop_lev\n");
	for (i=0; i<cdata->devspec.prop_cnt; i++)
	{
		fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cdata->devspec.prop[i]->cidx,cdata->devspec.prop[i]->cap,cdata->devspec.prop[i]->lev);
	}
	fclose(op);
}


//! Load Track list
/*! Load the file target.ini into an array of ::targetstruc. Space for the array is automatically allocated
 * and the number of items returned.
 *	\param cdata Pointer to ::cosmosstruc holding the vector of targets.
 *	\return Number of items loaded.
*/

int load_target(cosmosstruc *cdata)
{
	FILE *op;
	string fname;
	char inb[JSON_MAX_DATA];
	uint32_t count;

	fname = get_cnodedir(cdata->node.name) + "/target.ini";
	count = 0;
	if ((op=fopen(fname.c_str(),"r")) != NULL)
	{
		while (count < cdata->target.size() && fgets(inb,JSON_MAX_DATA,op) != NULL)
		{
			json_addentry((char *)"target_range",count,-1,(ptrdiff_t)offsetof(targetstruc,range)+count*sizeof(targetstruc),JSON_TYPE_DOUBLE,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_close",count,-1,(ptrdiff_t)offsetof(targetstruc,close)+count*sizeof(targetstruc),JSON_TYPE_DOUBLE,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_utc",count,-1,(ptrdiff_t)offsetof(targetstruc,utc)+count*sizeof(targetstruc),JSON_TYPE_DOUBLE,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_name",count,-1,(ptrdiff_t)offsetof(targetstruc,name)+count*sizeof(targetstruc),JSON_TYPE_NAME,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_type",count,-1,(ptrdiff_t)offsetof(targetstruc,type)+count*sizeof(targetstruc),JSON_TYPE_UINT16,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_azfrom",count,-1,(ptrdiff_t)offsetof(targetstruc,azfrom)+count*sizeof(targetstruc),JSON_TYPE_FLOAT,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_azto",count,-1,(ptrdiff_t)offsetof(targetstruc,azto)+count*sizeof(targetstruc),JSON_TYPE_FLOAT,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_elfrom",count,-1,(ptrdiff_t)offsetof(targetstruc,elfrom)+count*sizeof(targetstruc),JSON_TYPE_FLOAT,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_elto",count,-1,(ptrdiff_t)offsetof(targetstruc,elto)+count*sizeof(targetstruc),JSON_TYPE_FLOAT,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_min",count,-1,(ptrdiff_t)offsetof(targetstruc,min)+count*sizeof(targetstruc),JSON_TYPE_FLOAT,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_loc",count,-1,(ptrdiff_t)offsetof(targetstruc,loc)+count*sizeof(targetstruc),JSON_TYPE_LOC,JSON_GROUP_TARGET,cdata);
			json_addentry((char *)"target_loc_pos_geod",count,-1,(ptrdiff_t)offsetof(targetstruc,loc.pos.geod)+count*sizeof(targetstruc),JSON_TYPE_POS_GEOD,JSON_GROUP_TARGET,cdata);
            if (json_parse(inb,cdata) >= 0)
			{
//				loc_update(&cdata->target[count].loc);
				++count;
			}
		}
		fclose(op);
		return (count);
	}
	else
		return 0;
}

//! Update Track list
/*! For each entry in the Track list, calculate the azimuth, elevation and range to and
 *from the current base location.
 *	\param cdata Pointer to ::cosmosstruc to use.
 *	\return 0, otherwise negative error.
 */
int update_target(cosmosstruc *cdata)
{
	rvector topo, dv, ds;
	for (uint32_t i=0; i<cdata->target.size(); ++i)
	{
		loc_update(&cdata->target[i].loc);
		geoc2topo(cdata->target[i].loc.pos.geod.s,cdata->node.loc.pos.geoc.s,&topo);
		topo2azel(topo,&cdata->target[i].azto,&cdata->target[i].elto);
		geoc2topo(cdata->node.loc.pos.geod.s,cdata->target[i].loc.pos.geoc.s,&topo);
		topo2azel(topo,&cdata->target[i].azfrom,&cdata->target[i].elfrom);
		ds = rv_sub(cdata->target[i].loc.pos.geoc.s,cdata->node.loc.pos.geoc.s);
		cdata->target[i].range = length_rv(ds);
		dv = rv_sub(cdata->target[i].loc.pos.geoc.v,cdata->node.loc.pos.geoc.v);
		cdata->target[i].close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
	}
	for (uint32_t i=cdata->target.size(); i<100; ++i)
	{
		cdata->target[i].azto = NAN;
		cdata->target[i].elto = NAN;
		cdata->target[i].azfrom = NAN;
		cdata->target[i].elfrom = NAN;
		cdata->target[i].range = NAN;
		cdata->target[i].close = NAN;
	}
	return 0;
}

//! Load Event Dictionary
/*! Read a specific  event dictionary for a specific node. The
* dictionary is stored as multiple condition based JSON event strings
* in a file of the specified name, in the cdata directory of the specified node.
* The result is a vector of event structures.
 *	\param cdata Pointer to ::cosmosstruc to use.
 *	\param dict Reference to vector of ::shorteventstruc.
 *	\param file Name of dictionary file.
 *	\return Number of items loaded.
*/
int load_dictionary(vector<shorteventstruc> &dict, cosmosstruc *cdata, char *file)
{
	FILE *op;
	char inb[JSON_MAX_DATA];
	shorteventstruc sevent;
	jsonhandle handle;
	int32_t iretn;

	string fname = (get_cnodedir(cdata->node.name) + "/") + file;
	if ((op=fopen(fname.c_str(),"r")) != NULL)
	{
		while (fgets(inb,JSON_MAX_DATA,op) != NULL)
		{
			json_clear_cosmosstruc(JSON_MAP_EVENT, cdata);
            if (json_parse(inb,cdata) > 0)
			{
				if ((iretn=json_equation_map(cdata->event[0].l.condition, cdata, &handle)) < 0)
					continue;
				sevent = cdata->event[0].s;
				sevent.utcexec = 0.;
				sevent.handle = handle;
				dict.push_back(sevent);
			}
		}
		fclose(op);
        dict.shrink_to_fit();
		return (dict.size());
	}
	else
		return 0;
}

//! Load tab delimited database files
/*! Read tab delimited files that make up a relational database representing
 * the various elements of the satellite. Tables represent Parts, Components,
 * Devices, Temperature Sensors and Power Buses.
	\param name Node name
*/
void load_databases(char *name, uint16_t type, cosmosstruc *cdata)
{
	FILE *op;
	int i, j, iretn;
	piecestruc *s;
	genstruc *cs;
	rwstruc *rws;
	imustruc *ims;
	sttstruc *sts;

	strcpy(cdata->node.name, name);
	cdata->node.type = type;

	/*
 *	op = fopen("target.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%s\t%hu\n",&cdata->target[i].name[0],&cdata->target[i].type);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->node.target_cnt = i;
	}
*/

	op = fopen("piece.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			s = &cdata->piece[i];
			iretn = fscanf(op,"%*d\t%s\t%hu\t%hu\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%hu",&s->name[0],&s->type,&s->cidx,&s->mass,&s->emi,&s->abs,&s->dim,&s->hcap,&s->hcon,&s->area,&s->pnt_cnt);
			if (iretn > 0)
			{
				for (j=0; j<s->pnt_cnt; j++)
				{
					iretn = fscanf(op,"\t%lg\t%lg\t%lg",&s->points[j].col[0],&s->points[j].col[1],&s->points[j].col[2]);
				}
				iretn = fscanf(op,"\n");
				++i;
			}
		} while (iretn > 0);
		fclose(op);
		cdata->node.piece_cnt = i;
	}

	op = fopen("comp.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			cs = &cdata->device[i].gen;
			iretn = fscanf(op,"%*d\t%hu\t%hu\t%hu\t%hu\t%g\t%g\n",&cs->type,&cs->didx,&cs->pidx,&cs->bidx,&cs->namp,&cs->volt);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->node.device_cnt = i;
	}

	op = fopen("ant.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&cdata->devspec.ant[i]->cidx,&cdata->devspec.ant[i]->align.d.x,&cdata->devspec.ant[i]->align.d.y,&cdata->devspec.ant[i]->align.d.z,&cdata->devspec.ant[i]->align.w);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.ant_cnt = i;
	}

	op = fopen("batt.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%g\t%g\n",&cdata->devspec.batt[i]->cidx,&cdata->devspec.batt[i]->capacity,&cdata->devspec.batt[i]->efficiency);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.batt_cnt = i;
	}

	op = fopen("cpu.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%f\t%f\t%f\n",&cdata->devspec.cpu[i]->cidx,&cdata->devspec.cpu[i]->maxmem,&cdata->devspec.cpu[i]->maxdisk,&cdata->devspec.cpu[i]->maxload);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.cpu_cnt = i;
	}

	op = fopen("gps.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\n",&cdata->devspec.gps[i]->cidx);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.gps_cnt = i;
	}

	op = fopen("imu.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			ims = cdata->devspec.imu[i];
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&ims->cidx,&ims->align.d.x,&ims->align.d.y,&ims->align.d.z,&ims->align.w);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.imu_cnt = i;
	}

	op = fopen("mtr.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\t%g\t%g\t%g\t%g\n",&cdata->devspec.mtr[i]->cidx,&cdata->devspec.mtr[i]->align.d.x,&cdata->devspec.mtr[i]->align.d.y,&cdata->devspec.mtr[i]->align.d.z,&cdata->devspec.mtr[i]->align.w,&cdata->devspec.mtr[i]->npoly[0],&cdata->devspec.mtr[i]->npoly[1],&cdata->devspec.mtr[i]->npoly[2],&cdata->devspec.mtr[i]->mxmom);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.mtr_cnt = i;
	}

	op = fopen("pload.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%u\t%hu",&cdata->devspec.pload[i]->cidx,&cdata->devspec.pload[i]->bps,&cdata->devspec.pload[i]->key_cnt);
			for (j=0; j<cdata->devspec.pload[i]->key_cnt; j++)
			{
//				iretn = fscanf(op,"\t%s",&cdata->devspec.pload[i]->key[j][0]);
			}
			iretn = fscanf(op,"\n");
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.pload_cnt = i;
	}

	op = fopen("rw.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			rws = cdata->devspec.rw[i];
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\t%g\t%g\n",&rws->cidx,&rws->align.d.x,&rws->align.d.y,&rws->align.d.z,&rws->align.w,&rws->mom.col[0],&rws->mom.col[1],&rws->mom.col[2],&rws->mxomg,&rws->mxalp);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.rw_cnt = i;
	}

	op = fopen("ssen.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&cdata->devspec.ssen[i]->cidx,&cdata->devspec.ssen[i]->align.d.x,&cdata->devspec.ssen[i]->align.d.y,&cdata->devspec.ssen[i]->align.d.z,&cdata->devspec.ssen[i]->align.w);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.ssen_cnt = i;
	}

	op = fopen("strg.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%g\t%g\t%g\n",&cdata->devspec.strg[i]->cidx,&cdata->devspec.strg[i]->effbase,&cdata->devspec.strg[i]->effslope,&cdata->devspec.strg[i]->maxpower);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.strg_cnt = i;
	}

	op = fopen("stt.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			sts = cdata->devspec.stt[i];
			iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&sts->cidx,&sts->align.d.x,&sts->align.d.y,&sts->align.d.z,&sts->align.w);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.stt_cnt = i;
	}

	op = fopen("tsen.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\n",&cdata->devspec.tsen[i]->cidx);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.tsen_cnt = i;
	}

	op = fopen("motr.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%g\t%g\n",&cdata->devspec.motr[i]->cidx,&cdata->devspec.motr[i]->max,&cdata->devspec.motr[i]->rat);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.motr_cnt = i;
	}

	op = fopen("swch.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\n",&cdata->devspec.swch[i]->cidx);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.swch_cnt = i;
	}

	op = fopen("thst.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%g\t%lg\t%lg\t%lg\t%lg\n",&cdata->devspec.thst[i]->cidx,&cdata->devspec.thst[i]->isp,&cdata->devspec.thst[i]->align.d.x,&cdata->devspec.thst[i]->align.d.y,&cdata->devspec.thst[i]->align.d.z,&cdata->devspec.thst[i]->align.w);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.thst_cnt = i;
	}

	op = fopen("prop.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\t%g\n",&cdata->devspec.prop[i]->cidx,&cdata->devspec.prop[i]->cap);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.prop_cnt = i;
	}

	op = fopen("tcv.txt","r");
	if (op != NULL)
	{
		i = 0;
		do
		{
			iretn = fscanf(op,"%*d\t%hu\n",&cdata->devspec.tcv[i]->cidx);
			if (iretn > 0)
				++i;
		} while (iretn > 0);
		fclose(op);
		cdata->devspec.tcv_cnt = i;
	}

}

//! Calculate current Events.
/*! Using the provided Event Dictionary, calculate all the Events for
* current state of the provided Node. The Event Dictionary will be
* updated with any new values so that repeating Events can be properly
* assessed.
*	\param dictionary Reference to vector of ::shortenventstruc representing event dictionary.
*	\param cdata Pointer to ::cosmosstruc.
*	\param events Reference to vector of ::shortenventstruc representing events.
*	\return Number of events created.
*/
uint32_t calc_events(vector<shorteventstruc> &dictionary, cosmosstruc *cdata, vector<shorteventstruc> &events)
{
	double value;
//	const char *cp;
	char *sptr, *eptr;

	events.resize(0);
	for (uint32_t k=0; k<dictionary.size(); ++k)
	{
		if (!isnan(value=json_equation_handle(&dictionary[k].handle, cdata)) && value != 0. && dictionary[k].value == 0.)
		{
			dictionary[k].utc = cdata->node.loc.utc;
			events.push_back(dictionary[k]);
			string tstring = json_get_string_name(dictionary[k].data, cdata);
			strcpy(events[events.size()-1].data, tstring.c_str());
			strcpy(events[events.size()-1].node,cdata->node.name);
			if ((sptr=strstr(events[events.size()-1].name,"${")) != NULL && (eptr=strstr(sptr,"}")) != NULL)
			{
				*eptr = 0;
				tstring = json_get_string_name(sptr+2, cdata);
				strcpy(sptr, tstring.c_str());
			}
		}
		dictionary[k].value = value;
	}

    events.shrink_to_fit();
	return (events.size());
}

