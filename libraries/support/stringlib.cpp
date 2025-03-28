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
#include "support/timelib.h"

//! \addtogroup stringlib_functions
//! @{

//! Parse a string
/*! Divide a string into substrings based on a delimeter and return a vector of the
 * results.
    \param in Zero terminated character string.
    \param delimeter String of delimeters.
    \param multi Flag indicating whether or not to count repeated separators as a
    single separator.
    \return vector of sub strings.
*/
vector < string > string_split(string in, string delimeters, bool multi) {
    vector<string> result;
    const char *str = in.data();
    do {
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
        if (begin != str)
        {
            result.push_back(string(begin, str));
        }
        else if (!multi)
        {
            result.push_back("");
        }
    } while (0 != *str++);
    return result;
}

//! \brief Find nth occurence
//! Return position of the nth occurence of desired string
//! \param input Input string to search
//! \param chars Characters to look for
//! \param num Last character to search
//! \return Position, or string::npos
size_t string_find(string input, string chars, uint16_t num)
{
    size_t pos = 0;
    for (uint16_t i=0; i<num; ++i)
    {
        size_t npos = input.find(chars);
        if (npos == string::npos || npos+1 >= input.size())
        {
            pos = string::npos;
            break;
        }
        else
        {
            pos += npos + 1;
            input = input.substr(npos + 1);
        }
    }
    if (pos == 0)
    {
        return string::npos;
    }
    else
    {
        return pos;
    }
}

//! \brief Find nth string in vector<>
//! Return index of the nth occurence of desired string in vector
//! \param input Input vector<string> to search
//! \param chars String to look for
//! \param num n
//! \return Index, or string::npos
size_t string_find(vector<string> input, string chars, uint16_t num)
{
    size_t count = 0;
    for (uint16_t i=0; i<input.size(); ++i)
    {
        size_t npos = input[i].find(chars);
        if (npos != string::npos)
        {
            if (++count == num)
            {
                break;
            }
        }
    }
    if (count < num)
    {
        return string::npos;
    }
    else
    {
        return count;
    }
}

//! \brief Join a vector of string together
//! Turn vector<string> into a single string, separated by delimeter.
//! \param in vector<string>
//! \param delimeter Separator to use. Defaults to " ".
//! \param first First element to use
//! \param last Last element to use
//! \return Newly created string.
string string_join(vector<std::string> &in, std::string delimeter, uint16_t first, uint16_t last)
{
    string out;
    if (in.size() > first)
    {
        if (last >= in.size())
        {
            last = in.size() - 1;
        }
        out = in[first];
        for (uint16_t i=first+1; i<=last; ++i)
        {
            out += delimeter + in[i];
        }
    }
    return out;
}

//! Parse a string into words
/*! Divide a string into words separated by white space and return an array of the
 * results.
    \param string Zero terminated character string.
    \param words Empty array for storage of substrings.
    \param wmax maximum number of words that can be stored in words array.
    \return Number of words
*/
uint16_t string_parse(char *string, char *words[], uint16_t wmax) {
    uint16_t wcount, ccount, i;
    if (string == NULL) return 0;

    wcount = ccount = i = 0;
    while (string[i] == ' ' || string[i] == '\t')	{
        i++;
	}

    if (string[i] == 0) return 0;

    while (string[i] != 0) {
        if (string[i] == ' ' || string[i] == '\t') {
            words[wcount][ccount] = 0;
            ccount = 0;
            wcount++;
            if (wcount == wmax-1) break;
            do {
                i++;
            } while (string[i] == ' ' || string[i] == '\t');
        } else {
            if (!ccount)
                words[wcount] = &string[i];
            ccount++;
            i++;
        }
    }
    if (ccount) wcount++;
    words[wcount] = (char *)nullptr;
    return (wcount);
}

