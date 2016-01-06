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

#include <vector>
#include <iostream>
//#include <Eigen/Dense>

//using namespace std;
//using namespace Eigen;

#define EKF_NUM_STATES 7
#define EKF_NUM_ACTUATORS 4
#define EKF_NUM_SENSORS 3

//class kalman_struc
//{
//public:
//	std::vector< double > time;
//	std::vector< Matrix < double, EKF_NUM_STATES, EKF_NUM_STATES > > gain;
//	std::vector< Matrix < double, EKF_NUM_STATES, 1              > > state;
//	std::vector< Matrix < double, EKF_NUM_STATES, EKF_NUM_STATES > > error;
//};

//void extended_kalman_filter
//(
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& A,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_ACTUATORS         >& B,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& H,
//	Matrix<double, EKF_NUM_STATES, 1         >& X,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& P,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& Q,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& R,
//	Matrix<double, EKF_NUM_STATES, 1         >& z,
//	Matrix<double, EKF_NUM_ACTUATORS         , 1         >& u,
//	double dt
//);

//// same as Kalman Filter above, only it takes one extra argument (of type kalman_gain_struc) which is filled with Kalman Gain information
//void extended_kalman_filter
//(
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& A,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_ACTUATORS         >& B,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& H,
//	Matrix<double, EKF_NUM_STATES, 1         >& X,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& P,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& Q,
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>& R,
//	Matrix<double, EKF_NUM_STATES, 1         >& z,
//	Matrix<double, EKF_NUM_ACTUATORS         , 1         >& u,
//	double dt,
//	kalman_struc& kalman
//);

//Matrix4d skew4(Vector3d v);
//MatrixXd linear_dynamics(Vector3d omega, const Matrix<double,3,3>& I);
