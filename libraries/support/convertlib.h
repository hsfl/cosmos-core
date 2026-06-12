/*! \file convertlib.h
    \brief convertlib include file
    A library providing functions for conversion between the various
    position and attitude coordinate systems used in orbital mechanics.
*/

//! \ingroup support
//! \defgroup convertlib Coordinate conversion library
//! Coordinate conversion.
//!
//! COSMOS supports a wide variety of coordinate systems for both position and attitude. This
//! library provides both definitions of structures for representing these various systems, as
//! well as functions for conversion between them. A number of constants are also defined that
//! relate to the various physical parameters involved in these conversions.
//!
//! COSMOS approaches the process of defining position and attitude by defining something called
//! a Location. A fully defined Location, stored in a ::locstruc, represents the complete state
//! vector for an object, including time, represented in all possible coordinate systems. This
//! Location is first broken down in to a ::posstruc and ::attstruc, representing Position and
//! Attitude. These are subsequently broken down in to the various possible representations in
//! the various systems supported.
//!
//! Position is actually a triple of the 0th, 1st and 2nd derivative of position, measured
//! in either meters, or meters and radians, with respect
//! to a standardly defined coordinate system. The systems currently supported are:
//! - Barycentric: Cartesian, ICRF centered on Solar System Barycenter
//! - Earth Centered Inertial: Cartesian, ICRF centered on instantaneous Earth position
//! - Selene Centered Inertial: Cartesian, ICRF centered on instantaneous Moon position
//! - Geocentric: Cartesian, ITRF centered on instantaneous Earth position
//! - Selenocentric: Cartesian, lunar pole and equator of epoch and the IAU node of epoch
//! centered on instantaneous Moon position
//! - Geodetic: Oblate Spherical, ITRF centered on instantaneous Earth position
//! - Selenographic: Spherical, lunar pole and equator of epoch and the IAU node of epoch
//! - Geospherical: Spherical, ITRF centered on instantaneous Earth position
//!
//! The ::posstruc also contains some extra derivative information such as:
//! - angular separation between Earth and Sun
//! - angular separation between Moon and Sun
//! - angular size of Sun
//! - Solar radiance
//! - Sun to Earth vector
//! - Sun to Moon vector
//!
//! Attitude is also a triple of the 0th, 1st and 2nd derivatives. The 1st and 2nd derivatives,
//! or Omega and Alpha, are represented as vectors in the relevant coordinate system. The 0th
//! derivative is represented as the quaternion that will transform a vector in the relevant
//! coordinate system into a vector in the body frame. The systems currently supported are:
//! - Topocentric: Geocentric based, East North Up
//! - Local Vertical Local Horizontal: Geocentric based, +z = -position, +y = +z cross velocity, +x = +y cross +z
//! - Geocentric: Geocentric based
//! - Selenocentric:: Selenocentric based
//! - ICRF: ICRF based
//!
//! Additionally, support is provided for Two Line Element and table based coordinates.

#ifndef _CONVERTLIB_H
#define _CONVERTLIB_H 1

#include "support/configCosmos.h"

#include "math/mathlib.h"
using namespace Cosmos::Math::Matrices;
using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;
#include "support/convertdef.h"
#include "support/timelib.h"
#include "support/geomag.h"
#include "support/ephemlib.h"
namespace Cosmos {
    namespace Convert {


        //! \ingroup convertlib
        //! \defgroup convertlib_functions Coordinate conversion functions
        //! @{

