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

#include "configCosmos.h"
// COSMOS Library for Printing Vector and stuff like that to the stream

#include "print_utils.h"

//-------------------------------------------------------------
// print functions to help visualize vectors and other data


PrintUtils::PrintUtils()
{
    reset();
}

void PrintUtils::reset()
{
    // default values
    precision      = -1; // start with no set precision
    fieldwidth     = -1; // start with no set fieldwidth
    scale          = 1;  // start with no scale factor
    prefix         = ""; // start with no prefix text
    suffix         = ""; // start with no suffix text
    use_brackets   = false; // start with no brackets around text
    delimiter      = ","; // start with coma delimiter
    delimiter_flag = false; // start with no delimiter flags, user has to turn on this flag for use
}

void PrintUtils::text(string text)
{
    string out;

    if (delimiter_flag)
    {
        out = text + delimiter;
    }
    else
    {
        out = text;
    }

    cout << out;

    // add to full message
    fullMessage += out;

}




// main print vector function with all options
void PrintUtils::vector(
        string prefix,
        rvector v,
        double scale,
        string suffix,
        int precision,
        int fieldwidth)
{

    std::ostringstream out;

    out << prefix;

    if (delimiter_flag)
    {
        out << delimiter;
    }

    if (use_brackets)
    {
        out << "[";
    }

    // in case we don't want to print the vector with fixed width
    if (precision != -1)
    {
        // with set precision
        //cout.precision(precision);
        out << std::fixed;
        out << std::setprecision(precision);
    }

    if (fieldwidth != -1)
    {
        // print with fixed width
        // std::cout.width(filedwidth);

        out << std::setw(fieldwidth) << v.col[0]*scale << ","
             << std::setw(fieldwidth) << v.col[1]*scale <<  ","
             << std::setw(fieldwidth) << v.col[2]*scale;

    }
    else
    {
        // simple print, no fixed width
        out << v.col[0]*scale << ","
            << v.col[1]*scale <<  ","
            << v.col[2]*scale;
    }

    if (use_brackets)
    {
        out << "]";
    }

    out << suffix;

    if (delimiter_flag)
    {
        out << delimiter;
    }

    cout << out.str();

    fullMessage += out.str();
}


void PrintUtils::vector(rvector v)
{
    // simple print
    vector(prefix, v, scale, suffix, precision, fieldwidth);
}


//void PrintUtils::vector(rvector v, int precision)
//{
//    vector("", v, 1., "", precision, -1);
//}


void PrintUtils::vector(string prefix, rvector v)
{
    vector(prefix, v, scale, suffix, precision, fieldwidth);
}

//void PrintUtils::vector(string vector_name, rvector v, int precision)
//{
//    vector(vector_name, v, 1, "", precision, -1);
//}

//void PrintUtils::vector(string vector_name, rvector v, double scale, int precision){
//    vector(vector_name, v, scale, " ", precision);
//}

void PrintUtils::vector(string vector_name,
                        rvector v,
                        string suffix)
{
    vector(vector_name, v, scale, suffix, precision, fieldwidth);
}


// ----------------------------------------------
// Scalar prints


// main print vector function with all options
void PrintUtils::scalar(
        string prefix,
        double s,
        double scale,
        string suffix,
        int precision,
        int fieldwidth)
{

    std::ostringstream out;

    out << prefix;

    if (prefix == "")
    {
        // don't print delimiter
    }
    else
    {
        if (delimiter_flag)
        {
            out << delimiter;
        }
    }



    if (use_brackets)
    {
        out << "[";
    }

    // in case we don't want to print the vector with fixed width
    if (precision != -1)
    {
        out << std::fixed;
        out << std::setprecision(precision);
    }

    if (fieldwidth != -1)
    {
        out << std::setw(fieldwidth);
    }

    out << s*scale;

    out << suffix;

    if (use_brackets)
    {
        out << "]";
    }

    if (delimiter_flag)
    {
        out << delimiter;
    }

    cout << out.str();

    fullMessage += out.str();
}

void PrintUtils::scalar(double s)
{
    scalar(prefix, s, scale, suffix, precision, fieldwidth);
}

void PrintUtils::scalar(string prefix,
                        double s,
                        string suffix)
{
    scalar(prefix, s, scale, suffix, precision, fieldwidth);
}

void PrintUtils::scalar(string prefix,
                        double s)
{
    //    if (delimiter_flag)
    //    {
    //        scalar(prefix + delimiter, s, scale, suffix, precision, fieldwidth);
    //    }
    //    else
    //    {
    //        scalar(prefix, s, scale, suffix, precision, fieldwidth);
    //    }

    scalar(prefix, s, scale, suffix, precision, fieldwidth);
}


void PrintUtils::scalar(double s,
                        string suffix)
{
    scalar("", s, scale, suffix, precision, fieldwidth);
}