/// Replace every occurance of a substring within a string with another subtring.
/** Search through a string for every instance of a specified substring, then replace. For internal use.
    @param	str		string to search through
    @param	from	substring to replace
    @param	to		substring to be replaced with
    @return	n/a
*/
string string_replace(string str, const string& from, const string& to)
{
    if(from.empty()) return str;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

int string_cmp(const char *wild, const char *string) {
    // Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
    const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*')) {
        if ((*wild != *string) && (*wild != '?')) { return 0; }
        wild++;
        string++;
    }

    while (*string) {
        if (*wild == '*') {
            if (!*++wild) { return 1; }
            mp = wild;
            cp = string+1;
        } else if ((*wild == *string) || (*wild == '?')) {
            wild++;
            string++;
        } else {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*') {
        wild++;
    }
    return !*wild;
}



// default constructor
StringParser::StringParser(string str) { splitString(str, ','); }

// overladed constructor
StringParser::StringParser(string str, char delimiter) { splitString(str,delimiter); }


//  splits the string into a vector field of strings
void StringParser::splitString(string str, char delimiter)
{
    stringstream ss(str);
    string token;

    while(std::getline(ss, token, delimiter)) {
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
string StringParser::getFieldNumber(uint32_t index) {
    string out;
    uint32_t real_offset = index + offset;

    if ( index>0 && numberOfFields >= (real_offset) && real_offset < vect.size() ){
        out = vect.at(real_offset);
    } else { // fail safe
        return "";
    }
    return out;
}


// JIMNOTE:  why is this code using exceptions?  should we have exceptions at all?
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

int StringParser::getFieldNumberAsInteger(uint32_t index) { return getFieldNumberAsDouble(index); }

/**
 * @brief Convert byte array to human-readable string
 *
 * @param buffer Pointer to byte array
 * @param size Length of array
 * @param ascii If true, converts alphanumeric and whitespace ascii values to equivalent ascii character
 * @param start Offset to start in buffer
 * @return Human-readable string
 */
string to_hex_string(uint8_t* buffer, uint16_t size, bool ascii, uint16_t start)
{
    vector<uint8_t> data;
    data.assign(buffer+start, buffer+start+size);
    return to_hex_string(data, ascii);
}

/**
 * @brief Convert byte vector to human-readable string
 * 
 * @param buffer Byte vector
 * @param ascii If true, converts alphanumeric and whitespace ascii values to equivalent ascii character
 * @param start Offset to start in buffer
 * @return Human-readable string
 */
string to_hex_string(const vector<uint8_t> &buffer, bool ascii, uint16_t start)
{
    std::stringstream ss;
    if (start >= buffer.size())
    {
        start = buffer.size() - 1;
    }
    for (uint16_t i=start; i<buffer.size(); ++i)
    {
        if (ascii && (ispunct(buffer[i]) || isalnum(buffer[i]) || isspace(buffer[i])))
        {
            ss << buffer[i];
        }
        else
        {
            ss << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(buffer[i]);
        }
    }
    return ss.str();
}

vector<uint8_t> from_hex_string(string hex)
{
    vector<uint8_t> bytes;
    if (hex.length() < 2)
    {
        return bytes;
    }
    if ((hex[1] == 'x' || hex[1] == 'X'))
    {
        for (uint16_t ib=1; ib<hex.length()/2; ++ib)
        {
            if (from_hex(hex[ib*2]) < 16)
            {
                bytes.push_back(from_hex(hex[ib*2]));
                if (from_hex(hex[ib*2+1]) < 16)
                {
                    bytes[ib-1] *= 16;
                    bytes[ib-1] += from_hex(hex[ib*2+1]);
                }
                else
                {
                    return bytes;
                }
            }
            else
            {
                return bytes;
            }
        }
    }
    else
    {
        for (uint16_t ib=0; ib<hex.length()/2; ++ib)
        {
            if (from_hex(hex[ib*2]) < 16)
            {
                bytes.push_back(from_hex(hex[ib*2]));
                if (from_hex(hex[ib*2+1]) < 16)
                {
                    bytes[ib] *= 16;
                    bytes[ib] += from_hex(hex[ib*2+1]);
                }
                else
                {
                    return bytes;
                }
            }
            else
            {
                return bytes;
            }
        }
    }
    return bytes;
}

vector<uint8_t> from_hex_vector(vector<uint8_t>& hex)
{
    vector<uint8_t> bytes;
    for (uint16_t ib=0; ib<hex.size()/2; ++ib)
    {
        if (from_hex(hex[ib*2]) < 16)
        {
            bytes.push_back(from_hex(hex[ib*2]));
            if (from_hex(hex[ib*2+1]) < 16)
            {
                bytes[ib] *= 16;
                bytes[ib] += from_hex(hex[ib*2+1]);
            }
            else
            {
                return bytes;
            }
        }
        else
        {
            return bytes;
        }
    }
    return bytes;
}

string to_astring(vector<uint8_t> buf, bool hex)
{
    return to_astring((char *)buf.data(), buf.size(), hex);
}

string to_astring(string buf, bool hex)
{
    return to_astring((char *)buf.c_str(), buf.length(), hex);
}

string to_astring(char *value, size_t length, bool hex)
{
    string output;
    if (hex)
    {
        for (size_t i=0; i<length; ++i)
        {
            if (value[i] < 32 || value[i] > 126)
            {
                output += '(' + to_hex(value[i], 2, true) + ')';
            }
            else
            {
                output.push_back(value[i]);
            }
        }
    }
    else
    {
        output = value;
    }
    return output;
}

string byte_vector_to_string(const vector<uint8_t> &buf, uint16_t offset)
{
    string output(buf.begin()+offset, buf.end());
    return output;
}

vector<uint8_t> string_to_byte_vector(const string &buf)
{
    vector<uint8_t> output(buf.begin(), buf.end());
    return output;
}

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
string to_binary(uint64_t value, uint16_t digits, bool zerofill)
#else
string to_binary(size_t value, uint16_t digits, bool zerofill)
#endif
{
    string output="0b";
    if (digits > 66)
    {
        digits = 66;
    }
    output.resize(digits);
    uint8_t bits = 0;
    if (value > 0)
    {
        bits = log2(value) + 1;
    }

    if (zerofill)
    {
        if (digits > bits)
        {
            for (uint8_t i=0; i<digits-bits; ++i)
            {
                output += "0";
            }
        }
    }
    else
    {
        for (uint8_t i=bits-1; i<bits; --i)
        {
            if (value & 1<<i)
            {
                output += "1";
            }
            else
            {
                output += "0";
            }
        }
    }
    if (digits)
    {
        output.resize(digits);
    }
    else
    {
        output.resize(strlen(output.c_str()));
    }
    return output;
}

uint8_t from_hex(char value)
{
    if (value >= '0' && value <= '9')
    {
        return value - '0';
    }
    else if (value >= 'a' && value <= 'f')
    {
        return (value - 'a') + 10;
    }
    else if (value >= 'A' && value <= 'F')
    {
        return (value - 'A') + 10;
    }
    else
    {
        return 16;
    }
}

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
uint64_t from_hex(string value)
{
uint64_t result = 0;
#else
size_t from_hex(string value)
{
size_t result;
#endif
    for (uint16_t i=0; i<value.size(); ++i)
    {
        if (from_hex(value[i] < 16))
        {
            result <<= 4;
            result += from_hex(value[i]);
        }
        else
        {
            return result;
        }
    }
    return result;
}

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
string to_hex(uint64_t value, uint16_t digits, bool zerofill)
#else
string to_hex(size_t value, uint16_t digits, bool zerofill)
#endif
{
    string output="";
    output.resize(digits>17?digits:17);
    if (zerofill) {
        if (digits) {
            sprintf(&output[0], "%0*lx", digits, value);
        } else {
            sprintf(&output[0], "%0lx", value);
        }
    } else {
        if (digits) {
            sprintf(&output[0], "%*lx", digits, value);
        } else {
            sprintf(&output[0], "%lx", value);
        }
    }
    if (digits)
    {
        output.resize(digits);
    }
    else
    {
        output.resize(strlen(output.c_str()));
    }
    return output;
}

#if ((PTRDIFF_WIDTH) == (INT64_WIDTH))
string to_signed(int64_t value, uint16_t digits, bool zerofill)
#else
string to_signed(int32_t value, uint16_t digits, bool zerofill)
#endif
{
    string output="";
    output.resize(digits>20?digits:20);
    if (zerofill) {
        if (digits) {
            sprintf(&output[0], "%+0*ld", digits, value);
        } else {
            sprintf(&output[0], "%+0ld", value);
        }
    } else {
        if (digits) {
            sprintf(&output[0], "%+*ld", digits, value);
        } else {
            sprintf(&output[0], "%+ld", value);
        }
    }
    if (digits)
    {
        output.resize(digits);
    }
    else
    {
        output.resize(strlen(output.c_str()));
    }
    return output;
}

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
string to_unsigned(uint64_t value, uint16_t digits, bool zerofill)
#else
string to_unsigned(uint32_t value, uint16_t digits, bool zerofill)
#endif
{
    string output="";
    output.resize(digits>20?digits:20);
    if (zerofill) {
        if (digits) {
            sprintf(&output[0], "%0*lu", digits, value);
        } else {
            sprintf(&output[0], "%0lu", value);
        }
    } else {
        if (digits) {
            sprintf(&output[0], "%*lu", digits, value);
        } else {
            sprintf(&output[0], "%lu", value);
        }
    }
    if (digits)
    {
        output.resize(digits);
    }
    else
    {
        output.resize(strlen(output.c_str()));
    }
    return output;
}

string to_fixed(double value, uint16_t digits, uint16_t precision, bool zerofill) {
    string output="";
    output.resize(digits+2);
    if (zerofill)
    {
        if (precision) {
            sprintf(&output[0], "%0*.*f", digits, precision, static_cast<double>(value));
        } else {
            sprintf(&output[0], "%0*f", digits, static_cast<double>(value));
        }
    }
    else
    {
        if (precision) {
            sprintf(&output[0], "%*.*f", digits, precision, static_cast<double>(value));
        } else {
            sprintf(&output[0], "%*f", digits, static_cast<double>(value));
        }
    }

    output.resize(strlen(&output[0]));
    return output;
}

string to_floating(float value, int16_t precision) {
    string output="                 ";
    if (precision >= 0) {
    output.resize(17+precision);
        sprintf(&output[0], "%0.*f", precision, static_cast<double>(value));
    } else {
        sprintf(&output[0], "%f", static_cast<double>(value));
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_floating(double value, int16_t precision)
{
    uint16_t digits = abs(log10(value));
    string output="";
    if (precision >= 0) {
        output.resize(digits+17+precision);
        sprintf(&output[0], "%0.*f", precision, value);
            } else {
        output.resize(digits+17);
        sprintf(&output[0], "%f", value);
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_floatexp(float value, int16_t precision) {
    string output="                 ";
    if (precision >= 0) {
        output.resize(17+precision);
        sprintf(&output[0], "%0.*e", precision, static_cast<double>(value));
    } else {
        sprintf(&output[0], "%e", static_cast<double>(value));
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_floatexp(double value, int16_t precision) {
    string output="";
    if (precision < 0)
    {
        precision = 8;
    }
    output.resize(17+precision);
    sprintf(&output[0], "%0.*e", precision, value);
    output.resize(strlen(&output[0]));
    return output;
}

string to_floatany(float value, int16_t precision) {
    string output="                 ";
    if (precision >= 0) {
        output.resize(17+precision);
        sprintf(&output[0], "%0.*g", precision, static_cast<double>(value));
    } else {
        sprintf(&output[0], "%.*g", std::numeric_limits<float>::digits10, static_cast<double>(value));
    }
    output.resize(strlen(&output[0]));
    return output;
}

string to_floatany(double value, int16_t precision) {
    string output="";
    if (!precision)
    {
        precision = std::numeric_limits<double>::digits10;
    }
    output.resize(17+precision);
    sprintf(&output[0], "%0.*g", precision, value);
    output.resize(strlen(&output[0]));
    return output;
}

string to_mjd(double value)
{
    return to_floatany(value, 13);
}

string to_temperature(double value, char units, uint8_t precision)
{
    switch (units) {
    case 'K':
        return to_floatany(value, precision) + " K";
    case 'C':
        return to_floatany(value - 273.15, precision) + " C";
    case 'F':
        return to_floatany((value - 273.15) / 1.8 + 32., precision) + " F";
    }
    return "";
}

string to_angle(double value, char units, uint8_t precision) {
    switch (units)
    {
    case 'r':
    case 'R':
        return to_floatany(value, precision);
    case 'd':
        return to_floatany(DEGOF(value), precision);
    case 'D':
        return to_floatany(DEGOF(value), precision) + 'D';
    case 'a':
        return to_floatany(value / DAS2R, precision);
    case 'A':
        return to_floatany(value / DAS2R, precision) + '\'';
    }
    return "";
}

string to_bool(bool value, char type)
{
    string output;
    switch (type)
    {
    case 'y':
        output = value?"y":"n";
        break;
    case 'Y':
        output = value?"Y":"N";
        break;
    case 't':
        output = value?"t":"f";
        break;
    case 'T':
        output = value?"T":"F";
        break;
    case '1':
        output = value?"1":"0";
        break;
    }
    return output;
}

string to_iso8601(double value)
{
    return utc2iso8601(value);
}

string to_unixtime(double value, uint8_t precision)
{
    return to_floating(86400. * (value - 40587.), precision);
}

string to_datename(double mjd)
{
    calstruc cal = mjd2cal(mjd);
    string output = to_unsigned(cal.year, 4);
    output += to_unsigned(cal.month, 2, true);
    output += to_unsigned(cal.dom, 2, true);
    output += "_" + to_unsigned(cal.hour, 2, true);
    output += to_unsigned(cal.minute, 2, true);
    output += to_unsigned(cal.second, 2, true);
    return output;
}

string to_json(string key, string value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, double value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

#if ((PTRDIFF_WIDTH) == (INT64_WIDTH))
string to_json(string key, int64_t value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, int32_t value) {
    return to_json(key, static_cast<int64_t>(value));
}

string to_json(string key, int16_t value) {
    return to_json(key, static_cast<int64_t>(value));
}

string to_json(string key, int8_t value) {
    return to_json(key, static_cast<int64_t>(value));
}
#else
string to_json(string key, int32_t value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, int16_t value) {
    return to_json(key, static_cast<int32_t>(value));
}

string to_json(string key, int8_t value) {
    return to_json(key, static_cast<int32_t>(value));
}
#endif

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
string to_json(string key, uint64_t value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, uint32_t value) { return to_json(key, static_cast<uint64_t>(value)); }

string to_json(string key, uint16_t value) { return to_json(key, static_cast<uint64_t>(value)); }

string to_json(string key, uint8_t value) { return to_json(key, static_cast<uint64_t>(value)); }
#else
string to_json(string key, uint32_t value) {
    JSONObject jobject;
    jobject.addElement(key, value);
    return jobject.to_json_object();
}

string to_json(string key, uint16_t value) { return to_json(key, static_cast<uint32_t>(value)); }

string to_json(string key, uint8_t value) { return to_json(key, static_cast<uint32_t>(value)); }

#endif

string to_label(string label, double value, uint16_t precision, bool mjd)
{
    if (mjd)
    {
        return label + ": " + to_mjd(value);
    }
    else
    {
        if (fabs(value) >= pow(10., 13-precision) || fabs(value) < pow(10., -precision))
        {
            return label + ": " + to_floatexp(value, precision);
        }
        else
        {
            return label + ": " + to_floating(value, precision);
        }
    }
}

string to_label(string label, float value, uint16_t precision, bool mjd)
{
    if (mjd)
    {
        return label + ": " + to_mjd(value);
    }
    else
    {
        if (fabs(value) >= pow(10., 7-precision) || fabs(value) < pow(10., -precision))
        {
            return label + ": " + to_floatexp(value, precision);
        }
        else
        {
            return label + ": " + to_floating(value, precision);
        }
    }
}

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
string to_label(string label, uint64_t value, uint16_t digits, bool hex) {
    if (hex) {
        return label + ": " + to_hex(value, digits);
    } else {
        return label + ": " + to_unsigned(value, digits);
    }
}

string to_label(string label, uint32_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}

string to_label(string label, uint16_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}

string to_label(string label, uint8_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<uint64_t>(value), digits, hex);
}
#else
string to_label(string label, uint32_t value, uint16_t digits, bool hex) {
    if (hex) {
        return label + ": " + to_hex(value, digits);
    } else {
        return label + ": " + to_unsigned(value, digits);
    }
}

string to_label(string label, uint16_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<uint32_t>(value), digits, hex);
}

string to_label(string label, uint8_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<uint32_t>(value), digits, hex);
}
#endif

#if ((PTRDIFF_WIDTH) == (INT64_WIDTH))
string to_label(string label, int64_t value, uint16_t digits, bool hex) {
    if (hex) {
        return label + ": " + to_hex(value, digits);
    } else {
        return label + ": " + to_signed(value, digits);
    }
}

string to_label(string label, int32_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}

string to_label(string label, int16_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}

string to_label(string label, int8_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<int64_t>(value), digits, hex);
}
#else
string to_label(string label, int32_t value, uint16_t digits, bool hex) {
    if (hex) {
        return label + ": " + to_hex(value, digits);
    } else {
        return label + ": " + to_signed(value, digits);
    }
}

string to_label(string label, int16_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<int32_t>(value), digits, hex);
}

string to_label(string label, int8_t value, uint16_t digits, bool hex) {
    return to_label(label, static_cast<int32_t>(value), digits, hex);
}
#endif

string to_label(string label, bool value) {
    return label + ": " + to_bool(value);
}

string to_label(string label, string value) {
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
//            printf("%c", value[i]);
        }
//            else
//        {
//            printf(" [0] ");
//        }
    }
//    printf("\n");
    output.push_back(0);
    return output;
}

uint64_t to_uint64(string svalue)
{
    return to_uint64(svalue.data(), svalue.length());
}

uint64_t to_uint64(const char* svalue, uint16_t digits)
{
    uint64_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

uint32_t to_uint32(string svalue)
{
    return to_uint32(svalue.data(), svalue.length());
}

uint32_t to_uint32(const char* svalue, uint16_t digits)
{
    uint32_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

uint16_t to_uint16(string svalue)
{
    return to_uint16(svalue.data(), svalue.length());
}

uint16_t to_uint16(const char* svalue, uint16_t digits)
{
    uint16_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

uint8_t to_uint8(string svalue)
{
    return to_uint8(svalue.data(), svalue.length());
}

uint8_t to_uint8(const char* svalue, uint16_t digits)
{
    uint8_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

int64_t to_int64(string svalue)
{
    return to_int64(svalue.data(), svalue.length());
}

int64_t to_int64(const char* svalue, uint16_t digits)
{
    int64_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

int32_t to_int32(string svalue)
{
    return to_int32(svalue.data(), svalue.length());
}

int32_t to_int32(const char* svalue, uint16_t digits)
{
    int32_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

int16_t to_int16(string svalue)
{
    return to_int16(svalue.data(), svalue.length());
}

int16_t to_int16(const char* svalue, uint16_t digits)
{
    int16_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

int8_t to_int8(string svalue)
{
    return to_int8(svalue.data(), svalue.length());
}

int8_t to_int8(const char* svalue, uint16_t digits)
{
    int8_t nvalue = 0;
    bool negative = false;
    for (size_t i=0; i<digits; ++i)
    {
        if (svalue[i] == '-')
        {
            negative = true;
        }
        else if (svalue[i] >= '0' && svalue[i] <= '9')
        {
            nvalue = 10 * nvalue + (svalue[i] - '0');
        }
    }
    if (negative)
    {
        nvalue = -nvalue;
    }
    return nvalue;
}

double_t to_double(string svalue)
{
    return stod(svalue);
}

double_t to_double(const char *svalue, uint16_t digits)
{
    return to_double(string(svalue, digits));
}

float_t to_float(string svalue)
{
    return stof(svalue);
}

float_t to_float(const char *svalue, uint16_t digits)
{
    return to_float(string(svalue, digits));
}

//! @}