        // TODO: implement using pos_geoc2geod
        //! \brief Zero-initialize a locstruc so that propagation can detect updates.
        //! \param loc Pointer to the location structure to clear (output).
        //! \return 0 on success, negative error code on failure.
        int32_t loc_clear(locstruc *loc);
        //! \brief Compute time-dependent auxiliary position data (rotation matrices, ephemerides).
        //! \param utc Current epoch in Modified Julian Date (UTC).
        //! \param loc Pointer to the location structure whose extra fields are populated (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_extra(double utc, locstruc *loc);
        //! \brief Compute the LVLH frame rotation matrices and store them in loc.pos.extra.
        //! \param loc Pointer to the location structure to update (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh(locstruc *loc);
        //! \brief Propagate all other position frames from the ICRF (barycentric) slot.
        //! \param loc Pointer to the location structure whose ICRF position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf(locstruc *loc);
        //! \brief Propagate all other position frames from the ECI slot.
        //! \param loc Pointer to the location structure whose ECI position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci(locstruc *loc);
        //! \brief Propagate all other position frames from the SCI (Selene-centered inertial) slot.
        //! \param loc Pointer to the location structure whose SCI position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci(locstruc *loc);
        //! \brief Propagate all other position frames from the GEOC (geocentric ITRF) slot.
        //! \param loc Pointer to the location structure whose GEOC position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc(locstruc *loc);
        //! \brief Propagate all other position frames from the GEOS (geographic spherical) slot.
        //! \param loc Pointer to the location structure whose GEOS position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geos(locstruc *loc);
        //! \brief Propagate all other position frames from the GEOD (geodetic) slot.
        //! \param loc Pointer to the location structure whose GEOD position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geod(locstruc *loc);
        //! \brief Propagate all other position frames from the SELC (selenocentric) slot.
        //! \param loc Pointer to the location structure whose SELC position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc(locstruc *loc);
        //! \brief Propagate all other position frames from the SELG (selenographic) slot.
        //! \param loc Pointer to the location structure whose SELG position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selg(locstruc *loc);
        //! \brief Convert position from ICRF (barycentric) to ECI frame within a locstruc.
        //! \param loc Pointer to the location structure whose ICRF slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf2eci(locstruc *loc);
        //! \brief Convert position from ECI to ICRF (barycentric) frame within a locstruc.
        //! \param loc Pointer to the location structure whose ECI slot is the source; ICRF slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2icrf(locstruc *loc);
        //! \brief Convert position from ICRF (barycentric) to SCI (Selene-centered inertial) frame within a locstruc.
        //! \param loc Pointer to the location structure whose ICRF slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf2sci(locstruc *loc);
        //! \brief Convert position from SCI to ICRF (barycentric) frame within a locstruc.
        //! \param loc Pointer to the location structure whose SCI slot is the source; ICRF slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2icrf(locstruc *loc);
        //! \brief Convert position from ECI to GEOC (geocentric ITRF) frame within a locstruc.
        //! \param loc Pointer to the location structure whose ECI slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2geoc(locstruc *loc);
        //! \brief Convert position from ECI to SELC (selenocentric) frame within a locstruc.
        //! \param loc Pointer to the location structure whose ECI slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2selc(locstruc *loc);
        //! \brief Convert position from GEOC (geocentric ITRF) to ECI frame within a locstruc.
        //! \param loc Pointer to the location structure whose GEOC slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2eci(locstruc *loc);
        //! \brief Convert position from GEOC (geocentric ITRF) to GEOD (geodetic) frame within a locstruc.
        //! \param loc Pointer to the location structure whose GEOC slot is the source; GEOD slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2geod(locstruc *loc);
        //! \brief Convert position from GEOD (geodetic) to GEOC (geocentric ITRF) frame within a locstruc.
        //! \param loc Pointer to the location structure whose GEOD slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geod2geoc(locstruc *loc);
        //! \brief Convert position from GEOC (geocentric ITRF) to GEOS (geographic spherical) frame within a locstruc.
        //! \param loc Pointer to the location structure whose GEOC slot is the source; GEOS slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2geos(locstruc *loc);
        //! \brief Convert position from GEOS (geographic spherical) to GEOC (geocentric ITRF) frame within a locstruc.
        //! \param loc Pointer to the location structure whose GEOS slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geos2geoc(locstruc *loc);
        //! \brief Convert position from ECI to SCI (Selene-centered inertial) frame within a locstruc.
        //! \param loc Pointer to the location structure whose ECI slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2sci(locstruc *loc);
        //! \brief Convert position from SCI to ECI frame within a locstruc.
        //! \param loc Pointer to the location structure whose SCI slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2eci(locstruc *loc);
        //! \brief Convert position from SCI (Selene-centered inertial) to SELC (selenocentric) frame within a locstruc.
        //! \param loc Pointer to the location structure whose SCI slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2selc(locstruc *loc);
        //! \brief Convert position from SELC (selenocentric) to SELG (selenographic) frame within a locstruc.
        //! \param loc Pointer to the location structure whose SELC slot is the source; SELG slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2selg(locstruc *loc);
        //! \brief Convert position from SELC (selenocentric) to SCI frame within a locstruc.
        //! \param loc Pointer to the location structure whose SELC slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2sci(locstruc *loc);
        //! \brief Convert position from SELG (selenographic) to SELC (selenocentric) frame within a locstruc.
        //! \param loc Pointer to the location structure whose SELG slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selg2selc(locstruc *loc);
        //! \brief Convert position from SELC (selenocentric) to ECI frame within a locstruc.
        //! \param loc Pointer to the location structure whose SELC slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2eci(locstruc *loc);
        //! \brief Remove the stored LVLH offset from loc, converting the location back to its inertial origin.
        //! \param loc Pointer to the location structure with a valid LVLH offset applied (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh2origin(locstruc *loc);
        //! \brief Apply the LVLH offset stored in loc to shift the location away from its inertial origin.
        //! \param loc Pointer to the location structure whose pos.lvlh field holds the offset to apply (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_origin2lvlh(locstruc *loc);
        //! \brief Compute the LVLH offset of geoc relative to the base orbit.
        //! \param geoc Pointer to the location structure for the offset spacecraft (input).
        //! \param base Pointer to the reference orbit location structure; LVLH offset is written to base->pos.lvlh (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2lvlh(locstruc *geoc, locstruc *base);
        //! \brief Reconstruct the inertial GEOC position of a spacecraft from a base orbit and an LVLH offset.
        //! \param base Pointer to the reference orbit location structure (input).
        //! \param geoc Pointer to the location structure to be filled with the recovered inertial position (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh2geoc(locstruc *base, locstruc *geoc);

