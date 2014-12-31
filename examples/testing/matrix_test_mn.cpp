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
