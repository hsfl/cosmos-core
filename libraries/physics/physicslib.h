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
//! possible of the complete spacecraft. The \ref physicslib_typedefs are used to define the structure of the spacecraft.
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

#include "support/configCosmos.h"
#include "support/timelib.h"
#include "math/mathlib.h"
#include "physics/nrlmsise-00.h"
#include "support/convertlib.h"
#include "support/demlib.h"
#include "physics/physicsdef.h"
#include "support/jsondef.h"

//#include <fcntl.h>
//#include <cmath>
//#include <time.h>
//#include <errno.h>
namespace Cosmos {
    namespace Physics {


        //! \ingroup physicslib
        //! \defgroup physicslib_functions Physics Library functions
        //! @{


        //! \brief JPL planetary ephemeris interpolation (single precision).
        //! \param et Ephemeris time array passed to the JPL DE library.
        //! \param ntarg Pointer to the target body number.
        //! \param ncent Pointer to the center body number.
        //! \param rrd Output position/velocity array (au, au/day).
        void pleph_(double [], long *, long *, double []);

        //! \brief JPL planetary ephemeris interpolation (double precision).
        //! \param et Ephemeris time array passed to the JPL DE library.
        //! \param ntarg Pointer to the target body number.
        //! \param ncent Pointer to the center body number.
        //! \param rrd Output position/velocity array (au, au/day).
        void dpleph_(double [], long *, long *, double []);

        //! \brief Compute geocentric gravitational acceleration vector from spherical harmonics.
        //! \param pos Spherical position of the spacecraft (radians, m).
        //! \param model Gravity model selector (e.g., EGM96).
        //! \param degree Maximum harmonic degree to evaluate.
        //! \return Gravitational acceleration vector in the body frame (m/s²).
        rvector gravity_vector(svector pos,int model,uint32_t degree);

        //! \brief Compute gravitational potential at the given geodetic location.
        //! \param lon Geodetic longitude (radians).
        //! \param lat Geodetic latitude (radians).
        //! \param r Geocentric radius (m).
        //! \param model Gravity model selector.
        //! \param degree Maximum harmonic degree to evaluate.
        //! \return Gravitational potential (m²/s²).
        double gravity_potential(double lon, double lat, double r,int model,uint32_t degree);

        //! Calculates geocentric acceleration vector from chosen model.
        //! \param pos Full position structure containing ECI, geodetic, and geocentric coordinates.
        //! \param model Gravity model selector (e.g., EGM96).
        //! \param degree Maximum harmonic degree to evaluate.
        //! \return Geocentric gravitational acceleration vector (m/s²).
        rvector gravity_accel(Convert::posstruc pos, int model, uint32_t degree);

        //! Calculates geocentric acceleration vector from chosen model.
        //! \param pos Full position structure containing ECI, geodetic, and geocentric coordinates.
        //! \param model Gravity model selector.
        //! \param degree Maximum harmonic degree to evaluate.
        //! \return Geocentric gravitational acceleration vector (m/s²).
        rvector gravity_accel2(Convert::posstruc pos, int model, uint32_t degree);

        //! Calculates geocentric acceleration magnitude from chosen model.
        //! \param radius Geocentric radius (m).
        //! \param colat Geocentric co-latitude measured from north pole (radians).
        //! \param elon East longitude (radians).
        //! \param model Gravity model selector.
        //! \param degree Maximum harmonic degree to evaluate.
        //! \return Gravitational acceleration magnitude (m/s²).
        double gravity(double radius, double colat, double elon, int model, uint32_t degree);

        //! Gravitational model parameters
        //! \param model Gravity model identifier to load parameters for.
        //! \return 0 on success, negative error code on failure.
        int32_t gravity_params(int model);

        //! Legendre polynomial
        //! \param l Degree of the associated Legendre polynomial.
        //! \param m Order of the associated Legendre polynomial.
        //! \param x Argument value (cos of co-latitude, dimensionless).
        //! \return Value of the normalized associated Legendre polynomial P_l^m(x).
        double nplgndr(uint32_t l, uint32_t m, double x);

        //! Radius of Earth at Latitude
        //! \param lat Geodetic latitude (radians).
        //! \return Geocentric radius of the Earth at the given latitude (m).
        double rearth(double lat);

        //! Ground station values
        //! \param satellite Location structure for the spacecraft.
        //! \param groundstation Location structure for the ground station.
        //! \return Spherical vector from ground station to satellite (azimuth rad, elevation rad, range m).
        svector groundstation(Convert::locstruc &satellite, Convert::locstruc &groundstation);

        //! Simulate all devices
        //! \param cinfo Pointer to the COSMOS data structure containing device definitions.
        //! \param loc Current spacecraft location and attitude state.
        void simulate_hardware(cosmosstruc *cinfo, Convert::locstruc &loc);

        //! \brief Simulate all devices over a vector of locations.
        //! \param cinfo Pointer to the COSMOS data structure containing device definitions.
        //! \param locvec Vector of location structures representing a propagation history.
        void simulate_hardware(cosmosstruc *cinfo, vector <Convert::locstruc> &locvec);

