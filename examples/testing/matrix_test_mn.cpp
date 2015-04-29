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

#include "mathlib.h"
#include <stdio.h>

//!
//!

int main(int argc, char *argv[])
{

    // different types
    rvector vec1;

    // different ways to initialize vectors and matrices

    vec1.col[0] = 1.;
    vec1.col[1] = 0.;
    vec1.col[2] = 0.;

    //vec1 = rv_zero(); // initialize with zeros

    rvector vector1 = {{3.,3.,0.}};
    rmatrix matrix1 = {{{{11,12,13}},
                        {{21,22,23}},
                        {{31,32,33}}}};


    //vector1.col[0] = 1.;

    // multiply matrix by vector
    //rv_mult()

    vec1 = rv_mmult(matrix1,vector1);

    printf("[%11f %11f %11f] \n",vector1.col[0],vector1.col[1],vector1.col[2]);

    printf("[%11f %11f %11f] \n",vec1.col[0],vec1.col[1],vec1.col[2]);

    printf("test\n");

    //! please refer to examples/math/myMatrxi2D.cpp

}