        //! \brief Zero-initialize a locstruc so that propagation can detect updates (reference overload).
        //! \param loc Reference to the location structure to clear (output).
        //! \return 0 on success, negative error code on failure.
        int32_t loc_clear(locstruc &loc);
        //! \brief Compute time-dependent auxiliary position data (reference overload).
        //! \param utc Current epoch in Modified Julian Date (UTC).
        //! \param loc Reference to the location structure whose extra fields are populated (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_extra(double utc, locstruc &loc);
        //! \brief Compute the LVLH frame rotation matrices (reference overload).
        //! \param loc Reference to the location structure to update (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh(locstruc &loc);
        //! \brief Zero-initialize only the position portion of a locstruc.
        //! \param loc Reference to the location structure whose posstruc is cleared (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_clear(locstruc &loc);
        //! \brief Propagate all other position frames from the ICRF (barycentric) slot (reference overload).
        //! \param loc Reference to the location structure whose ICRF position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf(locstruc &loc);
        //! \brief Propagate all other position frames from the ECI slot (reference overload).
        //! \param loc Reference to the location structure whose ECI position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci(locstruc &loc);
        //! \brief Propagate all other position frames from the SCI slot (reference overload).
        //! \param loc Reference to the location structure whose SCI position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci(locstruc &loc);
        //! \brief Propagate all other position frames from the GEOC slot (reference overload).
        //! \param loc Reference to the location structure whose GEOC position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc(locstruc &loc);
        //! \brief Propagate all other position frames from the GEOS slot (reference overload).
        //! \param loc Reference to the location structure whose GEOS position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geos(locstruc &loc);
        //! \brief Propagate all other position frames from the GEOD slot (reference overload).
        //! \param loc Reference to the location structure whose GEOD position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geod(locstruc &loc);
        //! \brief Propagate all other position frames from the SELC slot (reference overload).
        //! \param loc Reference to the location structure whose SELC position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc(locstruc &loc);
        //! \brief Propagate all other position frames from the SELG slot (reference overload).
        //! \param loc Reference to the location structure whose SELG position is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selg(locstruc &loc);
        //! \brief Convert position from ICRF to ECI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ICRF slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf2eci(locstruc &loc);
        //! \brief Convert position from ECI to ICRF frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ECI slot is the source; ICRF slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2icrf(locstruc &loc);
        //! \brief Convert position from ICRF to SCI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ICRF slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_icrf2sci(locstruc &loc);
        //! \brief Convert position from SCI to ICRF frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SCI slot is the source; ICRF slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2icrf(locstruc &loc);
        //! \brief Convert position from ECI to GEOC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ECI slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2geoc(locstruc &loc);
        //! \brief Convert position from ECI to SELC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ECI slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2selc(locstruc &loc);
        //! \brief Convert position from GEOC to ECI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose GEOC slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2eci(locstruc &loc);
        //! \brief Convert position from GEOC to GEOD frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose GEOC slot is the source; GEOD slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2geod(locstruc &loc);
        //! \brief Convert position from GEOD to GEOC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose GEOD slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geod2geoc(locstruc &loc);
        //! \brief Convert position from GEOC to GEOS frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose GEOC slot is the source; GEOS slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2geos(locstruc &loc);
        //! \brief Convert position from GEOS to GEOC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose GEOS slot is the source; GEOC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geos2geoc(locstruc &loc);
        //! \brief Convert position from ECI to SCI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose ECI slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_eci2sci(locstruc &loc);
        //! \brief Convert position from SCI to ECI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SCI slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2eci(locstruc &loc);
        //! \brief Convert position from SCI to SELC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SCI slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_sci2selc(locstruc &loc);
        //! \brief Convert position from SELC to SELG frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SELC slot is the source; SELG slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2selg(locstruc &loc);
        //! \brief Convert position from SELC to SCI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SELC slot is the source; SCI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2sci(locstruc &loc);
        //! \brief Convert position from SELG to SELC frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SELG slot is the source; SELC slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selg2selc(locstruc &loc);
        //! \brief Convert position from SELC to ECI frame within a locstruc (reference overload).
        //! \param loc Reference to the location structure whose SELC slot is the source; ECI slot is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_selc2eci(locstruc &loc);
        //! \brief Remove the LVLH offset from loc, converting the location back to its inertial origin (reference overload).
        //! \param loc Reference to the location structure with a valid LVLH offset applied (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh2origin(locstruc &loc);
        //! \brief Apply the LVLH offset stored in loc to shift the location away from its inertial origin (reference overload).
        //! \param loc Reference to the location structure whose pos.lvlh field holds the offset to apply (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_origin2lvlh(locstruc &loc);
        //! \brief Compute the LVLH offset of geoc relative to the base orbit (reference overload).
        //! \param geoc Reference to the location structure for the offset spacecraft (input).
        //! \param base Reference to the reference orbit location structure; LVLH offset is written to base.pos.lvlh (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_geoc2lvlh(locstruc &geoc, locstruc &base);
        //! \brief Reconstruct the GEOC position of a spacecraft from a base orbit and its LVLH offset (reference overload).
        //! \param base Reference to the reference orbit location structure (input).
        //! \param geoc Reference to the location structure to be filled with the recovered inertial position (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pos_lvlh2geoc(locstruc &base, locstruc &geoc);

        //! \brief Compute Keplerian orbital elements from an ECI Cartesian state vector.
        //! \param eci ECI position (m) and velocity (m/s) at a given epoch (input).
        //! \param kep Keplerian element structure populated by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t eci2kep(cartpos &eci, kepstruc &kep);
        //! \brief Compute an ECI Cartesian state vector from Keplerian orbital elements.
        //! \param kep Keplerian element structure providing the orbital elements (input).
        //! \param eci ECI position (m) and velocity (m/s) filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t kep2eci(kepstruc &kep,cartpos &eci);
        //! \brief Compute the quaternion that rotates from the inertial frame to the perifocal frame.
        //! \param cart ECI Cartesian state (position m, velocity m/s) used to define the perifocal plane (input).
        //! \param qperi Quaternion rotating from inertial to perifocal frame (output).
        //! \return 0 on success, negative error code on failure.
        int32_t cart2peri(cartpos cart, Quaternion &qperi);
        //! \brief Compute the quaternion that rotates from the perifocal frame back to the inertial frame.
        //! \param cart ECI Cartesian state (position m, velocity m/s) used to define the perifocal plane (input).
        //! \param qcart Quaternion rotating from perifocal to inertial frame (output).
        //! \return 0 on success, negative error code on failure.
        int32_t peri2cart(cartpos cart, Quaternion &qcart);
        //! \brief Convert a radial-intrack-crosstrack (RIC) offset to an absolute ECI Cartesian state.
        //! \param orig Origin spacecraft ECI state: position (m) and velocity (m/s) (input).
        //! \param ric RIC offset vector (m): [0]=radial, [1]=intrack, [2]=crosstrack (input).
        //! \param result Absolute ECI state of the offset point: position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t ric2eci(cartpos orig, rvector ric, cartpos& result);
        //! \brief Convert a RIC offset (Vector form) to an absolute ECI Cartesian state.
        //! \param orig Origin spacecraft ECI state: position (m) and velocity (m/s) (input).
        //! \param ric RIC offset vector (m): x=radial, y=intrack, z=crosstrack (input).
        //! \param result Absolute ECI state of the offset point: position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t ric2eci(cartpos orig, Vector ric, cartpos& result);
        //! \brief Approximate conversion from a RIC offset to an LVLH Cartesian offset.
        //! \param radius Orbital radius of the reference orbit (m).
        //! \param ric RIC Cartesian offset: position (m) and velocity (m/s) (input).
        //! \param lvlh LVLH Cartesian offset: position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t ric2lvlh(double radius, cartpos ric, cartpos& lvlh);
        //! \brief Approximate conversion from an LVLH Cartesian offset to a RIC offset.
        //! \param radius Orbital radius of the reference orbit (m).
        //! \param lvlh LVLH Cartesian offset: position (m) and velocity (m/s) (input).
        //! \param ric RIC Cartesian offset: position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t lvlh2ric(double radius, cartpos lvlh, cartpos& ric);
        //! \brief Compute the LVLH-frame position and velocity of a point relative to an ECI origin.
        //! \param origin ECI state of the LVLH frame origin: position (m) and velocity (m/s) (input).
        //! \param point ECI state of the point to transform: position (m) and velocity (m/s) (input).
        //! \return LVLH-frame position (m) and velocity (m/s) of the point relative to origin.
        cartpos eci2lvlh(cartpos origin, cartpos point);