        //! Initialize IMU simulation
        //! \param index Index of the IMU device within the device-specific structure.
        //! \param devspec Device-specific structure containing IMU configuration.
        //! \param loc Initial spacecraft location and attitude used to seed the IMU model.
        void initialize_imu(uint16_t index, devspecstruc &devspec, Convert::locstruc &loc);

        //! Simulated IMU values
        //! \param index Index of the IMU device within the COSMOS data structure.
        //! \param root Pointer to the COSMOS data structure.
        //! \param loc Current spacecraft location and attitude state.
        void simulate_imu(int index, cosmosstruc *root, Convert::locstruc &loc);

        //! Acceleration
        //! \brief Compute total linear acceleration acting on the spacecraft.
        //! \param physics Physics structure holding mass, area, drag, and radiation pressure parameters.
        //! \param loc Current spacecraft location and attitude state; acceleration stored back here (m/s²).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_accel(physicsstruc &physics, Convert::locstruc &loc);

        //! Torque
        //! \brief Compute total angular torque acting on the spacecraft.
        //! \param physics Physics structure holding inertia tensor and torque parameters.
        //! \param loc Current spacecraft location and attitude state; angular acceleration stored back here (rad/s²).
        void att_accel(physicsstruc &physics, Convert::locstruc &loc);

        //! Geodetic to Heliocentric
        //! \param pos Position structure whose geodetic fields are used to populate the heliocentric fields.
        void geod2icrf(Convert::posstruc *pos);

        //! \brief Compute atmospheric density using MSIS-86 model.
        //! \param pos Position structure providing the geocentric location of interest.
        //! \param f107avg 81-day average solar flux index F10.7 (solar flux units).
        //! \param f107 Daily solar flux index F10.7 (solar flux units).
        //! \param magidx Geomagnetic activity index Ap.
        //! \return Atmospheric density (kg/m³).
        double msis86_density(Convert::posstruc pos,float f107avg,float f107,float magidx);

        //! \brief Compute atmospheric density using NRLMSISE-00 model.
        //! \param pos Position structure providing the geocentric location of interest.
        //! \param f107avg 81-day average solar flux index F10.7 (solar flux units).
        //! \param f107 Daily solar flux index F10.7 (solar flux units).
        //! \param magidx Geomagnetic activity index Ap.
        //! \return Atmospheric density (kg/m³).
        double msis00_density(Convert::posstruc pos,float f107avg,float f107,float magidx);

        //! \brief Initialize orbit propagation from a TLE stored in the COSMOS data structure.
        //! \param mode Propagation mode flag (e.g., RK4 vs Gauss-Jackson selector).
        //! \param dt Fixed integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param root Pointer to the COSMOS data structure containing the TLE.
        void orbit_init_tle(int32_t mode, double dt, double mjd, cosmosstruc *root);

        //! \brief Initialize orbit propagation from an ECI state vector.
        //! \param mode Propagation mode flag.
        //! \param dt Fixed integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param ipos Initial ECI Cartesian position and velocity (m, m/s).
        //! \param root Pointer to the COSMOS data structure to populate.
        void orbit_init_eci(int32_t mode, double dt, double mjd, Convert::cartpos ipos, cosmosstruc *root);

        //! \brief Initialize orbit propagation from orbital shape parameters.
        //! \param mode Propagation mode flag.
        //! \param dt Fixed integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param altitude Orbit altitude above the reference ellipsoid (m).
        //! \param angle Orbit inclination or argument-of-latitude angle (radians).
        //! \param hour Local solar time at ascending node (hours).
        //! \param root Pointer to the COSMOS data structure to populate.
        void orbit_init_shape(int32_t mode, double dt, double mjd, double altitude, double angle, double hour, cosmosstruc *root);

        //! \brief Advance the spacecraft state to the requested epoch.
        //! \param root Pointer to the COSMOS data structure holding current state and propagator handle.
        //! \param mjd Target epoch in Modified Julian Date.
        void propagate(cosmosstruc *root, double mjd);

        //! \brief Update the COSMOS state from an externally supplied ECI Cartesian position.
        //! \param root Pointer to the COSMOS data structure to update.
        //! \param utc Epoch of the supplied state vector in Modified Julian Date.
        //! \param pos New ECI Cartesian position and velocity (m, m/s).
        //! \return 0 on success, negative error code on failure.
        int update_eci(cosmosstruc *root, double utc, Convert::cartpos pos);

        //! \brief Initialize hardware simulation state from an ECI location.
        //! \param cinfo Pointer to the COSMOS data structure containing hardware definitions.
        //! \param loc Initial spacecraft ECI location and attitude used to seed hardware models.
        void hardware_init_eci(cosmosstruc *cinfo, Convert::locstruc &loc);

        //! \brief Allocate and configure the Gauss-Jackson integrator handle.
        //! \param gjh Gauss-Jackson handle to initialize.
        //! \param order Integrator order (must be even; higher order improves accuracy).
        //! \param utc Starting epoch in Modified Julian Date.
        //! \param dt Integration time step (s); may be adjusted to satisfy stability criteria.
        void gauss_jackson_setup(gj_handle &gjh, uint32_t order, double utc, double &dt);

