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

#include "estimation_lib.h"
//void extended_kalman_filter
//(
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& A,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_ACTUATORS>& B,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& H,
//	Matrix<double, EKF_NUM_STATES,  1		  >& X,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& P,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& Q,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& R,
//	Matrix<double, EKF_NUM_STATES,  1		  >& z,
//	Matrix<double, EKF_NUM_ACTUATORS, 1		  >& u,
//	double dt
//)

//{

//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES> I;
//	I.setIdentity();

//	//State Prediction
//	Matrix<double, EKF_NUM_STATES, 1		 >	X_ = (A*dt + I)*X + B*u;

//	//Covariance Prediction
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	P_ = A*P*A.transpose() + Q;

//	//Measurement Residual
//	Matrix<double, EKF_NUM_STATES, 1		 >	y  = z - H*X_;

//	//Measurement Covariance
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	S  = H*P_*H.transpose() + R;

//	//Kalman Gain
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	K  = P_*H.transpose()*S.inverse();

//	//State Update
//	X = X_ + K*y;

//	//Covariance Update
//	P = (I - K*H)*P_;

//	return;
//}

//void extended_kalman_filter
//(
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& A,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_ACTUATORS>& B,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& H,
//	Matrix<double, EKF_NUM_STATES,  1		  >& X,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& P,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& Q,
//	Matrix<double, EKF_NUM_STATES,  EKF_NUM_STATES >& R,
//	Matrix<double, EKF_NUM_STATES,  1		  >& z,
//	Matrix<double, EKF_NUM_ACTUATORS, 1		  >& u,
//	double dt,
//	kalman_struc& kalman
//)

//{
//	//Identity Matrix
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES> I;
//	I.setIdentity();

//	//fill the kalman_struc with the calculated time
//	if(kalman.time.empty())
//			kalman.time.push_back(0.0);
//	else
//			kalman.time.push_back( *(kalman.time.end()-1) + dt );

//	//State Prediction
//	Matrix<double, EKF_NUM_STATES, 1		 >	X_ = (A*dt + I)*X + B*u;

//	//Covariance Prediction
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	P_ = A*P*A.transpose() + Q;

//	//Measurement Residual
//	Matrix<double, EKF_NUM_STATES, 1		 >	y  = z - H*X_;

//	//Measurement Covariance
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	S  = H*P_*H.transpose() + R;

//	//Kalman Gain
//	Matrix<double, EKF_NUM_STATES, EKF_NUM_STATES>	K  = P_*H.transpose()*S.inverse();

//	//fill the kalman_struc with the calculated gain
//	kalman.gain.push_back( K );
	
//	//State Update
//	X = X_ + K*y;

//	//fill the kalman_struc with the calculated state
//	kalman.state.push_back( X );

//	//Covariance Update
//	P = (I - K*H)*P_;

//	//fill the kalman_struc with the calculated error
//	kalman.error.push_back( P );

//	return;
//}


//Matrix4d skew4(Vector3d v)
//{
//	Matrix4d M;
//	M <<	 0,  v(2), -v(1), v(0),
//		 -v(2),  0   ,  v(0), v(1),
//		  v(1), -v(0),  0   , v(2),
//		 -v(0), -v(1), -v(2), 0 ;
//	return M;

//}


//MatrixXd linear_dynamics(Vector3d omega, const Matrix<double,3,3>& I)
//{
//	//printVector("omega",omega);

//	Matrix4d A_q;
//	Matrix3d A_omega;
//	MatrixXd A;


//	// initialize matrices
//	A = MatrixXd::Zero(EKF_NUM_STATES,EKF_NUM_STATES);
//	A_q = Matrix4d::Zero(4,4);
//	A_omega = Matrix3d::Zero(3,3);

//	//
//	float omega_x = omega(0);
//	float omega_y = omega(1);
//	float omega_z = omega(2);


//	A_q = 0.5*skew4(omega);
////	printMatrix("A_q",A_q);

//	//A.block(0,0,3,3) = A_q;
//	//printMatrix("A",A);


//	A.block<4,4>(0,0) = A_q;
//	// equivalent to
//	//A.block(0,0,4,4) = A_q;

//	// dynamics section

//	double k_1 = -(I(0,0) - I(2,2))/I(1,1);
//	double k_2 = (I(0,0) - I(1,1))/I(2,2);
//	double k_3 = (I(1,1) - I(2,2))/I(0,0);

//	A_omega << 0		  , k_3*omega_z , k_3*omega_y,
//			   k_1*omega_z, 0		   , k_1*omega_x,
//			   k_2*omega_y, k_2*omega_x , 0		  ;

//	A.block<3,3>(4,4) = A_omega;

////	printMatrix("AA",A);

//	return A;
//	//return MatrixXd::Identity(nstates,nstates)*i;
//}


