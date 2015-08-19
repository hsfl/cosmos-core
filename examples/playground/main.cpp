#include <iostream>     // std::cout, std::left
#include <iomanip>      // std::setprecision
using namespace std;

// cosmos includes
#include "math/mathlib.h"

int main()
{
    cout << "COSMOS Playground" << endl;

//    cout << 100 << '\n';
//    cout.width(10);
//    cout << 100 << '\n';
//    //cout.fill('x');
//    cout.width(15);
//    cout << left << 100 << '\n';

//    cout << "[PD] ";


//    quaternion q;
//    rvector v;
//    v = rv_one();

//    q = q_eye();
//    cout << "q:" << q << endl;
//    q = {{-1.,0,0},1};
//    cout << "q:" << q << endl;


//    v = {1.23,1,3};
//    cout << "v:" << v << endl;
//    v = {-1.23,-10,3};
//    cout << "v:" << v << endl;

    quaternion q_error, q_error_last, q_derror;
    double dt = 1.58805e-005;

    q_error = {0.0129237,-0.0372017,-0.648016,0.760608};
    q_error_last = {0.00867779,-0.030221,-0.654894,0.755066};
    q_derror = q_smult( 1.0/dt , q_sub(q_error, q_error_last));

    //cout << q_derror << endl;

    //
    DCM dcm;

    // define body frame base
    basisOrthonormal frame_body;
    frame_body.i = cv_unitx();
    frame_body.j = cv_unity();
    frame_body.k = cv_unitz();

    // define IMU frame
    // IMU+X is aligned with BODY+Y
    // IMU+Y is aligned with BODY+X
    // IMU+Z is aligned with BODY-Z
    basisOrthonormal frame_imu;
    frame_imu.i = {0,1,0};
    frame_imu.j = {1,0,0};
    frame_imu.k = {0,0,-1};

    // get the DCM to convert IMU vector into body vector
    cmatrix R_body_from_imu = dcm.base1_from_base2(frame_body,frame_imu);

    quaternion test_q = q_dcm2quaternion_cm(R_body_from_imu);

    //cout << R_body_from_imu << endl;
    cout << test_q << endl;

    //rvector sourcea = {frame_imu.i.x, frame_imu.i.y, frame_imu.i};
    uvector sourcea, sourceb, sourcec;
    sourcea.c = frame_imu.i;
    sourceb.c = frame_imu.j;
    sourcec.c = frame_imu.k;

    uvector targeta, targetb;
    targeta.c = frame_body.i;
    targetb.c = frame_body.j;

    quaternion tq = q_transform_for(sourcea.r, sourceb.r, targeta.r, targetb.r);
    cmatrix R = cm_quaternion2dcm(tq);

    cout << tq << endl;

    // test

    uvector test_imu_x, test_imu_y, test_imu_z;
    test_imu_x.c = cv_mmult(R_body_from_imu, frame_imu.i);
    test_imu_y.c = cv_mmult(R_body_from_imu, frame_imu.j);
    test_imu_z.c = cv_mmult(R_body_from_imu, frame_imu.k);
    cout << test_imu_x.c << endl;
    cout << test_imu_y.c << endl;
    cout << test_imu_z.c << endl;

    test_imu_x.r = transform_q(tq, sourcea.r);
    test_imu_y.r = transform_q(tq, sourceb.r);
    test_imu_z.r = transform_q(tq, sourcec.r);
    cout << test_imu_x.r << endl;
    cout << test_imu_y.r << endl;
    cout << test_imu_z.r << endl;





//-119.568,-195.145,-138.744,-1397.701
    //70.4439,-80.4836,222.288,183.266
    //267.364,-439.573,433.141,348.974

    return 0;

}

