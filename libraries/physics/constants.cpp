#include "constants.h"

Constants::Constants()
{
}

//// gravitational parameter
// mu = G*(m_earth + m_satellite) = G*(m_earth)
const double Constants::G = 6.673e-20; //+/- 0.001e-20 km^3/(kg.s^2)

//// Mass of the Earth
const double Constants::massEarth = 5.9733320e24; // [kg]

//// Earth mean equatorial radius
const double Constants::radiusEarth = 6378.1363; //km

//// mu = G*m_earth, Earth Gravitational Parameter
const double Constants::mu = 398600.4415; //km^3/s^2
