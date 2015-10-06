#include "configCosmos.h"
#include "vn100_lib.h"
#include "timelib.h"
#include "agentlib.h"
#include "datalib.h"
#include "elapsedtime.hpp"
#include "physicslib.h"

#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

vn100_handle handle;
cosmosstruc *cdata;
locstruc iloc, sloc;

string nodename = "nmsr";
string agentname = "find_initial_state";

ElapsedTime et, pt;

int main(int argc, char *argv[])
{
	// terms for debugging and time
	int32_t iretn;
	double lmjd;

	// terms for position and velocity
	double tt[4] = { 0 }; // time temp
    double ps[3][4];    // past position
	double pv[3][4] = { 0 }; // past velocity

	// terms for fitting acceleration to polynomial
	double at[3][4], a[3][4];

	// terms for least squares linear fit calculation
	rvector w_sum[3], w_avg, wt, wct, b, m;
	long c_sum, c_avg, ct, cct;
	int c[3];

	// terms for initial attitude and acceleration
//	rvector avg_body_accel = rv_zero();
//	quaternion avg_body_att = q_zero();
	quaternion body2geoc = q_zero();

	// terms for defining mission phase states
	bool avg_prd = true;
	int program_cycles = 0;
	const int INIT_P = 25;

	// terms for attitude calculation
	uvector au, t, p;
	rvector atttem;
	rmatrix skewt;
	matrix2d matrix2t;
	uvector vectort, vectort2;

	// Establish the command channel and heartbeat
	if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
	nodename.c_str(),
	agentname.c_str(),
	0.1,
	0,
	AGENTMAXBUFFER)))
	{
		cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}
	else
		cout << "Finding Initial State" << endl;

	// open state.ini file, holds the starting position and MJD
	struct stat fstat; FILE* fdes; string fname = get_nodedir(nodename);
	fname += "/state.ini";
	if ((iretn=stat(fname.c_str(), &fstat)) == 0 && (fdes=fopen(fname.c_str(),"r")) != NULL)
	{
		char* ibuf = (char *)calloc(1,fstat.st_size+1);
		size_t nbytes = fread(ibuf, 1, fstat.st_size, fdes);

		if (nbytes)
			json_parse(ibuf,cdata);

		free(ibuf);
		loc_update(&cdata[0].node.loc);
		iloc = cdata[0].node.loc; // place in both iloc and sloc
		sloc = iloc;

		cout << "\nInitial State Vector Vel: [" << iloc.pos.eci.v.col[0] << ", " << iloc.pos.eci.v.col[1] <<  ", " << iloc.pos.eci.v.col[2] << "] km" << endl;
		cout << "Initial MJD: " << setprecision(10) << iloc.utc << "\n" << endl;
	}
	else
	{
		printf("Unable to open state.ini\n");
		exit (1);
	}

	// Set IMU SOH string
	char sohstring[2000];
	sprintf(sohstring, "{\"node_loc_pos_geod\",\"node_loc_att_geod\",\"node_loc_pos_geoc\",\"node_loc_att_geoc\"");
	for (uint16_t i=0; i<cdata->devspec.imu_cnt; ++i)
		sprintf(&sohstring[strlen(sohstring)], ",\"device_imu_utc_%03d\",\"device_imu_temp_%03d\",\"device_imu_mag_%03d\",\"device_imu_bdot_%03d\",\"device_imu_omega_%03d\",\"device_imu_accel_%03d\"",i,i,i,i,i,i);
	agent_set_sohstring(cdata, sohstring);

    // determine if IMU names through the argument
    switch (argc)
    {
    case 2: // one imu
        if ((iretn=vn100_connect(argv[1], &handle)) < 0)
        {
            printf("Failed to open VN100 on %s, error %d\n",argv[1],iretn);
            exit (iretn);
        }
    case 3: // two imus
        if ((iretn=vn100_connect(argv[2], &handle)) < 0)
        {
            printf("Failed to open VN100 on %s, error %d\n",argv[2],iretn);
            exit (iretn);
        }
        if ((iretn=vn100_connect(argv[1], &handle)) < 0)
        {
            printf("Failed to open VN100 on %s, error %d\n",argv[1],iretn);
            exit (iretn);
        }
        break;
    default: // incorrect format
        printf("Usage: find_initial_state device1 device2");
        exit (1);
        break;
    }

	// connect to IMU and check measurements
	iretn = vn100_asynchoff(&handle);
	iretn = vn100_measurements(&handle);
	if (iretn < 0)
		printf("Fail: Unable to acquire measurements: %d\n", iretn);

	// print values to a file delimited by spaces
