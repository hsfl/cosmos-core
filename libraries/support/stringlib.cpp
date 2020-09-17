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

/*! \file stringlib.cpp
    \brief String handling library source file
*/

#include "support/stringlib.h"
#include "math/mathlib.h"

//! \addtogroup stringlib_functions
//! @{

//! Parse a string
/*! Divide a string into substrings based on a delimeter and return a vector of the
 * results.
    \param in Zero terminated character string.
    \param delimeter string of delimeters.
    \return vector of sub strings.
*/
vector < string > string_split(string in, string delimeters)
{
    vector<string> result;
    const char *str = in.data();

    do
    {
        const char *begin = str;

        while(*str)
        {
            bool match = false;

            for (size_t i=0; i<delimeters.size(); ++i)
            {
                if (*str == delimeters[i])
                {
                    match = true;
                    break;
                }
            }
            if (match)
            {
                break;
            }
            str++;
        }
        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

//! Parse a string into words
/*! Divide a string into words separated by white space and return an array of the
 * results.
    \param string Zero terminated character string.
    \param words Empty array for storage of substrings.
    \param wmax maximum number of words that can be stored in words array.
    \return Number of words
*/
uint16_t string_parse(char *string, char *words[], uint16_t wmax)
{
    uint16_t wcount, ccount, i;

    if (string == NULL)
        return 0;

    wcount = ccount = i = 0;
    while (string[i] == ' ' || string[i] == '\t')
        i++;

    if (string[i] == 0)
        return 0;

    while (string[i] != 0)
    {
        if (string[i] == ' ' || string[i] == '\t')
        {
            words[wcount][ccount] = 0;
            ccount = 0;
            wcount++;
            if (wcount == wmax-1)
                break;
            do
            {
                i++;
            } while (string[i] == ' ' || string[i] == '\t');
        }
        else
        {
            if (!ccount)
                words[wcount] = &string[i];
            ccount++;
            i++;
        }
    }
    if (ccount)
        wcount++;
    words[wcount] = (char *)nullptr;
    return (wcount);
}

int string_cmp(const char *wild, const char *string)
{
    // Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
    const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string+1;
        } else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        } else {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}



// default constructor
StringParser::StringParser(string str)
{
    splitString(str, ',');
}

// overladed constructor
StringParser::StringParser(string str, char delimiter)
{
    splitString(str,delimiter);
}


//  splits the string into a vector field of strings
void StringParser::splitString(string str, char delimiter)
{
    std::stringstream ss(str);
    string token;

    while(std::getline(ss, token, delimiter))
    {
        //std::cout << token << '\n';
        vect.push_back(token);
    }

    numberOfFields = vect.size();

    // default offset so that when we want to get the 1st entry we just to getFieldNumber(1)
    offset = -1;
}

// this function gets the field number of the comma delimited string
// Ex: string = "SOL_COMPUTED,NARROW_INT,-1634531.5683"
// StringParser::getFieldNumber(1) => "SOL_COMPUTED"
// StringParser::getFieldNumber(0) => NULL (does not return a meaning value)
string StringParser::getFieldNumber(uint32_t index)
{
    string out;
    uint32_t real_offset = index + offset;

    if ( index>0 && numberOfFields >= (real_offset) && real_offset < vect.size() ){

        out = vect.at(real_offset);
        //std::cout <<out << std::endl;
    } else { // fail safe
        return "";
    }

    return out;
}


double StringParser::getFieldNumberAsDouble(uint32_t index)
{
    double out;
    if ( (index > 0) && (numberOfFields >= index)){

        try {
            out = stod(vect.at(index + offset));
            //std::cout <<out << std::endl;
        } catch (const std::exception& e){
            std::cout << "error parsing string:" << e.what() << std::endl;
            return 0;
        }
    } else { // fail safe
        return 0;
    }

    return out;
}

int StringParser::getFieldNumberAsInteger(uint32_t index)
{
    return getFieldNumberAsDouble(index);
}

string to_string(char *value)
{
    string output = value;
    return output;
}

string to_hex(int64_t value, uint16_t digits, bool zerofill)
{
    string output="";
    output.resize(digits?digits+2:20);
    if (zerofill)
    {
        if (digits)
        {
            sprintf(&output[0], "%0*lx", digits, value);
        }
        else
        {
            sprintf(&output[0], "%0lx", value);
        }
    }
    else
    {
        if (digits)
        {
            sprintf(&output[0], "%*lx", digits, value);
        }
        else
        {
            sprintf(&output[0], "%lx", value);
        }
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_signed(int64_t value, uint16_t digits, bool zerofill)
{
    string output="";
    output.resize((value==0?0:size_t(log10(std::abs(value))))+digits+5);
    if (zerofill)
    {
        if (digits)
        {
            sprintf(&output[0], "%0*ld", digits, value);
        }
        else
        {
            sprintf(&output[0], "%0ld", value);
        }
    }
    else
    {
        if (digits)
        {
            sprintf(&output[0], "%*ld", digits, value);
        }
        else
        {
            sprintf(&output[0], "%ld", value);
        }
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_unsigned(uint64_t value, uint16_t digits, bool zerofill)
{
    string output="";
    output.resize((value==0?0:size_t(log10((value))))+digits+5);
    if (zerofill)
    {
        if (digits)
        {
            sprintf(&output[0], "%0*lu", digits, value);
        }
        else
        {
            sprintf(&output[0], "%0lu", value);
        }
    }
    else
    {
        if (digits)
        {
            sprintf(&output[0], "%*lu", digits, value);
        }
        else
        {
            sprintf(&output[0], "%lu", value);
        }
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_double(double value, uint16_t precision)
{
    string output="";
//    output.resize((value==0.?0:size_t(log10(std::abs(value))))+precision+5);
    output.resize(17+precision);
    if (precision)
    {
        sprintf(&output[0], "%.*g", precision, value);
    }
    else
    {
        sprintf(&output[0], "%g", value);
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_mjd(double value)
{
    return to_double(value, 17);
}

string to_temperature(double value, char units, uint8_t precision)
{
    switch (units)
    {
    case 'K':
        return to_double(value, precision) + " K";
    case 'C':
        return to_double(value - 273.15, precision) + " C";
    case 'F':
        return to_double((value - 273.15) / 1.8 + 32., precision) + " F";
    }
    return "";
}

string to_angle(double value, char units, uint8_t precision)
{
    switch (units)
    {
    case 'R':
        return to_double(value, precision);
    case 'D':
        return to_double(DEGOF(value), precision) + 'D';
    case 'A':
        return to_double(value / DAS2R, precision) + '\'';
    }
    return "";
}

string to_bool(bool value)
{
    string output="";
    output.resize(2);
    output[0] =  value?'1':'0';
    output.resize(strlen(&output[0]));
    return output;
}

string to_json(string key, string value)
{
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, double value)
{
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, int64_t value)
{
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, int32_t value)
{
    return to_json(key, static_cast<int64_t>(value));
}

string to_json(string key, int16_t value)
{
    return to_json(key, static_cast<int64_t>(value));
}

string to_json(string key, int8_t value)
{
    return to_json(key, static_cast<int64_t>(value));
}

string to_json(string key, uint64_t value)
{
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, uint32_t value)
{
    return to_json(key, static_cast<uint64_t>(value));
}

string to_json(string key, uint16_t value)
{
    return to_json(key, static_cast<uint64_t>(value));
}

string to_json(string key, uint8_t value)
{
    return to_json(key, static_cast<uint64_t>(value));
}

string to_label(string label, double value, uint16_t precision, bool mjd)
{
    if (mjd)
    {
        return label + ": " + to_mjd(value);
    }
    else
    {
        return label + ": " + to_double(value, precision);
    }
}

string to_label(string label, uint64_t value, uint16_t digits, bool hex)
{
    if (hex)
    {
        return label + ": " + to_hex(value, digits);
    }
    else
    {
        return label + ": " + to_unsigned(value, digits);
    }
}

string to_label(string label, uint32_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}

string to_label(string label, uint16_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}

string to_label(string label, uint8_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}

string to_label(string label, int64_t value, uint16_t digits, bool hex)
{
    if (hex)
    {
        return label + ": " + to_hex(value, digits);
    }
    else
    {
        return label + ": " + to_signed(value, digits);
    }
}

string to_label(string label, int32_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}

string to_label(string label, int16_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}

string to_label(string label, int8_t value, uint16_t digits, bool hex)
{
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}

string to_label(string label, bool value)
{
    return label + ": " + to_bool(value);
}

string to_label(string label, string value)
{
    return label + ": " + (value);
}

string clean_string(string value)
{
    string output;
    for (uint16_t i=0; i<value.length(); ++i)
    {
        if (value[i] != 0)
        {
            output.push_back(value[i]);
            printf("%c", value[i]);
        }
        else {
            printf(" [0] ");
        }
    }
    printf("\n");
    output.push_back(0);
    return output;
}

//! @}
