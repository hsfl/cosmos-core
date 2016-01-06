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

#ifndef _MATH_LSFIT_H
#define _MATH_LSFIT_H

#include "vector.h"
#include "quaternion.h"
// TODO: remove dependency from mathlib (uvector)
#include "mathlib.h"

#include <cstdint>
#include <deque>

// TODO: bring lsfit from mathlib

//class lsfit
//{
//private:
//        //! Least Squares Fit Element
//        /*! Contains the dependent (x) and independent (y) values for a single element of a ::fitstruc.
//         * The ::uvector allows both quaternions and rvector to be fit.
//         */
//    struct fitelement
//        {
//                // Independent variable
//                double x;
//                // Double, rvector or quaternion dependent variable
//                uvector y;
//    } ;

//        //! Least Squares Fit Structure
//        /*! Contains the data for a second order least squares fit of N elements that are type
//         * ::rvector or ::quaternion.
//         */
//        // Number of elements in fit
//        uint16_t element_cnt;
//        // Number of axes (double, rvector, quaternion)
//        uint16_t depth;
//        // Order of fit
//        uint32_t order;
//        // base level subtracted from independent variable before fitting
//        double basex;
//        // Of size element_cnt
//        std::deque<fitelement> var;

//        std::vector< std::vector<double> > parms;

//        void fit();

//public:
//        double meanx;
//        uvector meany;
//        double stdevx;
//        uvector stdevy;
//        // Minimum reasonable step in dependent variable
//        double resolution;

//    // constructors
//        lsfit();
//        lsfit(uint16_t element_cnt);
//        lsfit(uint16_t element_cnt, uint16_t ord);
//        lsfit(uint16_t cnt, uint16_t ord, double res);

//        void update(double x, double y);
//        void update(double x, rvector y);
//        void update(double x, quaternion y);
//        void update(fitelement cfit, uint16_t dep);
//        double lastx();
//        double eval(double x);
//        rvector evalrvector(double x);
//        quaternion evalquaternion(double x);
//        double slope(double x);
//        rvector slopervector(double x);
//        quaternion slopequaternion(double x);
//        std::vector<std::vector<double> > getparms(double x);
//};

#endif // _MATH_LSFIT_H