//void PrintUtils::vector_endl(rvector v,
//                             double factor,
//                             string units,
//                             int precision)
//{
//    cout.precision(precision);
//    // this prints the vector v enclosed in brackets like this: [x,y,z]
//    cout << fixed << "[" << v.col[0]*factor << ", " << v.col[1]*factor <<  ", " << v.col[2]*factor << "] " << units << endl;
//}


//void PrintUtils::vectorScaled(string vector_name,
//                              rvector v,
//                              double scale,
//                              int precision)
//{
//    vector(vector_name, v, scale, " ", precision);
//}

//void PrintUtils::vectorScaled(string vector_name,
//							  quaternion q,
//							  double scale,
//							  int precision)
//{
//	vector(vector_name, q, scale, " ", precision);
//}


//// overloaded function
//void PrintUtils::vector(string vector_name,
//                        double a,
//                        double b,
//                        double c,
//                        string units)
//{
//    cout << vector_name << "[" << a << ", " << b <<  ", " << c << "] " << units << endl;
//}

//// overloaded function with setprecision
//void PrintUtils::vector(string vector_name,
//                        double a,
//                        double b,
//                        double c,
//                        string units,
//                        int precision){
//    cout.precision(precision);
//    cout << vector_name << fixed << "[" << a << ", " << b <<  ", " << c << "] " << units << endl;
//}

//void PrintUtils::vector(string vector_name,
//                        double a,
//                        double b,
//                        double c,
//                        double d,
//                        string units)
//{
//    cout << vector_name << "[" << a << ", " << b <<  ", " << c << ", " << d << "] " << units << endl;
//}






void PrintUtils::vector2(string name_v1,
                         rvector v1,
                         string name_v2,
                         rvector v2)
{
    cout << name_v1 << ": " << v1 << " | " << name_v2 << ": " << v2 << endl;
}

void PrintUtils::vector2(string name_v1,
                         rvector v1,
                         string name_v2,
                         rvector v2,
                         int precision)
{
    cout.precision(precision);
    cout << name_v1 << fixed << "[" << v1.col[0] << ", " << v1.col[1] <<  ", " << v1.col[2] << "]" << " | " << name_v2 << "[" << v2.col[0] << ", " << v2.col[1] <<  ", " << v2.col[2] << "]" << endl;
}



void PrintUtils::vectorAndMag(string vector_name, rvector v)
{
    vectorAndMag(vector_name, v, "");
}

void PrintUtils::vectorAndMag(string vector_name, rvector v, string suffix)
{
    vector(vector_name, v, scale, "", precision, fieldwidth);

    double magnitude = length_rv(v)*scale;

    if (delimiter_flag)
    {
        cout << "M,";
    }
    else
    {
        cout << ",M,";
    }

    if (use_brackets)
    {
        cout << "[";
    }

    if (fieldwidth != -1)
    {
        cout << std::fixed;



        cout << std::setw(fieldwidth) << magnitude;


        cout << suffix;
    }
    else
    {
        cout << magnitude << suffix;
    }

    if (use_brackets)
    {
        cout << "]";
    }

    if (delimiter_flag)
    {
        cout << ",";
    }
    else
    {
        cout << "";
    }


}



// ----------------------------------------------
// Quaternion prints

void PrintUtils::quat(
        string text_prefix,
        quaternion q,
        double scale,
        string text_suffix,
        int precision,
        int fieldwidth = 6)
{

    if (precision != -1){
        // with set precision
        cout.precision(precision);
        cout << fixed;
    }
    //    std::cout.width(filedwidth);
    std::setw(fieldwidth);
    cout << text_prefix << "[[" << q.d.x*scale << "," << q.d.y*scale <<  "," << q.d.z*scale << "] " << q.w*scale << "]" << text_suffix;

}

void PrintUtils::quat(quaternion q)
{
    // this prints the quaternion q enclosed in brackets like this: [[x,y,z] w]
    //cout << q;
    quat("", q, 1., "", -1, 6);
}

void PrintUtils::quat(quaternion q, int precision)
{
    cout.precision(precision);
    // this prints the vector v enclosed in brackets like this: [x,y,z]
    cout << fixed << "[[" << q.d.x << ", " << q.d.y <<  ", " << q.d.z << "] " << q.w << "]";
}

void PrintUtils::quat(string vector_name, quaternion q)
{
    //cout << vector_name << ": " << q;
    quat(vector_name, q, 1, "", -1, -1);
}

void PrintUtils::quat(string vector_name, quaternion q, int precision)
{
    quat(vector_name, q, 1, "", precision, -1);
}

void PrintUtils::quat(string vector_name,
                      quaternion q,
                      string text_suffix,
                      int precision)
{
    quat(vector_name, q, 1, text_suffix, precision, -1);
}

void PrintUtils::endline()
{
    cout << endl;
}
