// COSMOS Library for Printing Vector and stuff like that to the stream

#include "print_utils.h"

//-------------------------------------------------------------
// print functions to help visualize vectors and other data


// main print vector function with all options
void PrintUtils::vector(
        string text_prefix,
        rvector v,
        double scale,
        string text_suffix,
        int precision){

    if (precision != -1){
        // with set precision
        cout.precision(precision);
        cout << fixed;

    }
    cout << text_prefix << "[" << v.col[0]*scale << ", " << v.col[1]*scale <<  ", " << v.col[2]*scale << "]" << text_suffix;

}


void PrintUtils::vector(rvector v){
    // this prints the vector v enclosed in brackets like this: [x,y,z]
    //cout << v;
    vector("", v, 1, " ", -1);
}

void PrintUtils::vector(rvector v, int precision){
    cout.precision(precision);
    // this prints the vector v enclosed in brackets like this: [x,y,z]
    cout << fixed << "[" << v.col[0] << ", " << v.col[1] <<  ", " << v.col[2] << "]";
}


void PrintUtils::vector_endl(rvector v,
                             double factor,
                             string units,
                             int precision){
    cout.precision(precision);
    // this prints the vector v enclosed in brackets like this: [x,y,z]
    cout << fixed << "[" << v.col[0]*factor << ", " << v.col[1]*factor <<  ", " << v.col[2]*factor << "] " << units << endl;
}


void PrintUtils::vector(string vector_name, rvector v){
    //cout << vector_name << ": " << v;
    vector(vector_name, v, 1, " ", -1);
}

void PrintUtils::vector(string vector_name, rvector v, int precision){
    vector(vector_name, v, 1, " ", precision);
}

//void PrintUtils::vector(string vector_name, rvector v, double scale, int precision){
//    vector(vector_name, v, scale, " ", precision);
//}

void PrintUtils::vector(string vector_name, rvector v, string text_suffix, int precision){
     vector(vector_name, v, 1, text_suffix, precision);
}

void PrintUtils::vectorScaled(string vector_name, rvector v, double scale, int precision){
    vector(vector_name, v, scale, " ", precision);
}


// overloaded function with setprecision
void PrintUtils::vector(string vector_name,
                        double a,
                        double b,
                        double c,
                        string units){
    cout << vector_name << "[" << a << ", " << b <<  ", " << c << "] " << units << endl;
}

// overloaded function with setprecision
void PrintUtils::vector(string vector_name,
                        double a,
                        double b,
                        double c,
                        string units,
                        int precision){
    cout.precision(precision);
    cout << vector_name << fixed << "[" << a << ", " << b <<  ", " << c << "] " << units << endl;
}

void PrintUtils::vector(string vector_name,
                        double a,
                        double b,
                        double c,
                        double d,
                        string units){
    cout << vector_name << "[" << a << ", " << b <<  ", " << c << ", " << d << "] " << units << endl;
}






void PrintUtils::vector2(string name_v1, rvector v1, string name_v2, rvector v2){
    cout << name_v1 << ": " << v1 << " | " << name_v2 << ": " << v2 << endl;
}

void PrintUtils::vector2(string name_v1, rvector v1, string name_v2, rvector v2, int precision){
    cout.precision(precision);
    cout << name_v1 << fixed << "[" << v1.col[0] << ", " << v1.col[1] <<  ", " << v1.col[2] << "]" << " | " << name_v2 << "[" << v2.col[0] << ", " << v2.col[1] <<  ", " << v2.col[2] << "]" << endl;
}

void PrintUtils::end(){
    cout << endl;
}
