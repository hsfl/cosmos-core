#include <vector>
#include <iostream>
//#include <Eigen/Dense>

using namespace std;
//using namespace Eigen;

#define EKF_NUM_STATES 7
#define EKF_NUM_ACTUATORS 4
#define EKF_NUM_SENSORS 3

//class kalman_struc
//{
//public:
//	vector< double > time;
//	vector< Matrix < double, EKF_NUM_STATES, EKF_NUM_STATES > > gain;
//	vector< Matrix < double, EKF_NUM_STATES, 1              > > state;
//	vector< Matrix < double, EKF_NUM_STATES, EKF_NUM_STATES > > error;
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