        /**
         * \brief This function finds the interceptor's hill (relative) interceptor vectors
         * given the ECI target and ECI interceptor vectors.
         * Routine not dependent on km or m for distance unit, but needs to be consistent!
         * author        : sal alfano         719-573-2600   13 aug 2010
         * author        : SG                 July 2024 (conversion to C++)
         * 
         * \param tgteci Target's position/velocity in ECI
         * \param inteci Interceptor's position/velocity in ECI
         * \return Interceptor's position/velocity in Hill-frame (relative) vectors
         *      Output is the following axis definitions, which is the COSMOS LVLH frame:
         *      0: Normal to orbital plane in direction of velocity vector
         *      1: Radial vector
         *      2: Cross-product
         */
        Convert::cartpos eci2hill(const Convert::cartpos& tgteci, const Convert::cartpos& inteci);

        /**
         * \brief This function finds the interceptor's ECI pos/vel/acc vectors
         * given the ECI target and hill (relative) interceptor vectors.
         * Routine not dependent on km or m for distance unit, but needs to be consistent!
         * author        : sal alfano         719-573-2600   13 aug 2010
         * author        : SG                 July 2024 (conversion to C++, added acceleration)
         * 
         * \param tgteci Target's position/velocity/acceleration in ECI
         * \param inthill Interceptor's position/velocity/acceleration in Hill-frame (relative) vectors.
         *      Input expects the following axis definitions, which is the COSMOS LVLH frame:
         *      0: Normal to orbital plane in direction of velocity vector
         *      1: Radial vector
         *      2: Cross-product
         * \param relative_accel If true, the acceleration returned is relative to the target (i.e., when desiring external forces only)
         * \return Interceptor's position/velocity/acceleration in ECI
         */
        Convert::cartpos hill2eci (const Convert::cartpos& tgteci, const Convert::cartpos& inthill, bool relative_accel=false);


        //! \brief Generate an ECI locstruc for an orbit defined by altitude and phase angle relative to a reference.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param altitude Orbital altitude above the reference body surface (m).
        //! \param angle Phase angle around the orbit (radians).
        //! \param timeshift Time offset from the epoch used to advance the orbit (days).
        //! \return Fully populated locstruc with ECI position and velocity for the specified orbit.
        locstruc shape2eci(double utc, double altitude, double angle, double timeshift);
        //! \brief Generate an ECI locstruc for an orbit defined by ground latitude, longitude, altitude and phase.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param latitude Sub-satellite geodetic latitude (radians).
        //! \param longitude Sub-satellite geodetic longitude (radians).
        //! \param altitude Orbital altitude above the reference body surface (m).
        //! \param angle Phase angle around the orbit (radians).
        //! \param timeshift Time offset from the epoch used to advance the orbit (days).
        //! \return Fully populated locstruc with ECI position and velocity for the specified orbit.
        locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift);

        //! \brief Compute auxiliary attitude data (body-to-inertial rotation matrix) from the ICRF attitude.
        //! \param loc Pointer to the location structure whose att.icrf is already valid (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_extra(locstruc *loc);
        //! \brief Propagate all other attitude frames from the ICRF attitude slot.
        //! \param loc Pointer to the location structure whose ICRF attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf(locstruc *loc);
        //! \brief Propagate all other attitude frames from the LVLH attitude slot.
        //! \param loc Pointer to the location structure whose LVLH attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh(locstruc *loc);
        //! \brief Propagate all other attitude frames from the GEOC (ITRF) attitude slot.
        //! \param loc Pointer to the location structure whose GEOC attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_geoc(locstruc *loc);
        //! \brief Propagate all other attitude frames from the SELC (selenocentric) attitude slot.
        //! \param loc Pointer to the location structure whose SELC attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_selc(locstruc *loc);
        //! \brief Propagate all other attitude frames from the topocentric attitude slot.
        //! \param loc Pointer to the location structure whose topocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_topo(locstruc *loc);
        //! \brief Convert the planetocentric (GEOC or SELC) attitude to topocentric attitude.
        //! \param loc Pointer to the location structure; the closest-body planetocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_planec2topo(locstruc *loc);
        //! \brief Convert the topocentric attitude to the closest planetocentric (GEOC or SELC) attitude.
        //! \param loc Pointer to the location structure; topocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_topo2planec(locstruc *loc);
        //! \brief Convert the ICRF attitude to the GEOC (ITRF) attitude.
        //! \param loc Pointer to the location structure; ICRF attitude is the source, GEOC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2geoc(locstruc *loc);
        //! \brief Convert the ICRF attitude to the LVLH attitude.
        //! \param loc Pointer to the location structure; ICRF attitude is the source, LVLH attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2lvlh(locstruc *loc);
        //! \brief Convert the ICRF attitude to the SELC (selenocentric) attitude.
        //! \param loc Pointer to the location structure; ICRF attitude is the source, SELC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2selc(locstruc *loc);
        //! \brief Convert the GEOC (ITRF) attitude to the ICRF attitude.
        //! \param loc Pointer to the location structure; GEOC attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_geoc2icrf(locstruc *loc);
        //! \brief Convert the closest planetocentric attitude to the LVLH attitude.
        //! \param loc Pointer to the location structure; GEOC or SELC attitude is the source, LVLH attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_planec2lvlh(locstruc *loc);
        //! \brief Convert the LVLH attitude to the closest planetocentric (GEOC or SELC) attitude.
        //! \param loc Pointer to the location structure; LVLH attitude is the source, GEOC or SELC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh2planec(locstruc *loc);
        //! \brief Convert the LVLH attitude to the ICRF attitude.
        //! \param loc Pointer to the location structure; LVLH attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh2icrf(locstruc *loc);
        //! \brief Convert the SELC (selenocentric) attitude to the ICRF attitude.
        //! \param loc Pointer to the location structure; SELC attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_selc2icrf(locstruc *loc);
        //! \brief Identify the most recent position/attitude frame in the locstruc and propagate to all others.
        //! \param loc Pointer to the location structure to synchronize (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t loc_update(locstruc *loc);

