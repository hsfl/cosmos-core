#include <iostream>     // std::cout, std::left
#include <iomanip>      // std::setprecision
using namespace std;

// cosmos includes
#include "math/mathlib.h"
#include "support/stringlib.h"
#include "device/cpu/devicecpu.h"
#include "support/elapsedtime.h"

//#define EIGEN_NO_DEBUG
//#include <Eigen/Dense>
//#include <Eigen/Geometry>

//using namespace Eigen;
//using Eigen::MatrixXd;
//using Eigen::Matrix3f;

using Cosmos::Math::Vector;

void compute_quaternion_from_vectors();
void torque_mtr();

int main()
{
    // use this program to test your code
    cout << "COSMOS Playground" << endl;

    ElapsedTime et;

    while (1) {
        cout << et.lap() << endl;
        COSMOS_SLEEP(1.);
    }

    // compute_quaternion_from_vectors();

    // torque_mtr();

    return 0;

}


void torque_mtr()
{
    // define torque desired
    Vector torqueDesired = {0,0,2};

    // define magnetic Field
    Vector magField = {0,2,1};

    // compute moment for torque desired
    Vector momentDesired = 1./(magField.norm()*magField.norm()) * magField.cross(torqueDesired) ;

    Vector torqueReal = momentDesired.cross(magField);

    cout << "torqueDesired = " << torqueDesired << endl;
    cout << "momentDesired = " << momentDesired << endl;
    cout << "torqueReal    = " << torqueReal << endl;
}