        //! \brief Initialize the Gauss-Jackson integrator from a TLE.
        //! \param gjh Gauss-Jackson handle to populate.
        //! \param order Integrator order.
        //! \param mode Propagation mode flag.
        //! \param dt Integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param cinfo Pointer to the COSMOS data structure containing the TLE.
        void gauss_jackson_init_tle(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, cosmosstruc *cinfo);

        //! \brief Initialize the Gauss-Jackson integrator from an ECI state vector and attitude.
        //! \param gjh Gauss-Jackson handle to populate.
        //! \param order Integrator order.
        //! \param mode Propagation mode flag.
        //! \param dt Integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param ipos Initial ECI Cartesian position and velocity (m, m/s).
        //! \param iatt Initial ECI quaternion attitude and angular velocity (rad, rad/s).
        //! \param physics Physics structure with mass/drag/radiation pressure properties.
        //! \param loc Location structure updated with the seeded state.
        void gauss_jackson_init_eci(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, Convert::cartpos ipos, Convert::qatt iatt, physicsstruc &physics, Convert::locstruc &loc);

        //! \brief Initialize the Gauss-Jackson integrator from an STK ephemeris segment.
        //! \param gjh Gauss-Jackson handle to populate.
        //! \param order Integrator order.
        //! \param mode Propagation mode flag.
        //! \param dt Integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param stk STK ephemeris structure providing reference trajectory data.
        //! \param physics Physics structure with spacecraft physical properties.
        //! \param loc Location structure updated with the seeded state.
        void gauss_jackson_init_stk(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, Convert::stkstruc &stk, physicsstruc &physics, Convert::locstruc &loc);

        //! \brief Initialize the Gauss-Jackson integrator from orbital shape parameters.
        //! \param gjh Gauss-Jackson handle to populate.
        //! \param order Integrator order.
        //! \param mode Propagation mode flag.
        //! \param dt Integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param altitude Orbit altitude above the reference ellipsoid (m).
        //! \param angle Orbit inclination or argument-of-latitude (radians).
        //! \param hour Local solar time at ascending node (hours).
        //! \param iloc Intermediate location structure used during seeding.
        //! \param physics Physics structure with spacecraft physical properties.
        //! \param loc Location structure updated with the seeded state.
        void gauss_jackson_init(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double mjd, double altitude, double angle, double hour, Convert::locstruc &iloc, physicsstruc &physics, Convert::locstruc &loc);

        //! \brief Advance the Gauss-Jackson integrator by one step in ECI frame.
        //! \param gjh Gauss-Jackson handle containing integrator state.
        //! \param physics Physics structure with current force model parameters.
        //! \param loc Location structure updated with the new ECI state after stepping.
        void gauss_jackson_update_eci(gj_handle &gjh, physicsstruc &physics, Convert::locstruc &loc);

        //! \brief Run the Gauss-Jackson integrator to orbital convergence.
        //! \param gjh Gauss-Jackson handle containing integrator state.
        //! \param physics Physics structure with current force model parameters.
        //! \return Converged location structure representing the settled orbital state.
        Convert::locstruc gauss_jackson_converge_orbit(gj_handle &gjh, physicsstruc &physics);

        //! \brief Converge hardware simulation state within the Gauss-Jackson handle.
        //! \param gjh Gauss-Jackson handle containing integrator and hardware state.
        //! \param physics Physics structure with current force model parameters.
        void gauss_jackson_converge_hardware(gj_handle &gjh, physicsstruc &physics);

        //! \brief Propagate the Gauss-Jackson integrator forward to a target epoch.
        //! \param gjh Gauss-Jackson handle containing integrator state.
        //! \param physics Physics structure with current force model parameters.
        //! \param loc Current spacecraft location updated in place at each substep.
        //! \param mjd Target epoch in Modified Julian Date.
        //! \return Vector of intermediate location structures sampled during propagation.
        vector<Convert::locstruc> gauss_jackson_propagate(gj_handle &gjh, physicsstruc &physics, Convert::locstruc &loc, double mjd);

        //! Load TLE's from file
        //! \param root Pointer to the COSMOS data structure to update with new orbit state.
        //! \param mjd Target epoch in Modified Julian Date.
        //! \return 0 on success, negative error code on failure.
        int orbit_propagate(cosmosstruc *root, double mjd);

        //! \brief Initialize orbit propagation from a TLE file.
        //! \param mode Propagation mode flag.
        //! \param dt Fixed integration time step (s).
        //! \param mjd Initial epoch in Modified Julian Date.
        //! \param ofile Path to the TLE or orbit definition file.
        //! \param root Pointer to the COSMOS data structure to populate.
        //! \return 0 on success, negative error code on failure.
        int orbit_init(int32_t mode, double dt, double mjd, string ofile, cosmosstruc *root);
        //void SolidTide(Convert::posstruc pos, double dc[5][4], double ds[5][4]);

        //! @}
    }
}
#endif
