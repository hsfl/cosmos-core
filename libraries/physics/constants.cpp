#include "constants.h"

Constants::Constants()
{
}

//// gravitational parameter
// mu = G*(m_earth + m_satellite) = G*(m_earth)
const double Constants::G = 6.673e-20; //+/- 0.001e-20 km^3/(kg.s^2)

//// Mass of the Earth
const double Constants::m_earth = 5.9733320e24; // [kg]

//// Earth mean equatorial radius
const double Constants::r_earth = 6378.1363; //km

//// mu = G*m_earth, Earth Gravitational Parameter
const double Constants::mu = 398600.4415; //km^3/s^2

//// satellite altitude above the surface of the Earth
// for circular orbits
const double Constants::h = 590.0; //[km]

//// satellite altitude
const double Constants::r_satellite= r_earth + h;

// angular rate [rad/s]
const double Constants::omega = sqrt(mu/pow(r_satellite,3));

// gravity acceleration
const double Constants::g = mu/pow(r_satellite,2)*1e3; // convert to m/s^2 (not km/s^2)

// motor specifics
const double Constants::Isp = 320; //[s]

const double Constants::c = Isp*g;