void compute_quaternion_from_vectors(){
    DCM dcm;

    // define basis 1
    basisOrthonormal frame_inertial;
    frame_inertial.i = cv_unitx();
    frame_inertial.j = cv_unity();
    frame_inertial.k = cv_unitz();

    // define basis 2
    // target frame wrt basis1 frame inertial
    basisOrthonormal frame_target_wrt_inertial;

    //    // rotate 90 deg
    //    frame_target_wrt_inertial.i = {0,1,0};
    //    frame_target_wrt_inertial.j = {-1,0,0};
    //    frame_target_wrt_inertial.k = {0,0,1};

    //    // rotate 180 deg
    //    frame_target_wrt_inertial.i = {-1,0,0};
    //    frame_target_wrt_inertial.j = {0,-1,0};
    //    frame_target_wrt_inertial.k = {0,0,1};

    //    // rotate 270 deg, q = [0,0,-0.707107, 0.707107]
    //    frame_target_wrt_inertial.i = {0,-1,0};
    //    frame_target_wrt_inertial.j = {1,0,0};
    //    frame_target_wrt_inertial.k = {0,0,1};

    // rotate x down a bit [1,0,0]->[1,0,-1] [0 -0.382683 0 0.92388]
    frame_target_wrt_inertial.i = {1,0,1};
    frame_target_wrt_inertial.j = {0,1,0};
    frame_target_wrt_inertial.k = {-1,0,1};

    // rotate x up a bit [1,0,0]->[1,0,-1] [0 -0.382683 0 0.92388]
    frame_target_wrt_inertial.i = {1,0,-1};
    frame_target_wrt_inertial.j = {0,1,0};
    frame_target_wrt_inertial.k = {1,0,1};

    // rotate y up a bit [1,0,0]->[1,0,-1] [0 -0.382683 0 0.92388]
    frame_target_wrt_inertial.i = {1,0,0};
    frame_target_wrt_inertial.j = {0,1,1};
    frame_target_wrt_inertial.k = {0,-1,1};

    // this rotation matrix represents an active rotation (fixed axis, vector rotates)
    // the vector is represented in the inertial frame
    // the vector will rotate counterclockwise around the axis
    cmatrix R_target_from_inertial = dcm.base1_from_base2(frame_inertial, frame_target_wrt_inertial);

    //    cout << R_target_from_inertial << endl;

    // test vector rotation
    // a x-vector of the inertial frame (1,0,0) will be rotated to the x-vector of the target frame (1,1,0)
    cvector test_vector = cv_mmult(R_target_from_inertial, {1,0,0});
    cout << "test_vector " << test_vector << endl;

    // quaternion that represents the rotation between the body frame and the inertial frame
    quaternion q_target_wrt_inertial = q_dcm2quaternion_cm(R_target_from_inertial);
    cout << "q_target_wrt_inertial: ";
    cout << q_target_wrt_inertial.d.x << " ";
    cout << q_target_wrt_inertial.d.y << " ";
    cout << q_target_wrt_inertial.d.z << " ";
    cout << q_target_wrt_inertial.w << " " << endl;


    // load the Quaternion namespace
    using Cosmos::Math::Quaternion;

    // The above math can be simplified by just calling this function
    Quaternion q;
    //    // rotate 90 deg (i -> j) = [0 0 0.707107 0.707107]
    //    q.fromTwoVectors({1,0,0},{0,1,0});

    //    // rotate 180 deg (i -> -i) = [0 0 1 0]
    //    q.fromTwoVectors({1,0,0},{-1,0,0});

    //    // rotate 270 deg (i -> -j) = [0,0,-0.707107, 0.707107]
    //    q.fromTwoVectors({1,0,0},{0,-1,0});

    // rotate x down a bit [1,0,0]->[1,0,-1] [0 -0.382683 0 0.92388]
    q.fromTwoVectors({1,0,0},{1,0,1});

    // rotate x up a bit [1,0,0]->[1,0,-1] [0 0.382683 0 0.92388]
    q.fromTwoVectors({1,0,0},{1,0,-1});

    // rotate y up a bit [0,1,0]->[0,1,1] [0 0.382683 0 0.92388]
    q.fromTwoVectors({0,1,0},{0,1,1});


    cout << "q: " << q << endl;

    Quaternion q1(0.017,-0.022,-0.004, 1.000);
    cout << q1.toEuler()*(180./PI) << endl;


    //    Matrix3f m;
    //    m = AngleAxisf(45*PI/180., Vector3f::UnitZ());

    //    cout << m << endl;

    //    Quaterniond q;
    //    Vector3d a,b;
    //    a << 1,0,0;
    //    b << 1,1,0;
    //    //Quaterniond q1 = q.setFromTwoVectors(a,b);

    //    Quaterniond q1 = q.FromTwoVectors(a,b);
    //    cout << q1.x() << "," << q1.y() << "," << q1.z() << "," << q1.w() << endl;

    //    q.setFromTwoVectors(a,b);
    //    cout << q.x() << "," << q.y() << "," << q.z() << "," << q.w() << endl;

    //
    //q_rotation_45deg_around_z;

    // I want: the quaternion that represents the target frame wrt the inertial frame
    // this is equivalent of a quaternion that represents the rotation of 45 deg around z axis of the inertial frame

    //    using COSMOS::Math::Vector;
    //    Vector a, b;
    //    a = {123,2,3};

    //    cout << a.x << endl;
    //    cout << a.y << endl;
    //    cout << a.z << endl;

    //    for (int i= 0; i<3; i++){
    //        cout << a.at(i) << endl;
    //    }

}