        //! \brief Compute auxiliary attitude data from the ICRF attitude (reference overload).
        //! \param loc Reference to the location structure whose att.icrf is already valid (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_extra(locstruc &loc);
        //! \brief Zero-initialize an attstruc so that propagation can detect updates.
        //! \param att Reference to the attitude structure to clear (output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_clear(attstruc &att);
        //! \brief Propagate all other attitude frames from the ICRF attitude slot (reference overload).
        //! \param loc Reference to the location structure whose ICRF attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf(locstruc &loc);
        //! \brief Propagate all other attitude frames from the LVLH attitude slot (reference overload).
        //! \param loc Reference to the location structure whose LVLH attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh(locstruc &loc);
        //! \brief Propagate all other attitude frames from the GEOC attitude slot (reference overload).
        //! \param loc Reference to the location structure whose GEOC attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_geoc(locstruc &loc);
        //! \brief Propagate all other attitude frames from the SELC attitude slot (reference overload).
        //! \param loc Reference to the location structure whose SELC attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_selc(locstruc &loc);
        //! \brief Propagate all other attitude frames from the topocentric attitude slot (reference overload).
        //! \param loc Reference to the location structure whose topocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_topo(locstruc &loc);
        //! \brief Convert the planetocentric attitude to topocentric attitude (reference overload).
        //! \param loc Reference to the location structure; the closest-body planetocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_planec2topo(locstruc &loc);
        //! \brief Convert the topocentric attitude to the closest planetocentric attitude (reference overload).
        //! \param loc Reference to the location structure; topocentric attitude is the source (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_topo2planec(locstruc &loc);
        //! \brief Convert the ICRF attitude to the GEOC attitude (reference overload).
        //! \param loc Reference to the location structure; ICRF attitude is the source, GEOC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2geoc(locstruc &loc);
        //! \brief Convert the ICRF attitude to the LVLH attitude (reference overload).
        //! \param loc Reference to the location structure; ICRF attitude is the source, LVLH attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2lvlh(locstruc &loc);
        //! \brief Convert the ICRF attitude to the SELC attitude (reference overload).
        //! \param loc Reference to the location structure; ICRF attitude is the source, SELC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_icrf2selc(locstruc &loc);
        //! \brief Convert the GEOC attitude to the ICRF attitude (reference overload).
        //! \param loc Reference to the location structure; GEOC attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_geoc2icrf(locstruc &loc);
        //! \brief Convert the closest planetocentric attitude to the LVLH attitude (reference overload).
        //! \param loc Reference to the location structure; GEOC or SELC attitude is the source, LVLH attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_planec2lvlh(locstruc &loc);
        //! \brief Convert the LVLH attitude to the closest planetocentric attitude (reference overload).
        //! \param loc Reference to the location structure; LVLH attitude is the source, GEOC or SELC attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh2planec(locstruc &loc);
        //! \brief Convert the LVLH attitude to the ICRF attitude (reference overload).
        //! \param loc Reference to the location structure; LVLH attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_lvlh2icrf(locstruc &loc);
        //! \brief Convert the SELC attitude to the ICRF attitude (reference overload).
        //! \param loc Reference to the location structure; SELC attitude is the source, ICRF attitude is updated (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t att_selc2icrf(locstruc &loc);
        //! \brief Identify the most recent position/attitude frame and propagate to all others (reference overload).
        //! \param loc Reference to the location structure to synchronize (input/output).
        //! \return 0 on success, negative error code on failure.
        int32_t loc_update(locstruc &loc);