//	ofstream txtout ("../../../imu-data/" + to_string(currentmjd()) + "_fisdata.txt");
	ofstream txtout ("/nmsr/" + to_string(currentmjd()) + "_fisdata.txt");
	txtout << "BODY.ACCEL.X BODY.ACCEL.Y BODY.ACCEL.Z BODY.OMEGA.X BODY.OMEGA.Y BODY.OMEGA.Z CORRE.OMEGA.X CORRE.OMEGA.Y CORRE.OMEGA.Z BODY.MAG.X BODY.MAG.Y BODY.MAG.Z GEOC.ACCEL.X GEOC.ACCEL.Y GEOC.ACCEL.Z GEOC.VEL.X GEOC.VEL.Y GEOC.VEL.Z GEOC.POS.X GEOC.POS.Y GEOC.POS.Z CYCLES MJD" << endl;

	// streams for printing omega to a temp text file and reading it back in
	ofstream omegaout ("omegaout.txt", ios_base::trunc);
	ifstream omegain;

	while(agent_running(cdata))
	{
		// number of imu readings
		const int IMU_AVERAGE = 10;

		lmjd = currentmjd(0.);

		// reset imu struct to zero
		imustruc simu;
		simu.mag = rv_zero();
		simu.omega = rv_zero();
		simu.accel = rv_zero();
		simu.gen.temp = 0.;

		// collect multiple measurements per cycle
		for(int i = 0; i < IMU_AVERAGE; i++)
		{
			vn100_measurements(&handle);

			simu.omega = rv_add(simu.omega, handle.imu.omega);
			simu.accel = rv_add(simu.accel, handle.imu.accel);
			simu.mag = rv_add(simu.mag, handle.imu.mag);
			simu.gen.temp += handle.imu.gen.temp;
		}

		// average the readings to reduce the noise
		simu.omega = rv_smult(1./IMU_AVERAGE, simu.omega);
		simu.accel = rv_smult(1./IMU_AVERAGE, simu.accel);
		simu.mag = rv_smult(1./IMU_AVERAGE, simu.mag);
		simu.gen.temp /= IMU_AVERAGE;
		simu.gen.utc = currentmjd();

		*cdata->devspec.imu[0] = simu;

		// first, collect data to find average attitude and acceleration
		if (program_cycles < INIT_P)
		{
			if(program_cycles == 0)
				cout <<  "Initialization Period... Don't move IMU." << endl;

			rvector sourcea, sourceb, targeta, targetb, avg_body_accel;
			quaternion avg_body_att;

			targeta = rv_smult(-1., iloc.pos.geoc.s);
			targetb = iloc.bearth;
			sourcea = rv_smult(-1., simu.accel);
			sourceb = simu.mag;

			// calculate attitude using magnetic field and gravity
			body2geoc = q_add(body2geoc, q_transform_for (sourcea, sourceb, targeta, targetb));

			avg_body_accel = rv_add(simu.accel, avg_body_accel);
			avg_body_att = q_add(body2geoc, avg_body_att);

			// divide by INIT_P to get the average attitude and acceleration
			if (program_cycles == INIT_P - 1)
			{
				avg_body_accel = rv_smult(1./INIT_P, avg_body_accel);
				avg_body_att = q_smult(1./INIT_P, avg_body_att);
				avg_body_att = q_eye();/////////////////////////////////////////////////////////////////////////////
				q_normalize(&avg_body_att);

				 // initial acceleration in geoc frame
				sloc.pos.geoc.a = transform_q(q_conjugate(avg_body_att), avg_body_accel);
				sloc.pos.geoc.utc = lmjd;
				sloc.pos.geoc.pass++;

				// find initial attitude
				sloc.att.geoc.s = avg_body_att;
				sloc.att.geoc.utc = lmjd;
				sloc.att.geoc.pass++;
				pos_geoc (&sloc);

				// update previous position, avoids using zero in calculation
				for(int i = 0; i <= 2; i++)
				{
					ps[i][3] = sloc.pos.geoc.s.col[i];
					ps[i][2] = sloc.pos.geoc.s.col[i];
					ps[i][1] = sloc.pos.geoc.s.col[i];
					ps[i][0] = sloc.pos.geoc.s.col[i];
				}

				// print initial attitude and acceleration
				char buffer[300];
				sprintf (buffer, "\nInitial Geoc Attitude # %f %f %f (%f) \nInitial Geoc Acceleration # %f %f %f [%f]", sloc.att.geoc.s.d.x, sloc.att.geoc.s.d.y, sloc.att.geoc.s.d.z, sloc.att.geoc.s.w, sloc.pos.geoc.a.col[0], sloc.pos.geoc.a.col[1], sloc.pos.geoc.a.col[2], length_rv(sloc.pos.geoc.a));
				cout << buffer << endl;

				// ensure sums for averages start at zero
				wct = rv_zero();
				cct = 0;
				c_sum = 0;
			}

			// start timer for tt and t.a4
			et.start();

			program_cycles++;
		}

		// enter averaging period, moves onto estimation period if large force is detected
//		else if (abs(length_rv(simu.accel) - length_rv(avg_body_accel)) < 10. && avg_prd)
		else if (program_cycles < 250 && program_cycles >= INIT_P)
		{
			if (program_cycles == INIT_P)
				cout << "\nEntering Averaging Period" << endl;

			// find time values early to avoid using zero in line fitting
			tt[3] = tt[2]; tt[2] = tt[1]; tt[1] = tt[0];
			tt[0] = et.split();

			// push back sum and c, take the 2nd to last kept value to avoid omega spike
			c[2] = c[1]; c[1] = c[0];
			w_sum[2] = w_sum[1]; w_sum[1] = w_sum[0];

			program_cycles++;

			// get sum for linear regression
			c[0] = program_cycles - INIT_P;
			w_sum[0] = rv_add(w_sum[0], simu.omega);

			// write each omega value to a text file, allows for less intensive variable storage for omega
			omegaout << setprecision(10) << simu.omega.col[0] << " " << simu.omega.col[1] << " " << simu.omega.col[2] << endl;

			iloc = sloc;

			pt.start();
		}

		// enter estimation period, calculates geocentric attitude and position terms
		else
		{
			rvector c_ome;
			static rvector ome_os;

			if (avg_prd)
			{
				// not writing, now reading the omega files
				string line;
				omegaout.close();
				omegain.open("omegaout.txt");

				// calculate mean of omega and cycles
				w_avg = rv_smult(1./c[2], w_sum[2]);
				c_sum = (c[2] * (c[2] + 1)) / 2;
				c_avg = c_sum / c[2];

				// retrieve individual raw omega values, calculate sums
				for (int i = 0; i <= c[2]; i++)
				{
					rvector ot;

					// get line of data and put into stringstream
					getline(omegain, line, '\n');
					istringstream iss (line);
					vector<string> ome_str;

					// break line of data into three values
					while(iss >> line)
						ome_str.push_back(line);

					// convert to double, reset ome_str
					ot.col[0] = stod(ome_str[0]);
					ot.col[1] = stod(ome_str[1]);
					ot.col[2] = stod(ome_str[2]);
					ome_str.clear();

					// calculate terms needed for finding m
					wt = (rv_sub(ot, w_avg));
					ct = i - c_avg;
					wct = rv_add(wct, rv_smult(ct, wt));
					cct += ct * ct;
				}

				// calculate m, b, then calculate the offset using the equation
				m = rv_smult(1./cct, wct);
				b = rv_sub(w_avg, rv_smult(c_avg, m));

				// offset only uses b, m only matters over long periods of time
				ome_os = b;

				// delete temp omega file
				if (remove("omegaout.txt") != 0)
					cout << "Could not delete temp file." << endl;
			}

//			iloc.att.geoc.a = ome_os; ////////// NOT CORRECT DO NOT EVER DO THIS NO
			c_ome = rv_sub(simu.omega, ome_os);
//			iloc.att.geoc.v = c_ome; ////////// NOT CORRECT DO NOT EVER DO THIS NO

			// calculate time based on last recorded time value;
			tt[3] = tt[2]; tt[2] = tt[1]; tt[1] = tt[0]; tt[0] = et.split();
			t.a4[3] = 0;
			t.a4[2] = tt[2] - tt[3];
			t.a4[1] = tt[1] - tt[3];
			t.a4[0] = tt[0] - tt[3];

			// calculate new attitude based on omega
			double timestep = t.a4[0] / 100.;
			for (int i = 0; i < 100; i++)
			{
				// scale omega by 1/3 and timestep
				atttem = rv_smult(timestep * (1./3), c_ome);
				skewt = rm_skew(atttem);

				matrix2t.rows = matrix2t.cols = 4;
				for (int l = 0; l < 3; l++)
				{
					matrix2t.array[3][l] = -atttem.col[l];
					matrix2t.array[l][3] = atttem.col[l];
					for (int m = 0; m < 3; m++)
						matrix2t.array[l][m] = -skewt.row[l].col[m];
				}
				matrix2t.array[3][3] = 0.;

				// find derivative to get attitude
				vectort.m1.cols = 4;
				vectort.q = iloc.att.geoc.s;
				vectort.m1 = m1_smult(0.5,m1_mmult(matrix2t,vectort.m1));

				// add steps together
				iloc.att.geoc.s = q_add(iloc.att.geoc.s,q_smult(0.5, q_add(vectort.q, vectort2.q)));
//				iloc.att.geoc.s = q_add(iloc.att.geoc.s, vectort.q);
				q_normalize(&iloc.att.geoc.s); // ends up in geoc frame

				vectort2 = vectort;
			}

			// calculate omega and acceleration in geocentric frame with new attitude
//			iloc.att.geoc.v = transform_q(q_conjugate(iloc.att.geoc.s), c_ome);
			iloc.pos.geoc.a = rv_sub(transform_q(q_conjugate(iloc.att.geoc.s), simu.accel), sloc.pos.geoc.a);
//			iloc.pos.geoc.a = transform_q(q_conjugate(iloc.att.geoc.s), simu.accel);

			// integrate acceleration to find velocity and position
			for(int j = 0; j <= 2; j++)
			{
				// avoid using zero when fitting to polynomial
				if (avg_prd)
				{
					a[j][2] = iloc.pos.geoc.a.col[j];
					a[j][1] = iloc.pos.geoc.a.col[j];
					a[j][0] = iloc.pos.geoc.a.col[j];
				}

				// push back acceleration, find latest value
				a[j][3] = a[j][2]; a[j][2] = a[j][1]; a[j][1] = a[j][0]; a[j][0] = iloc.pos.geoc.a.col[j];
				for(int l = 0; l <= 3; l++)
					au.a4[l] = a[j][l];

				// fit to polynomial and move acceleration terms into at
				p = rv_fitpoly(t, au, 3);
				for (int l = 0; l <= 3; l++)
					at[j][l] = p.a4[l];

				// push back previous position and velocity
				pv[j][3] = pv[j][2]; pv[j][2] = pv[j][1]; pv[j][1] = pv[j][0];
				ps[j][3] = ps[j][2]; ps[j][2] = ps[j][1]; ps[j][1] = ps[j][0];

				// calculate velocity and position
				iloc.pos.geoc.v.col[j] = (pv[j][3]) + (at[j][0] * t.a4[0]) + (at[j][1] * (1./2) * pow(t.a4[0], 2)) + (at[j][2] * (1./3) * pow(t.a4[0], 3)) + (at[j][3] * (1./4) * pow(t.a4[0], 4));
				iloc.pos.geoc.s.col[j] = (ps[j][3]) + (pv[j][3] * t.a4[0]) + (at[j][0] * (1./2) * pow(t.a4[0], 2)) + (at[j][1] * (1./6) * pow(t.a4[0], 3)) + (at[j][2] * (1./12) * pow(t.a4[0], 4)) + (at[j][3] * (1./20) * pow(t.a4[0], 5));

				// update with latest velocity and position
				pv[j][0] = iloc.pos.geoc.v.col[j];
				ps[j][0] = iloc.pos.geoc.s.col[j];

			}

			if (pt.split() > 10)
			{
				// formatted output of values
				char buffer[300];
				sprintf(buffer, "Geoc Accel (x, y, z): \t\t%.3g , %.3g , %.3g \nGeoc Vel (x, y, z): \t\t%.3g , %.3g , %.3g \nGeoc Pos (x, y, z): \t\t%.10g , %.10g , %.10g \nGeoc Att (x, y, z, theta): \t%.3g , %.3g, %.3g , %.3g \nBody Omega* (x, y, z): \t%.3g , %.3g , %.3g \nBody Accel (x, y, z): \t\t%.3g , %.3g , %.3g \nBody Mag (x, y, z): \t\t %.3g , %.3g, %.3g \n",
					iloc.pos.geoc.a.col[0], iloc.pos.geoc.a.col[1], iloc.pos.geoc.a.col[2],
					iloc.pos.geoc.v.col[0], iloc.pos.geoc.v.col[1], iloc.pos.geoc.v.col[2],
					iloc.pos.geoc.s.col[0], iloc.pos.geoc.s.col[1], iloc.pos.geoc.s.col[2],
					iloc.att.geoc.s.d.x, iloc.att.geoc.s.d.y, iloc.att.geoc.s.d.z, iloc.att.geoc.s.w,
					c_ome.col[0], c_ome.col[1], c_ome.col[2],
					simu.accel.col[0], simu.accel.col[1], simu.accel.col[2],
					simu.mag.col[0], simu.mag.col[1], simu.mag.col[2]);
				cout << "Cycles: \t\t\t" << program_cycles << "\n" << buffer << endl;

				// output values to file, uses space as delimiter
				txtout << simu.accel.col[0] << " " << simu.accel.col[1] << " " << simu.accel.col[2] << " "
					<< simu.omega.col[0] << " " << simu.omega.col[1] << " " << simu.omega.col[2] << " "
					<< c_ome.col[0] << " " << c_ome.col[1] << " " << c_ome.col[2] << " "
					<< simu.mag.col[0] << " " << simu.mag.col[1] << " " << simu.mag.col[2] << " "
					<< iloc.pos.geoc.a.col[0] << " " << iloc.pos.geoc.a.col[1] << " " << iloc.pos.geoc.a.col[2] << " "
					<< iloc.pos.geoc.v.col[0] << " " << iloc.pos.geoc.v.col[1] << " " << iloc.pos.geoc.v.col[2] << " "
					<< iloc.pos.geoc.s.col[0] << " " << iloc.pos.geoc.s.col[1] << " " << iloc.pos.geoc.s.col[2] << " "
					<< program_cycles << " " << currentmjd() << endl;
				pt.start();
			}

			// propagate to other positions and update cdata
			iloc.pos.geoc.utc = lmjd; iloc.att.geoc.utc = lmjd;
			iloc.pos.geoc.pass++; iloc.att.geoc.pass++;
			pos_geoc (&iloc);
			cdata->node.loc = iloc;

			program_cycles++;

			// prevent from entering averaging period again
			avg_prd = false;
		}
	}

}