void test () {

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

    //    quaternion q_error, q_error_last, q_derror;
    //    double dt = 1.58805e-005;

    //    q_error = {0.0129237,-0.0372017,-0.648016,0.760608};
    //    q_error_last = {0.00867779,-0.030221,-0.654894,0.755066};
    //    q_derror = q_smult( 1.0/dt , q_sub(q_error, q_error_last));

    //    //cout << q_derror << endl;

    //    //
    //    DCM dcm;

    //    // define body frame base
    //    basisOrthonormal frame_body;
    //    frame_body.i = cv_unitx();
    //    frame_body.j = cv_unity();
    //    frame_body.k = cv_unitz();

    //    // define IMU frame
    //    // IMU+X is aligned with BODY+Y
    //    // IMU+Y is aligned with BODY+X
    //    // IMU+Z is aligned with BODY-Z
    //    basisOrthonormal frame_imu;
    //    frame_imu.i = {0,1,0};
    //    frame_imu.j = {1,0,0};
    //    frame_imu.k = {0,0,-1};

    //    // get the DCM to convert IMU vector into body vector
    //    cmatrix R_body_from_imu = dcm.base1_from_base2(frame_body,frame_imu);

    //    quaternion test_q = q_dcm2quaternion_cm(R_body_from_imu);

    //    //cout << R_body_from_imu << endl;
    //    cout << test_q << endl;

    //    //rvector sourcea = {frame_imu.i.x, frame_imu.i.y, frame_imu.i};
    //    uvector sourcea, sourceb, sourcec;
    //    sourcea.c = frame_imu.i;
    //    sourceb.c = frame_imu.j;
    //    sourcec.c = frame_imu.k;

    //    uvector targeta, targetb;
    //    targeta.c = frame_body.i;
    //    targetb.c = frame_body.j;

    //    quaternion tq = q_transform_for(sourcea.r, sourceb.r, targeta.r, targetb.r);
    //    cmatrix R = cm_quaternion2dcm(tq);

    //    cout << tq << endl;

    //    // test

    //    uvector test_imu_x, test_imu_y, test_imu_z;
    //    test_imu_x.c = cv_mmult(R_body_from_imu, frame_imu.i);
    //    test_imu_y.c = cv_mmult(R_body_from_imu, frame_imu.j);
    //    test_imu_z.c = cv_mmult(R_body_from_imu, frame_imu.k);
    //    cout << test_imu_x.c << endl;
    //    cout << test_imu_y.c << endl;
    //    cout << test_imu_z.c << endl;

    //    test_imu_x.r = transform_q(tq, sourcea.r);
    //    test_imu_y.r = transform_q(tq, sourceb.r);
    //    test_imu_z.r = transform_q(tq, sourcec.r);
    //    cout << test_imu_x.r << endl;
    //    cout << test_imu_y.r << endl;
    //    cout << test_imu_z.r << endl;

    //    // ------------------------------------------
    //    // testing * operator in rvector
    //    rvector v, b;
    //    v = {1,2,3};

    //    b = v * 3;

    //    cout << "b=" <<  b << endl;

    //-119.568,-195.145,-138.744,-1397.701
    //70.4439,-80.4836,222.288,183.266
    //267.364,-439.573,433.141,348.974

    // ------------------------------------------
    // define ST frame wrt BODY frame
    // TODO: must check with Lance
    // ST+X (direction of connector) is aligned with BODY-X (opposite of + direction)
    // ST+Y is triad between X and Z
    // ST+Z (direction of outward boresight) is aligned with BODY-Z (opposite of + direction)

    //    DCM dcm;

    //    // define body frame base
    //    basisOrthonormal frame_body_wrt_body;
    //    frame_body_wrt_body.i = cv_unitx();
    //    frame_body_wrt_body.j = cv_unity();
    //    frame_body_wrt_body.k = cv_unitz();

    //    // define ST frame base wrt to the body frame
    //    basisOrthonormal frame_st_wrt_body;
    //    frame_st_wrt_body.i = {-1,0,0};
    //    frame_st_wrt_body.j = {0,1,0};
    //    frame_st_wrt_body.k = {0,0,-1};

    //    // get the DCM to represent a vector in the ST frame on the body frame
    //    // This is a passive rotation
    //    // Ex: The vector (1,0,0) in ST frame is (-1,0,0) in body frame
    //    cmatrix R_body_from_st_frame = dcm.base1_from_base2(frame_body_wrt_body, frame_st_wrt_body);

    //    //cout << R_body_from_st_frame << endl;
    //    // test vector rotation
    //    cvector test_vector = cv_mmult(R_body_from_st_frame, {1,0,0});
    //    //cout << "test_vector1" << test_vector << endl;

    //    // quaternion that represents the rotation between the st frame and the body frame
    //    quaternion q_rotation_body_from_st_frame = q_dcm2quaternion_cm(R_body_from_st_frame);
    //    cout << "q_rotation_body_from_st_frame:" << q_rotation_body_from_st_frame << endl;

    //    quaternion q_transform_body_from_st_frame = q_transform_for(rv_unitx(), rv_unitz(), {-1,0,0}, {0,0,-1});
    //    cout << "q_transform_body_from_st_frame:" << q_transform_body_from_st_frame << endl;

    //    // quaternion given by star tracker (in st frame)

    //    // co-aligned with inertial frame
    //    quaternion q_st = {{0,0,0},1};

    //    // st frame rotated +45 deg around z
    //    q_st = q_change_around_z( DEG2RAD(45) ); // this is actually a rotation of the object!!!
    //    q_st = q_change_around_cv({1,2,3},DEG2RAD(45));

    //    cout << "q_st:::::" << q_st << endl;

    //    cmatrix R_inertial_from_st = cm_quaternion2dcm(q_st);
    //    //test_vector = cv_mmult(R_inertial_from_st, {1,0,0});
    //    //cout << "test_vector:" << test_vector << endl;

    //    cout << "q_st:" << q_st << endl;

    //    quaternion q_inertial_from_body = q_mult(q_st, q_conjugate(q_rotation_body_from_st_frame));

    //    cout << "q_inertial_from_body (1):" << q_inertial_from_body << endl;

    //    //cout << q_body_from_st_frame << endl;
    //    //cout << q_body_from_st_data << endl;

    //    // now test the conversion of a vector in the ST frame to body frame
    //    // testing
    //    cmatrix R_inertial_from_body = cm_quaternion2dcm(q_inertial_from_body);
    //    // testing with conjugate because cm_quaternion2dcm is not proper???
    //    R_inertial_from_body = cm_quaternion2dcm(q_conjugate(q_inertial_from_body));
    //    //cout << R_inertial_from_body << endl;
    //    test_vector = cv_mmult(R_inertial_from_body, {1,0,0});
    //    cout << "test_vector:" << test_vector << endl;

    //    // confirm
    //    R_inertial_from_body = cm_mmult(R_inertial_from_st, cm_transpose(R_body_from_st_frame));

    //    //cout << R_inertial_from_body << endl;

    //    test_vector = cv_mmult(R_inertial_from_body, {1,0,0});

    //    //cout << test_vector << endl;

    //    // convert the omega given in the st frame to the body frame
    //    rvector omega_st_wrt_eci_in_st_frame = {-1.4,-1.34,-4};
    //    rvector omega_st_wrt_eci_in_body_frame = rv_mmult(rm_from_cm(R_body_from_st_frame), omega_st_wrt_eci_in_st_frame);

    //    //cout << omega_st_wrt_eci_in_body_frame << endl;

    //    // calculated from
    //    // calc_transform 1 0 0 0 0 1 -1 0 0 0 0 -1
    //    q_rotation_body_from_st_frame = {{0,1,0},0};
    //    omega_st_wrt_eci_in_body_frame = transform_q(q_rotation_body_from_st_frame, omega_st_wrt_eci_in_st_frame);

    //    //cout << omega_st_wrt_eci_in_body_frame << endl;

    //    /// testing quaterion conversion
    //    q_inertial_from_body = q_conjugate(q_mult(q_rotation_body_from_st_frame,q_conjugate(q_st)));

    //    cout << "q_inertial_from_body (2):" << q_inertial_from_body << endl;

    //    // testing the vectors now
    //    R_inertial_from_body = cm_quaternion2dcm(q_conjugate(q_inertial_from_body));
    //    test_vector = cv_mmult(R_inertial_from_body, {1,0,0});
    //    cout << "test_vector:" << test_vector << endl;
}
