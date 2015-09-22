#ifndef _MATH_CONSTANTS_H
#define _MATH_CONSTANTS_H

//! \ingroup mathlib
//! \defgroup mathlib_constants Math library constants
//! @{

//! PI
#define PI 3.1415926535897932384626433832795028841971693993751
//! Double precision PI
#define DPI 3.1415926535897932384626433832795028841971693993751
//! Double precision 2*PI
#define D2PI 6.2831853071795864769252867665590057683943387987502
//! Double precision PI/2
#define DPI2 1.5707963267948966192313216916397514420985846996876
//! Multiplicand for Seconds of Time to Radians
#define DS2R 7.2722052166430399038487115353692196393452995355905e-5
//! Multiplicand for Seconds of Arc to Radians
#define DAS2R 4.8481368110953599358991410235794797595635330237270e-6

//! Multiplicand for Degrees to Radians
#define DTOR            (DPI / (double)180.)
//! Multiplicand for Radians to Degrees
#define RTOD            ((double)180. / DPI)
//! Radians of a Degree value
#define RADOF(deg)  (double)(DTOR * (deg))
//! Radians of a Degree value
#define DEG2RAD(deg)  (double)(DTOR * (deg))
//! Degrees of a Radian value
#define DEGOF(rad)  (double)(RTOD * (rad))
//! Degrees of a Radian value
#define RAD2DEG(rad)  (double)(RTOD * (rad))

#define O_UNDEFINED 999999.1
#define O_INFINITE 10000000000000000000000000.9
#define O_SMALL 0.00000001
#define D_SMALL ((double)1e-76)

//! Big Endian byte order
#define ORDER_BIGENDIAN 0
//! PowerPC byte order
#define ORDER_PPC ORDER_BIGENDIAN
//! Motorola byte order
#define ORDER_MOTOROLA ORDER_BIGENDIAN
//! Little Endian byte order
#define ORDER_LITTLEENDIAN 1
//! Intel byte order
#define ORDER_INTEL ORDER_LITTLEENDIAN
//! Network byte order
#define ORDER_NETWORK ORDER_BIGENDIAN

#define DIRECTION_ROW 0
#define DIRECTION_COLUMN 1

#define ESTIMATOR_SIZE 5
//! @}

#endif //_MATH_CONSTANTS_H
