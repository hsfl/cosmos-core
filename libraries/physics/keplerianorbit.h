#ifndef KEPLER_H
#define KEPLER_H
#include <iostream>
#include <iomanip> //setprecision
using namespace std;

#include <Eigen/Eigen>
#include <Eigen/Dense>
using namespace Eigen;

#include "constants.h"


// structure to hold keplerian elements
struct Keplerian
{
public:
    double e;  // eccentricity
    double xi; // specific mechanical energy
    double a;  // semimajor axis
    double p;  // semi parameter

    double i; // inclination
    double i_deg; // inclination in degrees

    double raan; // right ascension of the ascending node
    double raan_deg; // right ascension of the ascending node

    double omega; // argument of perigee
    double omega_deg; // argument of perigee

    double nu; // true anomaly
    double nu_deg; // true anomaly
};

struct CartesianVector
{
public:
    Vector3d r; // position
    Vector3d v; // velocity
};


class Kepler
{
public:
    Kepler();
    double nu2anomaly(double e, double nu);
    double anomaly2nu(double e, double E);
    Keplerian rv2coe(Vector3d v_r, Vector3d v_v);
    void coe2rv(Keplerian k, CartesianVector &cv);
    double KepEqtnE(double M, double e);

    void KeplerCOE(CartesianVector &cv_initial, double dt, CartesianVector &cv_final);
    void KeplerCOE(double dt);


    Keplerian k;

    // pseudo ECI frame related functions
    void computePseudoEci(Vector3d r_ecef, Vector3d v_ecef);
    void convertPseudoEciToGeodetic(double elapsedTime, Vector3d r_eci);
    void convertPseudoEciToGeodetic(double elapsedTime);

    Vector3d r_peci; // position vector
    Vector3d v_peci; // velocity vector

    double geocentric_alt;
    double geocentric_lat;
    double geocentric_lon;

    //
    CartesianVector cv_initial;
    CartesianVector cv_final;


};

#endif // KEPLER_H
