#ifndef _ORBITLIB_H
#define _ORBITLIB_H 1

/*! \file physicslib.h
* \brief Orbit library include file
*/

//! \ingroup support
//! \defgroup physicslib Physics Simulation Library
//! A library containing functions for simulating the orbital
//! environment of a spacecraft.
//!
//! The \ref physicslib_functions in this library allow you to work with the various
//! aspects of a spacecrafts environment including:
//! - Position: The physical location of the spacecraft wrt various
//! frames of reference.
//! - Attitude: The orientation of the spacecraft in various frames of
//! reference.
//! - Power: Power produced and used by the spacecraft.
//! - Temperature: Thermal inputs, outputs and flows, and their effect
//! on overall temperature.
//! - Forces: Linear and angular forces exerted both externally and
//! internally.
//! - Hardware State: Aspects of the internal state of various
//! subsystems.
//!
//! The goal of this library is to provide for as full a simulation as
//! possible of the complete spacecraft. The \ref physicslib_typdefs and \ref
//! orbit_contants are used to define the structure of the spacecraft.
//! Both Static and Dynamic definitions exist for element. The elements
//! are described using the following categories:
//! - Structures: Physical elements of the spacecraft. These are
//! located in the Body frame of the spacecraft and can be:
//! 	- Panel: 4 corners and a thickness
//! 	- Box: 6 sides and a wall thickness
//!	- Cylinder: 2 ends and a radius
//! 	- Sphere: center and a radius
//! - Components: Logical elements of the spacecraft. They represent
//! additional physical elements of the spacecraft that are not purely
//! structural. They have physical qualities (mass, power, temperature)
//! tied to them, and are in turn tied to a Structure.
//! - Devices: Specialized Component extensions. Each different type has
//! specialized information tied to it, as well as being tied to a
//! Component. Currently supported Devices are:
//! 	- Reaction Wheel
//! 	- IMU
//! 	- GPS
//! 	- Processor (CPU)
//! 	- Magnetic Torque Rod (MTR)
//! 	- Sun Sensor (SS)
//! 	- Solar Panel String
//! -
//! Many of the elements used in the ::cosmosstruc are defined here.

#include "configCosmos.h"

#include "cosmos-defs.h"
#include "mathlib.h"
#include "nrlmsise-00.h"
#include "convertlib.h"
#include "demlib.h"
#include "physicsdef.h"
#include "jsondef.h"

#include <fcntl.h>
#include <cmath>
#include <time.h>
#include <errno.h>

//! \ingroup physicslib
//! \defgroup physicslib_functions Physics Library functions
//! @{


void pleph_(double [], long *, long *, double []);
void dpleph_(double [], long *, long *, double []);

rvector gravity_vector(svector pos,int model,uint32_t degree);
double gravity_potential(double lon, double lat, double r,int model,uint32_t degree);
//! Calculates geocentric acceleration vector from chosen model. 
rvector gravity_accel(posstruc pos, int model, uint32_t degree);
//! Calculates geocentric acceleration vector from chosen model. 
rvector gravity_accel2(posstruc pos, int model, uint32_t degree);
//! Calculates geocentric acceleration magnitude from chosen model.
double gravity(double radius, double colat, double elon, int model, uint32_t degree);
//! Gravitational model parameters
int32_t gravity_params(int model);
//! Legendre polynomial
double nplgndr(uint32_t l, uint32_t m, double x);
//! Power values
void power(cosmosstruc *root);
//! Ground station values
svector groundstation(locstruc *satellite,locstruc *groundstation);
//! Simulate all devices
void simulate_hardware(cosmosstruc &cdata, locstruc &loc);
//! Initialize IMU simulation
void initialize_imu(uint16_t index, devspecstruc &devspec, locstruc &loc);
//! Simulated IMU values
void simulate_imu(int index, cosmosstruc &root, locstruc &loc);
//! Acceleration
void pos_accel(physicsstruc &physics, locstruc &loc);
//! Torque
void att_accel(physicsstruc &physics, locstruc &loc);
//! Geodetic to Heliocentric
void geod2baryc(posstruc *pos);
double msis86_density(posstruc pos,float f107avg,float f107,float magidx);
double msis00_density(posstruc pos,float f107avg,float f107,float magidx);
void orbit_init_tle(int32_t mode,double dt,double mjd,cosmosstruc &root);
void orbit_init_eci(int32_t mode, double dt, double mjd, cartpos ipos, cosmosstruc &root);
void orbit_init_shape(int32_t mode, double dt, double mjd, double altitude, double angle, double hour, cosmosstruc &root);
void propagate(cosmosstruc &root, double mjd);
double rearth(double lat);
int update_eci(cosmosstruc &root, double utc, cartpos pos);

void hardware_init_eci(devspecstruc &devspec, locstruc &loc);
void gauss_jackson_setup(gj_handle &gjh, uint32_t order, double utc, double &dt);
void gauss_jackson_init_tle(gj_handle &gjh, uint32_t order,int32_t mode,double dt,double mjd,cosmosstruc &cdata);
void gauss_jackson_init_eci(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, cartpos ipos, qatt iatt, cosmosstruc &cdata);
void gauss_jackson_init_stk(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, stkstruc *stk, cosmosstruc &cdata, locstruc &loc);
void gauss_jackson_init(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, double altitude, double angle, double hour, cosmosstruc &cdata, locstruc &loc);
locstruc gauss_jackson_converge_orbit(gj_handle &gjh, cosmosstruc &cdata);
void gauss_jackson_converge_hardware(gj_handle &gjh, cosmosstruc &cdata);
void gauss_jackson_propagate(gj_handle &gjh, cosmosstruc &root, double mjd);
//! Load TLE's from file
int orbit_propagate(cosmosstruc &root, double mjd);
int orbit_init(int32_t mode,double dt,double mjd,char *ofile,cosmosstruc &root);
//void SolidTide(posstruc pos, double dc[5][4], double ds[5][4]);

//! @}

#endif