        //! \brief Convert Modified Julian Date (UTC) to Greenwich Mean Sidereal Time.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return GMST in radians.
        double mjd2gmst(double mjd);
        //! \brief Convert a geodetic position to UTM easting/northing.
        //! \param geod Geodetic position: lat (radians), lon (radians), h (m) (input).
        //! \param utm UTM coordinates: x=easting (m), y=northing (m) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geod2utm(geoidpos &geod, Vector &utm);
        //! \brief Convert a standalone GEOC Cartesian position to geodetic (lat/lon/alt).
        //! \param geoc GEOC Cartesian position: s in m, v in m/s (input).
        //! \param geod Geodetic position populated: lat (radians), lon (radians), h (m) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geoc2geod(Convert::cartpos &geoc, geoidpos &geod);
        //! \brief Convert a geographic spherical position to geocentric Cartesian.
        //! \param geos Pointer to the spherical geographic position: r (m), phi (radians), lambda (radians) (input).
        //! \param geoc Pointer to the geocentric Cartesian position filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geos2geoc(Convert::spherpos *geos, cartpos *geoc);
        //! \brief Convert a standalone geodetic position to GEOC Cartesian.
        //! \param geod Geodetic position: lat (radians), lon (radians), h (m) (input).
        //! \param geoc GEOC Cartesian position: s in m, v in m/s filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geod2geoc(geoidpos &geod, cartpos &geoc);
        //! \brief Convert a geocentric Cartesian position to geographic spherical.
        //! \param geoc Pointer to the geocentric Cartesian position (input).
        //! \param geos Pointer to the spherical geographic position filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geoc2geos(Convert::cartpos *geoc, spherpos *geos);
        //! \brief Convert a selenographic position to selenocentric Cartesian.
        //! \param selg Pointer to the selenographic position: lat (radians), lon (radians), h (m) (input).
        //! \param selc Pointer to the selenocentric Cartesian position filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t selg2selc(geoidpos *selg, cartpos *selc);
        //! \brief Compute rotation matrices from the Geocentric Celestial Reference Frame to the ITRS.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rnp Pointer to the precession+nutation rotation matrix (output).
        //! \param rm Pointer to the full GCRF-to-ITRS rotation matrix (output).
        //! \param drm Pointer to the first derivative of rm with respect to time (output).
        //! \param ddrm Pointer to the second derivative of rm with respect to time (output).
        //! \return 0 on success, negative error code on failure.
        int32_t gcrf2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
        //! \brief Compute rotation matrices from the ITRS to the Geocentric Celestial Reference Frame.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rnp Pointer to the precession+nutation rotation matrix (output).
        //! \param rm Pointer to the full ITRS-to-GCRF rotation matrix (output).
        //! \param drm Pointer to the first derivative of rm with respect to time (output).
        //! \param ddrm Pointer to the second derivative of rm with respect to time (output).
        //! \return 0 on success, negative error code on failure.
        int32_t itrs2gcrf(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
        //! \brief Compute the rotation matrix from True-of-Date to Pseudo-Earth-Fixed (PEF).
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t true2pef(double utc, rmatrix *rm);
        //! \brief Compute the rotation matrix from PEF to True-of-Date.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pef2true(double utc, rmatrix *rm);
        //! \brief Compute the rotation matrix from PEF to ITRS (applies polar motion).
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t pef2itrs(double utc, rmatrix *rm);
        //! \brief Compute the rotation matrix from ITRS to PEF (inverse polar motion).
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t itrs2pef(double utc, rmatrix *rm);
        //! \brief Compute the rotation matrix from True-of-Epoch to Mean-of-Epoch (removes nutation).
        //! \param ep1 Epoch in Modified Julian Date (UTC).
        //! \param pm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t true2mean(double ep1, rmatrix *pm);
        //! \brief Compute the rotation matrix from Mean-of-Epoch to True-of-Epoch (adds nutation).
        //! \param ep0 Epoch in Modified Julian Date (UTC).
        //! \param pm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t mean2true(double ep0, rmatrix *pm);
        //! \brief Compute the rotation matrix from J2000 to Mean-of-Epoch (precession forward).
        //! \param ep1 Target epoch in Modified Julian Date (UTC).
        //! \param pm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t j20002mean(double ep1, rmatrix *pm);
        //! \brief Compute the rotation matrix from Mean-of-Epoch to J2000 (precession backward).
        //! \param ep0 Source epoch in Modified Julian Date (UTC).
        //! \param pm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t mean2j2000(double ep0, rmatrix *pm);
        //! \brief Compute the small bias rotation matrix from GCRF to J2000.
        //! \param rm Pointer to the resulting bias rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t gcrf2j2000(rmatrix *rm);
        //! \brief Compute the small bias rotation matrix from J2000 to GCRF.
        //! \param rm Pointer to the resulting bias rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t j20002gcrf(rmatrix *rm);
        //! \brief Compute the rotation matrix from TEME (True Equator Mean Equinox) to True-of-Date.
        //! \param ep0 Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t teme2true(double ep0, rmatrix *rm);
        //! \brief Compute the rotation matrix from True-of-Date to TEME.
        //! \param ep0 Epoch in Modified Julian Date (UTC).
        //! \param rm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t true2teme(double ep0, rmatrix *rm);
        //! \brief Compute the precession rotation matrix from Mean-of-Epoch ep0 to Mean-of-Epoch ep1.
        //! \param ep0 Start epoch in Modified Julian Date (UTC).
        //! \param ep1 End epoch in Modified Julian Date (UTC).
        //! \param pm Pointer to the resulting rotation matrix (output).
        //! \return 0 on success, negative error code on failure.
        int32_t mean2mean(double ep0, double ep1, rmatrix *pm);
        //! \brief Compute the topocentric position vector of a target as seen from a ground source.
        //! \param gs Geodetic location of the observer: lat (radians), lon (radians), h (m) (input).
        //! \param geoc GEOC Cartesian position of the target (m) (input).
        //! \param topo Topocentric position vector of the target relative to the observer (m) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geoc2topo(gvector gs, rvector geoc, rvector &topo);
        //! \brief Compute the topocentric position of a target relative to a body-centric source.
        //! \param com Body-centric Cartesian position of the observer (m) (input).
        //! \param body Body-centric Cartesian position of the target (m) (input).
        //! \param topo Topocentric position vector of the target (m) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t body2topo(Vector com, Vector body, Vector &topo);
        //! \brief Compute azimuth and elevation from a topocentric position vector.
        //! \param tpos Topocentric position vector (m) (input).
        //! \param az Computed azimuth (radians, East of North) (output).
        //! \param el Computed elevation above the horizontal (radians) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t topo2azel(rvector tpos, float &az, float &el);
        //! \brief Compute azimuth and elevation from a topocentric position vector (Vector form).
        //! \param tpos Topocentric position vector (m) (input).
        //! \param az Computed azimuth (radians, East of North) (output).
        //! \param el Computed elevation above the horizontal (radians) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t topo2azel(Vector tpos, float &az, float &el);
        //! \brief Intersect a satellite body-frame pointing vector with the Earth surface and return the geocentric position.
        //! \param sat Unit vector in the satellite body frame indicating the pointing direction (input).
        //! \param loc Location structure providing the satellite position and attitude (input).
        //! \param pos Geocentric Cartesian position of the intersection point (m) (output).
        //! \return Distance from satellite to intersection point (m), or 0 if the vector does not intersect.
        int32_t sat2geoc(rvector sat, locstruc &loc, rvector &pos);
        //! \brief Compute the great-circle separation distance between two geodetic points.
        //! \param src Starting geodetic position: lat (radians), lon (radians) (input).
        //! \param dst Destination geodetic position: lat (radians), lon (radians) (input).
        //! \param sep Great-circle distance between the two points (m) (output).
        //! \return 0 on success, negative error code on failure.
        int32_t geod2sep(gvector src, gvector dst, double &sep);
        //! \brief Compute the great-circle separation distance between two geodetic points (value overload).
        //! \param src Starting geodetic position: lat (radians), lon (radians) (input).
        //! \param dst Destination geodetic position: lat (radians), lon (radians) (input).
        //! \return Great-circle distance between the two points (m).
        double geod2sep(gvector src, gvector dst);
        //! \brief Propagate an array of TLEs to a given epoch and return the ECI state.
        //! \param mjd Target epoch in Modified Julian Date (UTC).
        //! \param tle Vector of TLE structures spanning the desired epoch (input).
        //! \param eci ECI Cartesian position (m) and velocity (m/s) at the requested epoch (output).
        //! \return 0 on success, negative error code (e.g. TLE_ERROR_OUTOFRANGE) on failure.
        int lines2eci(double mjd, vector<tlestruc> tle, cartpos &eci);
        //! \brief Propagate a single TLE to a given epoch using SGP4 and return the ECI state.
        //! \param mjd Target epoch in Modified Julian Date (UTC).
        //! \param tle TLE structure for the satellite (input).
        //! \param eci ECI Cartesian position (m) and velocity (m/s) at the requested epoch (output).
        //! \return 0 on success, negative error code on failure.
        int tle2eci(double mjd, tlestruc tle, cartpos &eci);
        //! \brief Four-quadrant arctangent using sine and cosine of angle.
        //! \param sa Sine of the angle (input).
        //! \param cb Cosine of the angle (input).
        //! \return Angle in radians in the range [0, 2*pi).
        double atan3(double sa, double cb);
        //! \brief Convert an ECI state vector to a TLE using a direct Keplerian fit.
        //! \param eci ECI Cartesian state: position (m) and velocity (m/s) with a valid utc field (input).
        //! \param tle TLE structure filled with the resulting orbital elements (output).
        //! \return 0 on success, negative error code (e.g. -1 if eccentricity >= 1) on failure.
        int32_t eci2tle(cartpos eci, tlestruc &tle);
        //! \brief Convert an ECI state vector to a TLE using iterative SGP4 inversion.
        //! \param eci ECI Cartesian state: position (m) and velocity (m/s) with a valid utc field (input).
        //! \param tle TLE structure filled with the converged orbital elements (output).
        //! \return 0 on success, negative error code on failure.
        int32_t eci2tle2(cartpos eci, tlestruc &tle);
//        int32_t rv2tle(double utc, cartpos eci, tlestruc &tle);
        //! \brief Run the SGP4 propagator and return the TEME-frame state at the given epoch.
        //! \param utc Target epoch in Modified Julian Date (UTC).
        //! \param tle TLE structure for the satellite (input).
        //! \param pos_teme TEME-frame Cartesian position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code on failure.
        int sgp4(double utc, tlestruc tle, cartpos &pos_teme);
        //! \brief Retrieve a TLE entry from a vector by index.
        //! \param index Zero-based index into the TLE vector.
        //! \param tle Vector of TLE structures (input).
        //! \return The TLE at the requested index, or an empty TLE with utc=0 if the index is out of range.
        tlestruc get_line(uint16_t index, vector<tlestruc> tle);
        //! \brief Load a single-satellite TLE file into a vector of TLE structures.
        //! \param fname Path to the file containing TLE data (input).
        //! \param tle Vector of TLE structures populated from the file (output).
        //! \return Number of TLEs loaded on success, negative error code on failure.
        int32_t load_lines(string fname, vector<tlestruc>& tle);
        //! \brief Load a multi-satellite TLE file into a vector of TLE structures.
        //! \param fname Path to the file containing TLE data for multiple satellites (input).
        //! \param tle Vector of TLE structures populated from the file (output).
        //! \return Number of TLEs loaded on success, negative error code on failure.
        int32_t load_lines_multi(string fname, vector<tlestruc>& tle);
        //! \brief Load a single TLE from a file (deprecated alias for load_tle).
        //! \param fname Path to the TLE file (input).
        //! \param tle TLE structure populated from the file (output).
        //! \return 0 on success, negative error code on failure.
        int32_t loadTLE(char *fname, tlestruc &tle);
        //! \brief Load a single TLE from a file (char* overload).
        //! \param fname Path to the TLE file (input).
        //! \param tle TLE structure populated from the file (output).
        //! \return 0 on success, negative error code on failure.
        int32_t load_tle(char *fname, tlestruc &tle);
        //! \brief Load a single TLE from a file (string overload).
        //! \param fname Path to the TLE file (input).
        //! \param tle TLE structure populated from the file (output).
        //! \return 0 on success, negative error code on failure.
        int32_t load_tle(string fname, tlestruc &tle);
        //! \brief Load an STK-format ephemeris file (J2000 position/velocity/acceleration in metres).
        //! \param filename Path to the STK ephemeris file (input).
        //! \param stkdata STK structure populated with the tabulated states (output).
        //! \return Number of entries loaded on success, negative error code on failure.
        int32_t load_stk(string filename, stkstruc &stkdata);
        //! \brief Interpolate an ECI state from an STK ephemeris table at the given epoch.
        //! \param utc Target epoch in Modified Julian Date (UTC).
        //! \param stk STK structure containing the tabulated states (input).
        //! \param eci Interpolated ECI position (m) and velocity (m/s) (output).
        //! \return 0 on success, negative error code (STK_ERROR_LOWINDEX, STK_ERROR_HIGHINDEX) on failure.
        int stk2eci(double utc, stkstruc &stk, cartpos &eci);
        //! \brief Convert a TLE to an SGP4 internal parameter structure.
        //! \param tle Input TLE structure (input).
        //! \param sgp4 SGP4 parameter structure filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t tle2sgp4(tlestruc tle, sgp4struc &sgp4);
        //! \brief Convert an SGP4 internal parameter structure back to a TLE.
        //! \param sgp4 SGP4 parameter structure (input).
        //! \param tle TLE structure filled by the conversion (output).
        //! \return 0 on success, negative error code on failure.
        int32_t sgp42tle(sgp4struc sgp4, tlestruc &tle);
        //! \brief Compute the checksum digit for a TLE line.
        //! \param line Pointer to a null-terminated TLE line string (input).
        //! \return Checksum digit (0-9).
        int tle_checksum(const char *line);
        //! \brief Format an ECI state as a TLE string, using a reference TLE for non-dynamic fields.
        //! \param eci ECI Cartesian state: position (m) and velocity (m/s) (input).
        //! \param reftle Reference TLE providing satellite name, BSTAR, and other fixed fields (input/output).
        //! \return Two-line TLE string representing the ECI state.
        string eci2tlestring(Convert::cartpos eci, tlestruc &reftle);
        //! \brief Format a TLE structure as a standard two-line TLE text string.
        //! \param tles TLE structure to format (input).
        //! \return Two-line TLE string.
        string tle2tlestring(tlestruc tles);
        //! \brief Compute IAU 1980 nutation angles (Delta-psi and Delta-epsilon) for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return rvector where col[0] = Delta-psi (radians) and col[1] = Delta-epsilon (radians).
        rvector utc2nuts(double mjd);
        //! \brief Compute the nutation Delta-epsilon (obliquity correction) for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Delta-epsilon in radians.
        double utc2depsilon(double mjd);
        //! \brief Compute the nutation Delta-psi (longitude correction) for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Delta-psi in radians.
        double utc2dpsi(double mjd);
        //! \brief Compute Greenwich Apparent Sidereal Time (GAST) for a given UTC epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return GAST in radians in the range [0, 2*pi).
        double utc2gast(double mjd);
        //! \brief Convert UTC to UT1 using the IERS Bulletin A DUT1 table.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return UT1 in Modified Julian Date, or 0 if the IERS table is unavailable.
        double utc2ut1(double mjd);
        //! \brief Compute DUT1 = UT1 - UTC from the IERS Bulletin A table.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return DUT1 in days (typically a few hundred milliseconds), or 0 if the table is unavailable.
        double utc2dut1(double mjd);
        //! \brief Load the IERS Bulletin A table from disk if not already loaded.
        //! \return Number of entries loaded (> 0) on success, 0 or negative if the file is unavailable.
        int32_t load_iers();
        //! \brief Retrieve the polar motion (x_p, y_p) for a given UTC epoch from the IERS table.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return cvector with x = x_p (radians) and y = y_p (radians).
        cvector polar_motion(double mjd);
        //! \brief Return the number of accumulated leap seconds at the given UTC epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Total leap seconds (integer count), or negative error code if the table is unavailable.
        int32_t leap_seconds(double mjd);
        //! \brief Compute Earth Rotation Angle (ERA) for a given UTC epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return ERA in radians.
        double utc2era(double mjd);
        //! \brief Compute Greenwich Mean Sidereal Time using the IAU 1982 model.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return GMST in radians.
        double utc2gmst1982(double mjd);
        //! \brief Compute Greenwich Mean Sidereal Time using the IAU 2000 model.
        //! \param mjd Epoch in Modified Julian Date (UTC); defaults to 0 (uses internally cached value).
        //! \return GMST in radians.
        double utc2gmst2000(double mjd=0.);
        //! \brief Compute Julian centuries of UT1 from J2000.0 for nutation calculations.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Julian centuries of UT1 since J2000.0 (dimensionless).
        double utc2jcenut1(double mjd);
        //! \brief Convert UTC to Terrestrial Time (TT) using the leap second table.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return TT in Modified Julian Date, or negative error code if the IERS table is unavailable.
        double utc2tt(double mjd);
        //! \brief Compute Julian centuries of TT from J2000.0 for precession/nutation calculations.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Julian centuries of TT since J2000.0 (dimensionless).
        double utc2jcentt(double mjd);
        //! \brief Compute the mean obliquity of the ecliptic for a given UTC epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Mean obliquity in radians.
        double utc2epsilon(double mjd);
        //! \brief Compute the fundamental argument L (mean longitude of the Moon minus mean anomaly of the Moon).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Fundamental argument L in radians.
        double utc2L(double mjd);
        //! \brief Compute the fundamental argument L' (mean anomaly of the Sun).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Fundamental argument L' in radians.
        double utc2Lp(double mjd);
        //! \brief Compute the fundamental argument F (mean longitude of the Moon minus ascending node longitude).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Fundamental argument F in radians.
        double utc2F(double mjd);
        //! \brief Compute the fundamental argument D (mean elongation of the Moon from the Sun).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Fundamental argument D in radians.
        double utc2D(double mjd);
        //! \brief Compute the fundamental argument Omega (longitude of the Moon's ascending node).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Fundamental argument Omega in radians.
        double utc2omega(double mjd);
        //! \brief Compute the precession parameter zeta for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Precession parameter zeta in radians.
        double utc2zeta(double mjd);
        //! \brief Compute the precession parameter z for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Precession parameter z in radians.
        double utc2z(double mjd);
        //! \brief Convert UTC to GPS time.
        //! \param utc Epoch in Modified Julian Date (UTC).
        //! \return GPS time in Modified Julian Date.
        double utc2gps(double utc);
        //! \brief Convert UTC to Barycentric Dynamical Time (TDB).
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return TDB in Modified Julian Date.
        double utc2tdb(double mjd);
        //! \brief Compute the precession parameter theta for a given epoch.
        //! \param mjd Epoch in Modified Julian Date (UTC).
        //! \return Precession parameter theta in radians.
        double utc2theta(double mjd);
        //! \brief Convert Terrestrial Time (TT) to UTC using the leap second table.
        //! \param mjd Epoch in Modified Julian Date (TT).
        //! \return UTC in Modified Julian Date, or 0 if the IERS table is unavailable.
        double  tt2utc(double mjd);
        //! \brief Convert GPS time to UTC.
        //! \param gps GPS time in Modified Julian Date.
        //! \return UTC in Modified Julian Date.
        double  gps2utc(double gps);
        //! \brief Compute the geocentric radius of the Earth at a given geodetic latitude.
        //! \param lat Geodetic latitude in radians.
        //! \return Earth radius at the given latitude (m), accounting for oblateness.
        double Rearth(double lat);

                class Position {

                public:

                    explicit Position(Vector s0=Vector(), Vector v0=Vector(), Vector a0=Vector())
                    {
                        s = s0;
                        v = v0;
                        a = a0;
                    }

                    // 0th derivative
                    Vector s;
                    // 1st derivative
                    Vector v;
                    // 2nd derivative
                    Vector a;
                };
        //! @}
	json11::Json make_swarm_information_object(const string& name, const string& desc, const vector<string>& node_names);
	json11::Json make_sensor_information_object(const string& sensor_name, const double& fov, const double& ifov);
	json11::Json make_constraints_information_object(const double& max_slew_rate = 0.0, const double& max_thrust_total = 0.0, const double& max_thrust_impulse = 0.0);
	json11::Json make_tle_information_object(const string& tle_file = "/home/user/cosmos/source/core/data/demo/tle.dat");
	json11::Json make_node_information_object(json11::Json swarm_info_obj);
    } // end Convert namespace
} // end Cosmos namespace
#endif
