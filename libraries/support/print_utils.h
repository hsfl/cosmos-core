// COSMOS Library for Printing Vector and stuff like that to the stream

// v1.0 : 2014-10-05
// library created

//---------------------------------------------------------------
// Print class

#include <iostream>
#include <string>

// ?? remove the dependency of matlib later, change rvector to vector?
#include "mathlib.h"

class PrintUtils {

private:

public:

    //main functions
    //-------------------------------------------------------------
    // print functions to help visualize the vectors and other data

    // main function
    void vector(string text_prefix, rvector v, double scale, string text_suffix, int precision);



    // split vector
    void vector(string vector_name, double a, double b, double c, string units);
    void vector(string vector_name, double a, double b, double c, string units, int precision);
    void vector(string vector_name, double a, double b, double c, double d, string units);

    void vector(rvector v);
    void vector(rvector v, int precision);

    void vector(string vector_name, rvector v);
    void vector(string vector_name, rvector v, int precision);
    void vector(string vector_name, rvector v, string text_suffix, int precision);

    void vectorScaled(string vector_name, rvector v, double scale, int precision);

    void vector_endl(rvector v,
                     double factor,
                     string units,
                     int precision);

    void vector2(string name_v1, rvector v1, string name_v2, rvector v2);
    void vector2(string name_v1, rvector v1, string name_v2, rvector v2, int precision);
    void end();

};
