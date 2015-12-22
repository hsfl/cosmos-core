#ifndef _MATH_CONSTANTS_H
#define _MATH_CONSTANTS_H

#include <cstdint>

//! \ingroup mathlib
//! \defgroup mathlib_constants Math library constants
//! @{

//! PI
const double PI=3.1415926535897932384626433832795028841971693993751;
//! Double precision PI
const double DPI=3.1415926535897932384626433832795028841971693993751;
//! Double precision 2*PI
const double D2PI=6.2831853071795864769252867665590057683943387987502;
//! Double precision PI/2
const double DPI2=1.5707963267948966192313216916397514420985846996876;
//! Multiplicand for Seconds of Time to Radians
const double DS2R=7.2722052166430399038487115353692196393452995355905e-5;
//! Multiplicand for Seconds of Arc to Radians
const double DAS2R=4.8481368110953599358991410235794797595635330237270e-6;

//! Multiplicand for Degrees to Radians
const double DTOR=(DPI / (double)180.);
//! Multiplicand for Radians to Degrees
const double RTOD=((double)180. / DPI);
//! Radians of a Degree value
#define RADOF(deg)  (double)(DTOR * (deg))
//! Radians of a Degree value
#define DEG2RAD(deg)  (double)(DTOR * (deg))
//! Degrees of a Radian value
#define DEGOF(rad)  (double)(RTOD * (rad))
//! Degrees of a Radian value
#define RAD2DEG(rad)  (double)(RTOD * (rad))

const double O_UNDEFINED=999999.1;
const double O_INFINITE=10000000000000000000000000.9;
const double O_SMALL=0.00000001;
const double D_SMALL=((double)1e-76);

// BIGENDIAN and LITTLEENDIAN are defined somewhere else. This lets us place them in
// the ByteOrder class.
#undef BIGENDIAN
#undef LITTLEENDIAN

//! Enumeration of possible byte orders
enum class ByteOrder : std::uint8_t {
    //! Big Endian byte order
    BIGENDIAN=0,
    //! PowerPC byte order
    PPC=ByteOrder::BIGENDIAN,
    //! Motorola byte order
    MOTOROLA=ByteOrder::BIGENDIAN,
    //! Little Endian byte order
    LITTLEENDIAN=1,
    //! Intel byte order
    INTEL=ByteOrder::LITTLEENDIAN,
    //! Network byte order
    NETWORK=ByteOrder::BIGENDIAN
};

#define DIRECTION_ROW 0
#define DIRECTION_COLUMN 1

#define ESTIMATOR_SIZE 5
//! @}

#endif //_MATH_CONSTANTS_H
