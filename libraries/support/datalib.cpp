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

/*! \file datalib.cpp
    \brief Data support functions
*/

#include "support/datalib.h"
//#include "support/jsonlib.h"
//#include "support/jsondef.h"
#include "support/stringlib.h"
#include <algorithm>

//! \ingroup datalib
//! \defgroup datalib_statics Static variables for Data functions.
//! @{

//static vector<cosmosstruc> nodes;

//! Path to COSMOS root directory
static string cosmosroot;
//! Path to COSMOS Nodes directory
static string cosmosnodes="";
//! Path to COSMOS Resources directory
static string cosmosresources;
//! Path to current COSMOS Node directory
static string nodedir;

//! @}

// MSVC lacks these POSIX macros and other compilers may too:
#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

//! \ingroup datalib
//! \defgroup datalib_functions Data Management function declarations
//! @{

//! Construct DataLog
//! Class for logging regular entries
DataLog::DataLog()
{
}

//! \brief Initialize ::DataLog
//! Preset DataLog to be used for provided conditions
int32_t DataLog::Init(std::string node, std::string location, std::string agent, std::string type, std::string extra, double stride, bool fastmode, bool compress)
{
    fout = nullptr;
    this->node = node;
    this->location = location;
    this->agent = agent;
    this->type = type;
    this->extra = extra;
    this->fastmode = fastmode;
    this->compress = compress;
    if (stride < 0.)
    {
        stride = 0.;
    }
    this->stride = stride / 86400.;
    return 0;
}

//! Change DataLog stride
//! Change how many seconds between moving from temp to outgoing and compressing
//! \param seconds Number of seconds in stride.
int32_t DataLog::SetStride(double seconds)
{
    if (seconds < 0.)
    {
        seconds = 0.;
    }
    stride = seconds;
    startdate = currentmjd();
    enddate = startdate + stride / 86400.;
    return seconds;
}

//! Change DataLog fastmode
//! Change whether file is kept open (fast), or closed each time
//! \param state fastmode true or false.
int32_t DataLog::SetFastmode(bool state)
{
    fastmode = state;
    if (!fastmode)
    {
        if (fout != nullptr)
        {
            fclose(fout);
            fout = nullptr;
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

//! Change Starting Date
//! Resets internal paramaters to start logging from new starting date.
//! \param mjd UTC in Modified Julian Day
int32_t DataLog::SetStartdate(double mjd)
{
    if (mjd <= 0.)
    {
        mjd = currentmjd();
    }
    startdate = mjd;
    enddate = mjd;
    return startdate;
}

//! \brief Write DataLog
//! Append binary data to file described by ::DataLog.
//! \return Zero or negative error.
int32_t DataLog::Write(string& data)
{
    vector<uint8_t> bdata;
    bdata.insert(bdata.begin(), data.begin(), data.end());
    return Write(bdata);
}

//! \brief Write DataLog
//! Append binary data to file described by ::DataLog.
//! \return Zero or negative error.
int32_t DataLog::Write(vector<uint8_t>& data)
{
    int32_t iretn = 0;
    if (currentmjd() >= enddate)
    {
        startdate = enddate;
        enddate += stride;
        if (fout != nullptr)
        {
            fclose(fout);
            fout = nullptr;
        }
        if (!path.empty() && !location.empty() && path.find("/temp/") != string::npos)
        {
            string movepath = string_replace(path, "/temp/", "/" + location + "/");
            iretn = log_move_file(path, movepath, compress);
        }
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (fout == nullptr)
    {
        path = data_type_path(node, "temp", agent, startdate, type, extra);
        if (path.empty())
        {
            return GENERAL_ERROR_EMPTY;
        }
        fout = data_open(path, const_cast<char *>("a+"));
        if (fout == nullptr)
        {
            return GENERAL_ERROR_BAD_FD;
        }
    }
    iretn = fwrite(data.data(), data.size(), 1, fout);
    if (!fastmode)
    {
        fclose(fout);
        fout = nullptr;
    }
    return iretn;
}

//! Write log entry - full
//! Append the provided string to a file in the {node}/{location}/{agent} directory. The file name
//! is created as {node}_yyyyjjjsssss_{extra}.{type}
//! \param data Data to be written.
//! \param node Node name.
//! \param location Location, subfolder of node.
//! \param agent Agent name, subfolder of location.
//! \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
//! \param type Type part of name.
//! \param extra Extra part  of name.
int32_t DataLog::Write(vector<uint8_t> data, string node, string location, string agent, string type, string extra)
{
    int32_t iretn = 0;
    if (currentmjd() >= enddate)
    {
        startdate = enddate;
        enddate += stride;
        if (fout != nullptr)
        {
            fclose(fout);
            fout = nullptr;
        }
        if (!path.empty() && !location.empty() && path.find("/temp/") != string::npos)
        {
            string movepath = string_replace(path, "/temp/", "/" + location + "/");
            iretn = log_move_file(path, movepath, compress);
        }
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (fout == nullptr)
    {
        path = data_type_path(node, "temp", agent, startdate, type, extra);
        if (path.empty())
        {
            return GENERAL_ERROR_EMPTY;
        }
        fout = data_open(path, const_cast<char *>("a+"));
        if (fout == nullptr)
        {
            return GENERAL_ERROR_BAD_FD;
        }
    }
    iretn = fwrite(data.data(), data.size(), 1, fout);
    if (!fastmode)
    {
        fclose(fout);
        fout = nullptr;
    }
    return iretn;
}

//! If stridetime has elapsed since last write, then close the file
//! Append the provided string to a file in the {node}/{location}/{agent} directory. The file name
//! is created as {node}_yyyyjjjsssss_{extra}.{type}
//! \return 0 on success, negative on error
int32_t DataLog::CloseIfStrideTime()
{
    if (fout != nullptr && currentmjd() >= enddate)
    {
        startdate = enddate;
        enddate += stride;
        fclose(fout);
        fout = nullptr;
        if (!path.empty() && !location.empty() && path.find("/temp/") != string::npos)
        {
            string movepath = string_replace(path, "/temp/", "/" + location + "/");
            return log_move_file(path, movepath, compress);
        }
        return COSMOS_GENERAL_ERROR_NAME;
    }
    return COSMOS_GENERAL_ERROR_NOTREADY;
}

//! Write log entry - full
/*! Append the provided string to a file in the {node}/{location}/{agent} directory. The file name
 * is created as {node}_yyyyjjjsssss_{extra}.{type}
 * \param node Node name.
 * \param agent Agent name.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param extra Extra part  of name.
 * \param type Type part of name.
 * \param record String to be appended to file.
 * \param location Location name.
 */
string log_write(string node, string agent, double utc, string extra, string type, string record, string location)
{
    FILE *fout;
    string path;

    if (utc == 0.)
        return "";

    //    if (extra.empty())
    //    {
    //        path = data_type_path(node, location, agent, utc, type);
    //    }
    //    else
    //    {
    //        path = data_type_path(node, location, agent, utc, extra, type);
    //    }
    path = data_type_path(node, location, agent, utc, type, extra);

    if (location == "immediate")
    {
        fout = data_open(path, const_cast<char *>("w"));
    }
    else
    {
        fout = data_open(path, const_cast<char *>("a+"));
    }
    if (fout  != nullptr)
    {
        fprintf(fout,"%s\n",record.c_str());
        fclose(fout);
    }
    return path;
}

//! Write log entry - fixed location, no extra
/*! Append the provided string to a file in the {node}/temp/{agent} directory. The file name
 * is created as {node}_yyyyjjjsssss.{type}
 * \param node Node name.
 * \param agent Agent name.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param type Type part of name.
 * \param record String to be appended to file.
 */
string log_write(string node, string agent, double utc, string type, const char *record)
{
    return log_write(node, agent, utc, "", type, record);
}

//! Write log entry - fixed location, no extra, integer type and agent
/*! Append the provided string to a file in the {node}/temp/{agent_name} directory. The file name
 * is created as {node}_yyyyjjjsssss.{type_name}
 * \param node Node name.
 * \param type Integer specifying what type and agent of file.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param record String to be appended to file.
 */
string log_write(string node, int type, double utc, const char *record, string directory)
{

    switch (type)
    {
    case DATA_LOG_TYPE_SOH:
        return log_write(node, "soh", utc, "", "telemetry", record);
    case DATA_LOG_TYPE_EVENT:
        return log_write(node, "soh", utc, "", "event", record);
    case DATA_LOG_TYPE_BEACON:
        return log_write(node, "beacon", utc, "", "beacon", record);
    default:
        return log_write(node, "soh", utc, "", "log", record);
    }

}

//! Move log file - path version.
/*! Move a single file previously created with ::log_write to a final location, optionally
 * compressing with gzip.
 * \param oldpath Path to move from.
 * \param newpath Path to move to.
 * \param compress Wether or not to compress with gzip.
 * \return 0 on success, negative on error.
 */
int32_t log_move_file(string oldpath, string newpath, bool compress)
{
    int32_t iretn = 0;
    if (compress && oldpath.find(".gz") == string::npos)
    {
        char buffer[8192];
        string temppath = oldpath + ".gz";
        newpath += ".gz";
        FILE *fin = data_open(oldpath, "rb");
        if(fin == nullptr)
        {
            return GENERAL_ERROR_OPEN;
        }
        fseek(fin, 0, SEEK_END); // seek to end of file
        int64_t size = ftell(fin); // get current file pointer
        fseek(fin, 0, SEEK_SET); // seek back to beginning of file
        if(size <= 0)
        {
            //close the file
            fclose(fin);
            //remove the file
            remove(oldpath.c_str());
            //exit log_move_file
            return GENERAL_ERROR_BAD_SIZE;
        }
        FILE *fout = data_open(temppath, "wb");
        if(fout == nullptr)
        {
            return GENERAL_ERROR_OPEN;
        }
        gzFile gzfout;
        gzfout = gzdopen(fileno(fout), "a");

        do
        {
            unsigned nbytes = (unsigned)fread(buffer, 1, 8192, fin);
            if (nbytes)
            {
                gzwrite(gzfout, buffer, nbytes);
            }
        } while (!feof(fin));

        fclose(fin);
        gzclose_w(gzfout);
        fclose(fout);
        fout = data_open(newpath, "wb");
        if(fout == nullptr)
        {
            iretn = remove(temppath.c_str());
            return GENERAL_ERROR_OPEN;
        }
        fclose(fout);
        iretn = rename(temppath.c_str(), newpath.c_str());
        if (iretn < 0)
        {
            iretn = -errno;
            return iretn;
        }
        iretn = remove(oldpath.c_str());
        if (iretn < 0)
        {
            iretn = -errno;
        }
    }
    else
    {
        iretn = rename(oldpath.c_str(), newpath.c_str());
        if (iretn < 0)
        {
            iretn = -errno;
        }
    }
    return iretn;
}

//! Move log file - full version.
/*! Move files previously created with ::log_write to their final location, optionally
 * compressing with gzip. The full version allows for specification of the source and
 * destination locations, and whether compression should be used. The routine will find
 * all files currently in {node}/{srclocation}/{agent} and move them to {node}/{dstlocation}/{agent}.
 * \param node Node name.
 * \param agent Agent name.
 * \param srclocation Source location name.
 * \param dstlocation Destination location name.
 * \param compress Wether or not to compress with gzip.
 * \param age File must be this many seconds or older to move.
 */
int32_t log_move_agent_src(string node, string agent, string srclocation, string dstlocation, bool compress, float age)
{
    int32_t iretn = 0;
    vector<filestruc> oldfiles;
    iretn = data_list_files(node, srclocation, agent, oldfiles);
    for (auto oldfile: oldfiles)
    {
        if (86400.*(currentmjd()-oldfile.utc) > age)
        {
            iretn = log_move_file(oldfile.path, data_base_path(node, dstlocation, agent, oldfile.name), compress);
            if (iretn < 0)
            {
                return iretn;
            }
        }
    }
    return iretn;
}

//! Move log file - short version.
/*! Move files previously created with ::log_write to their final location.
 * The short version assumes a source location of "temp" and a destination
 * locations of "outgoing". The routine will find all files currently in
 * {node}/temp/{agent} and move them to {node}/outgoing/{agent}.
 * \param node Node name.
 * \param agent Agent name.
 */
int32_t log_move_agent_temp(string node, string agent, float age)
{
    return log_move_agent_src(node, agent, "temp", "outgoing", true, age);
}

//! Relocate files.
/*! Move files previously created with ::log_write to their final location.
 * The short version assumes a source location of "temp" and a destination
 * locations of "outgoing". The routine will find all files currently in
 * {node}/temp/{agent} and move them to {node}/outgoing/{agent}.
 * \param node Node name.
 * \param agent Agent name.
 */
int32_t log_move_directory(string srcdir, string dstdir, bool compress)
{
    int32_t iretn = 0;
    vector<filestruc> files = data_list_files(srcdir);
    if (data_isdir(dstdir, true))
    {
        for (filestruc file : files)
        {
            iretn = log_move_file(file.path, dstdir+"/"+file.name, compress);
        }
        return iretn;
    }
    else
    {
        return GENERAL_ERROR_BAD_DIR;
    }
}

//! Get a list of days in a Node archive.
/*! Generate a list of days available in the archive of the indicated Node and Agent.
 * The result is returned as a vector of Modified Julian Days.
 */
vector <double> data_list_archive_days(string node, string agent)
{
    vector <double> days;

    // Check Base Path
    string bpath = data_base_path(node, "data", agent);
    DIR *jdp;
    if ((jdp=opendir(bpath.c_str())) != nullptr)
    {
        struct dirent *td;
        while ((td=readdir(jdp)) != nullptr)
        {
            // Check Year Path
            if (td->d_name[0] != '.' && atof(td->d_name) > 1900 && atof(td->d_name) < 3000)
            {
                string ypath = (bpath + "/") + td->d_name;
                DIR *jdp;
                if ((jdp=opendir(ypath.c_str())) != nullptr)
                {
                    double year = atof(td->d_name);
                    struct dirent *td;
                    while ((td=readdir(jdp)) != nullptr)
                    {
                        // Check Day Path
                        if (td->d_name[0] != '.' && atof(td->d_name) > 0 && atof(td->d_name) < 367)
                        {
                            string dpath = (ypath + "/") + td->d_name;
                            //                            struct stat st;
                            //                            stat(dpath.c_str(), &st);
                            //                            if (S_ISDIR(st.st_mode))
                            if (data_isdir(dpath))
                            {
                                double jday = atof(td->d_name);
                                double mjd = cal2mjd((int)year, 1, 0.) + jday;
                                days.push_back(mjd);
                            }
                        }
                    }
                    closedir(jdp);
                }
            }
        }
        closedir(jdp);
    }
    sort(days.begin(), days.end());
    return days;
}

//! Use this function to loop through the gzfile until eof is reached. It returns each string line by line in the log file.
//! \brief log_reads Loop until the newline character is seen. Append it to a string and return the line once the newline is met.
//! \param file The file to be read.
//! \param num The maximum number of characters to be written to the buffer.
//! \return Each string in the gzfile.
//!
string log_read(gzFile &file, int num) {
    string buffer;
    buffer.resize(num);
    string line;

    while (!(line.back() == '\n')) {
        gzgets(file, const_cast <char *>(buffer.data()), num);
        line.append(buffer);
    }

    if (!gzeof(file)) {
        return line;
    }

    gzclose(file);

    return "";
}

//! Get a list of files in a Node archive.
/*! Generate a list of archived files for the indicated Node, Agent, and UTC.
 * The result is returned as a vector of ::filestruc, one entry for each file found.
 * \param node Node to search.
 * \param agent Subdirectory of location to search.
 * \param utc Day in archive as MJD.
 * \param type File extension.
 * \return A C++ vector of ::filestruc. Zero size if no files are found.
 */
vector<filestruc> data_list_archive(string node, string agent, double utc, string type)
{
    vector<filestruc> files;

    string dtemp;
    DIR *jdp;
    struct dirent *td;
    filestruc tf;

    tf.node = node;
    tf.agent = agent;
    dtemp = data_archive_path(node, agent, utc);
    if ((jdp=opendir(dtemp.c_str())) != nullptr)
    {
        while ((td=readdir(jdp)) != nullptr)
        {
            if (td->d_name[0] != '.')
            {
                tf.name = td->d_name;
                tf.path = dtemp + "/" + tf.name;
                struct stat st;
                stat(tf.path.c_str(), &st);
                tf.size = st.st_size;
                // Not looking for a specific type, or specifically looking for directory
                if (S_ISDIR(st.st_mode) && (type.empty() || type == "directory"))
                {
                    tf.type = "directory";
                }
                else
                {
                    for (size_t i=strlen(td->d_name)-1; i<strlen(td->d_name); --i)
                    {
                        if (td->d_name[i] == '.')
                        {
                            tf.type = &td->d_name[i+1];
                            break;
                        }
                    }
                }
                // Not looking for a specific type, or found type we were looking for
                if (type.empty() || tf.type == type)
                {
                    int32_t iretn = data_name_date(tf.node, tf.name, tf.year, tf.jday, tf.seconds);
                    if (iretn == 0)
                    {
                        tf.utc = cal2mjd(tf.year, 1, tf.seconds/86400.) + tf.jday;
                        files.push_back(tf);
                        for (size_t i=files.size()-1; i>0; --i)
                        {
                            if (files[i].utc < files[i-1].utc)
                            {
                                tf = files[i-1];
                                files[i-1] = files[i];
                                files[i] = tf;
                            }
                        }
                    }
                }
            }
        }
        closedir(jdp);
    }
    return files;
}

vector<filestruc> data_list_archive(string node, string agent, double utc)
{
    return data_list_archive(node, agent, utc, "");
}

//! Get list of files in a directory, directly.
/*! Generate a list of files for the indicated directory.
 * The result is returned as a vector of ::filestruc, one entry for each file found.
 * \param directory Directory to search.
 * \return A C++ vector of ::filestruc. Zero size if no files are found.
 */
vector<filestruc> data_list_files(string directory)
{
    vector<filestruc> files;

    data_list_files(directory, files);

    return files;
}

//! Get list of files in a directory, indirectly.
/*! Generate a list of files for the indicated directory.
 * The result is returned as a vector of ::filestruc, one entry for each file found.
 * Repeated calls to this function will append entries.
 * \param directory Directory to search.
 * \param files Reference to filestruc vector to fill.
 * \return Number of files found, otherwise negative error.
 */
size_t data_list_files(string directory, vector<filestruc>& files)
{
    DIR *jdp;
    struct dirent *td;
    filestruc tf;

    tf.node = "";
    tf.agent = "";
    if ((jdp=opendir(directory.c_str())) != nullptr)
    {
        while ((td=readdir(jdp)) != nullptr)
        {
            if (td->d_name[0] != '.')
            {
                tf.name = td->d_name;
                tf.directory = directory + "/";
                tf.path =  tf.directory + tf.name;
                vector <string> parts = string_split(directory, "/");
                if (parts.size() > 2)
                {
                    tf.agent = parts[parts.size()-1];
                    tf.node = parts[parts.size()-3];
                }
                struct stat st;
                stat(tf.path.c_str(), &st);
                tf.size = st.st_size;
                tf.utc = unix2utc((double)st.st_ctime);
                if (S_ISDIR(st.st_mode))
                {
                    tf.type = "directory";
                }
                else
                {
                    for (size_t i=strlen(td->d_name)-1; i<strlen(td->d_name); --i)
                    {
                        if (td->d_name[i] == '.')
                        {
                            tf.type = &td->d_name[i+1];
                            break;
                        }
                    }
                }
                files.push_back(tf);
                for (size_t i=files.size()-1; i>1; --i)
                {
                    if (files[i].name < files[i-1].name)
                    {
                        filestruc tfile = files[i-1];
                        files[i-1] = files[i];
                        files[i] = tfile;
                    }
                }
            }
        }
        closedir(jdp);
    }

    return files.size();
}

/**
 * @brief Get list of files and folders in a Node's <location> folder.
 * 
 * location is generally incoming or outgoing.
 * Meaning, the intended purpose of this function is to return the agents in, for example, a node's outgoing folder.
 * 
 * @param node Node to search
 * @param location Subdirectory of node to search
 * @return A C++ vector of ::filestruc. Zero size if no files are found.
 */
vector<filestruc> data_list_files(string node, string location)
{
    vector<filestruc> files;
    string dtemp = data_base_path(node, location);
    data_list_files(dtemp, files);

    return files;
}

//! Get list of files in a Node, directly.
/*! Generate a list of files for the indicated Node, location (eg. incoming, outgoing, ...),
 * and Agent. The result is returned as a vector of ::filestruc, one entry for each file found.
 * \param node Node to search.
 * \param location Subdirectory of Node to search.
 * \param agent Subdirectory of location to search.
 * \return A C++ vector of ::filestruc. Zero size if no files are found.
 */
vector<filestruc> data_list_files(string node, string location, string agent)
{
    vector<filestruc> files;

    data_list_files(node, location, agent, files);

    return files;
}

//! Get list of files in a Node, indirectly.
/*! Generate a list of files for the indicated Node, location (eg. incoming, outgoing, ...),
 * and Agent. The result is returned as a vector of ::filestruc, one entry for each file found.
 * Repeated calls to this function will append entries.
 * \param node Node to search.
 * \param location Subdirectory of Node to search.
 * \param agent Subdirectory of location to search.
 * \param files List of ::filestruc.
 * \return Number of files found, otherwise negative error.
 */
size_t data_list_files(string node, string location, string agent, vector<filestruc>& files)
{
    string dtemp;
    dtemp = data_base_path(node, location, agent);
    //    size_t fcnt = files.size();
    data_list_files(dtemp, files);
    //    for (size_t i=fcnt; i<files.size(); ++i)
    //    {
    //        files[i].agent = agent;
    //        files[i].node = node;
    //    }

    return (files.size());
}

//! Get list of Nodes, directly.
/*! Scan the COSMOS root directory and return the name of each
 * Node that is found.
 * \return Any Nodes that are found.
 */
vector<string> data_list_nodes()
{
    vector<string> nodes;

    data_list_nodes(nodes);

    return nodes;
}

//! Get list of Nodes, indirectly.
/*! Scan the COSMOS root directory and return the name of each
 * Node that is found. Repeated calls to this function will append entries.
 * \param nodes Vector of strings with Node names.
 * \return Zero or negative error.
 */
int32_t data_list_nodes(vector<string>& nodes)
{
    DIR *jdp;
    string dtemp;
    string rootd;
    struct dirent *td;
    string tnode;
    //    struct stat statbuf;

    int32_t iretn = get_cosmosnodes(rootd);
    if (iretn < 0)
    {
        return iretn;
    }

    dtemp = rootd;
    if ((jdp=opendir(dtemp.c_str())) != nullptr)
    {
        while ((td=readdir(jdp)) != nullptr)
        {
            //            if (td->d_name[0] != '.' && !stat(((dtemp+"/")+td->d_name).c_str(), &statbuf) && S_ISDIR(statbuf.st_mode))
            if (td->d_name[0] != '.' && data_isdir((dtemp+"/")+td->d_name))
            {
                tnode = td->d_name;
                nodes.push_back(tnode);
            }
        }
        closedir(jdp);
    }
    return 0;
}

//! Create data file name
/*! Builds a filename up from the date of creation and its type. Format is:
*    yyyyjjjsssss_extra.type, where yyyy is the Year, jjj is the Julian Day, sssss is
*    the Seconds, and type is any accepted COSMOS file type (eg. log, event,
*    telemetry, message, command.)
* \param node Node name.
*    \param mjd UTC of creation date in Modified Julian Day
* \param extra Extra part of file name.
*    \param type Any valid extension type
*    \return Filename string, otherwise nullptr
*/
string data_name(double mjd, string type, string node, string agent, string extra)
{
    string name;

    int32_t year, month, seconds;
    double jday, day;

    mjd2ymd(mjd,year,month,day,jday);
    seconds = static_cast<int32_t>(86400.*(jday-static_cast<int32_t>(jday)));
    name = to_unsigned(year, 4, true) + to_unsigned(jday, 3, true) + to_unsigned(seconds, 5, true);
    if (!node.empty())
    {
        name += ("_" + node);
    }
    if (!agent.empty())
    {
        name += ("_" + agent);
    }
    if (!extra.empty())
    {
        name += ("_" + extra);
    }
    if (!type.empty())
    {
        name += ("." + type);
    }
    return (name);
}

//! Get date from file name.
/*! Assuming the COSMOS standard filename format from ::data_name, extract
 * the date portion and return it as year, julian day and seconds.
 * \param node Name of Node.
 * \param filename Name of File.
 * \param year Holder for integer year.
 * \param jday Holder for integer julian day.
 * \param seconds Holder for integer julian seconds.
 * \return 0 or negative error.
 */
int32_t data_name_date(string node, string filename, uint16_t &year, uint16_t &jday, uint32_t &seconds)
{
    if (sscanf(filename.substr(node.size()+1).c_str(), "%4" SCNu16 "%3" SCNu16 "%5" SCNu32 "", &year, &jday, &seconds) == 3 && seconds < 86400 && jday < 367)
    {
        return 0;
    }
    else
    {
        return DATA_ERROR_FORMAT;
    }
}

//! Get date from file name.
/*! Assuming the COSMOS standard filename format from ::data_name, extract
 * the date portion and return it as a Modified Julian Day.
 * \param node Name of Node.
 * \param filename Name of File.
 * \param utc Holder for returned utc.
 * \return 0 or negative error.
 */
int32_t data_name_date(string node, string filename, double &utc)
{
    uint16_t year;
    uint16_t jday;
    uint32_t seconds;

    int32_t iretn = data_name_date(node, filename, year, jday, seconds);

    if (!iretn)
    {
        utc = cal2mjd(year, 1, seconds/86400.) + jday;
        return 0;
    }
    else
    {
        return iretn;
    }
}

//! Create data file path.
/*! Create a full path to the named file based on the provided information. The path
 * will be of the form {cosmosnodes}/{node}/{location}/{agent}/{filename}, or in the case
 * of a "data" location, {cosmosnodes}/{node}/{location}/{agent}/{yyyy}/{jjj}/{filename}. If
 * {cosmosnodes} is not defined, or cannot be found, it will be left blank.
 */
string data_base_path(string node, string location, string agent, string filename)
{
    string path;
    string tpath;

    tpath = data_base_path(node, location, agent);

    if (!tpath.empty())
    {
        if (location == "data")
        {
            uint16_t year;
            uint16_t jday;
            uint32_t seconds;
            int32_t iretn = data_name_date(node, filename, year, jday, seconds);
            if (!iretn)
            {
                char tbuf[10];
                sprintf(tbuf, "/%04u", year);
                tpath += tbuf;
                if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
                {
                    sprintf(tbuf, "/%03u", jday);
                    tpath += tbuf;
                    if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
                    {
                        path = tpath;
                    }
                }
            }
        }
        else
        {
            path = tpath;
        }

        path += "/" + filename;
    }
    return path;
}

string data_base_path(string node, string location, string agent)
{
    string tpath;
    string path;

    tpath = data_base_path(node, location);
    if (!tpath.empty())
    {
        if (agent.empty())
        {
            tpath += "/none";
        }
        else
        {
            tpath += "/" + agent;
        }
        if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
        {
            path = tpath;
        }
    }
    return path;

}

string data_base_path(string node, string location)
{
    string tpath;
    string path;

    tpath = data_base_path(node);
    if (!tpath.empty())
    {
        if (location.empty())
        {
            tpath += "/none";
        }
        else
        {
            tpath += "/" + location;
        }
        if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
        {
            path = tpath;
        }
    }
    return path;

}

string data_base_path(string node)
{
    string tpath;
    string path="";

    int32_t iretn = get_cosmosnodes(tpath);
    if (iretn >= 0)
    {
        if (node.empty())
        {
            tpath += "/none";
        }
        else
        {
            tpath += node;
        }

        if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
        {

            path = tpath;
        }
    }
    return path;

}

string data_archive_path(string node, string agent, double mjd)
{
    string tpath;
    char ntemp[COSMOS_MAX_NAME+1];
    string path;

    tpath = data_base_path(node, "data", agent);
    if (!tpath.empty())
    {
        int year, month;
        double jday, day;
        mjd2ymd(mjd,year,month,day,jday);
        sprintf(ntemp, "/%04d", year);
        tpath += ntemp;
        if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
        {
            sprintf(ntemp, "/%03d", (int32_t)jday);
            tpath += ntemp;
            if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
            {
                path = tpath;
            }
        }
    }

    return path;

}

//! Create data file path
/*! Build a path to a data file using its filename and the current Node
 * directory.
 * \param node Node directory in ::cosmosroot.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param agent Task specific subfolder of location, if relevant
*    \param mjd UTC of creation date in Modified Julian Day
*    \param type Any valid extension type
*    \return File path string, otherwise nullptr
*/
//string data_type_path(string node, string location, string agent, double mjd, string type)
//{
//    string path;

//    path = data_type_path(node, location, agent, mjd, type);

//    return (path);
//}

//! Create data file path
/*! Build a path to a data file based on time, type, node, agent, and extra information.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param mjd UTC of creation date in Modified Julian Day
 * \param type Any valid extension type
 * \param node Node directory in ::cosmosroot.
 * \param agent Task specific subfolder of location, if relevant
 * \param extra Extra text to add to the full name.
 * \return File path string, otherwise empty
*/
string data_path(string location, double mjd, string type, string node, string agent, string extra)
{
    string path;
    string tpath = "";
    if (!(fabs(mjd) > 0.))
    {
        mjd = currentmjd();
    }

    if (location == "data")
    {
        tpath = data_archive_path(node, agent, mjd);
    }
    else
    {
        tpath = data_base_path(node, location, agent);
    }

    if (!tpath.empty())
    {
        tpath += "/" + data_name(mjd, type, node, agent, extra);
        path = tpath;
    }
    return path;


    return tpath;
}

//! Create data file path
/*! Build a path to a data file using its filename and the current Node
 * directory.
 * \param node Node directory in ::cosmosroot.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param agent Task specific subfolder of location, if relevant
 * \param mjd UTC of creation date in Modified Julian Day
 * \param extra Extra text to add to the full name.
 * \param type Any valid extension type
 * \return File path string, otherwise nullptr
*/
string data_type_path(string node, string location, string agent, double mjd, string type, string extra)
{
    string tpath = data_name_path(node, location, agent, mjd, data_name(mjd, type, node, agent, extra));

    return tpath;
}

//! Create data file path
/*! Build a path to a data file using its filename and the current Node
 * directory.
 * \param node Node directory in ::cosmosroot.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param agent Task specific subfolder of location, if relevant
 * \param mjd UTC of creation date in Modified Julian Day
 * \param name File name.
 * \return File path string, otherwise nullptr
*/
string data_name_path(string node, string location, string agent, double mjd, string name)
{
    string path;
    string tpath;

    if (location == "data")
    {
        tpath = data_archive_path(node, agent, mjd);
    }
    else
    {
        tpath = data_base_path(node, location, agent);
    }

    if (!tpath.empty())
    {
        tpath += "/" + name;
        path = tpath;
    }
    return path;

}

filestruc data_name_struc(string node, string location, string agent, double mjd, string name)
{
    filestruc file;
    file.node = node;
    file.agent = agent;
    file.utc = mjd;
    file.name = name;
    file.path = data_name_path(node, location, agent, mjd, name);
    return file;
}

//! Create resource file path
/*! Build a path to a resource file using its filename and the current resource
 * directory.
 * \param name File name.
 * \return File path string, otherwise nullptr
*/
string data_resource_path(string name)
{
    string path;

    path = get_cosmosresources() + "/" + name;
    if (data_isfile(path))
    {
        return path;
    }
    else
    {
        return "";
    }
}

//! Check existence of path.
/*! Check whether a path exists, within the limits of permissions.
 * \param path string containing full path.
 * \return TRUE or FALSE
 */
bool data_exists(string& path)
{
    struct stat buffer;
    return (stat (path.c_str(), &buffer) == 0);
}

//! Open file from path.
/*! Attempt to open a file with an optional path. If a path is included, each
 * directory element will be created if it is missing. The final file will be
 * opened with the requested mode.
 * \param path Full path to file, absolute or relative.
 * \param mode fopen style mode.
 * \return fopen style file handle, or nullptr if either a directory element can not be
 * created, or the file can not be opened.
 */

FILE *data_open(string path, const char *mode)
{
    char dtemp[1024];
    uint32_t index, dindex, length;
    FILE *tfd;

    length = (uint32_t)path.size();
    for (dindex=length-1; dindex<length; --dindex)
    {
        if (path[dindex] == '/')
            break;
    }

    if (dindex > 0 && dindex < length)
    {
        for (index=1; index<=dindex; ++index)
        {
            if (path[index] == '/')
            {
                strncpy(dtemp, path.c_str(), index+1);
                dtemp[index+1] = 0;
                if (COSMOS_MKDIR(dtemp,00777))
                {
                    if (errno != EEXIST)
                        return (nullptr);
                }
            }
        }
    }

    if ((tfd = fopen(path.c_str(),mode)) != nullptr)
    {
        return (tfd);
    }

    return (nullptr);
}

//! Set Root Directory
/*! Set the internal variable that points to where all COSMOS files
 * are stored.
    \param name Absolute or relative pathname of directory.
    \param create_flag Create directory if not already present.
    \return Zero, or negative error.
*/
int32_t set_cosmosroot(string name, bool create_flag)
{
    cosmosroot.clear();
    for (size_t i=0; i<name.length(); ++i)
    {
        if (name[i] == '\\')
        {
            name.replace(i, 1, "/");
        }
    }
    if (data_isdir(name))
    {
        cosmosroot = name;
        return 0;
    }
    else
    {
        if (create_flag)
        {
            if (COSMOS_MKDIR(name.c_str(), 00777) == 0 || errno == EEXIST)
            {
                cosmosroot = name;
                return 0;
            }
            else
            {
                return -errno;
            }
        }
        else
        {
            return DATA_ERROR_RESOURCES_FOLDER;
        }
    }
}

//! Find Root Directory
/*! Set the internal variable that points to where all COSMOS
 * resource files are stored. This checks, in succession:
 * - a folder named "root" in a path specified by the COSMOS environment variable
 * - the path specified by the COSMOSRESOURCES environment variable
 * - "~/cosmos/root" (Unix); "c:\cosmos\root" (Windows)
 * - up to 6 levels above the current directory, first in "cosmosroot", and then in "root".
 * \return Zero, or negative error.
*/
int32_t set_cosmosroot(bool create_flag)
{
    string croot;
    char *troot;

    if (cosmosroot.empty())
    {
        // if the COSMOS environment variable was set, use it
        if ((troot = getenv("COSMOS")) != nullptr)
        {
            croot = troot;
            if (!set_cosmosroot(croot, create_flag))
            {
                return 0;
            }
        }

        // No environment variables set. Look in standard location.
#if defined(COSMOS_WIN_OS)
        croot = "c:/cosmos";
#elseif defined(COSMOS_MAC_OS)
        croot = "/Applications/cosmos";
#else
        croot = "/cosmos";
#endif
        if (!set_cosmosroot(croot, create_flag))
        {
            return 0;
        }

        // , or home folder for running user
        if ((troot = getenv("HOME")) != nullptr)
        {
            croot = troot + string("/cosmos");
            if (!set_cosmosroot(croot, create_flag))
            {
                return 0;
            }
        }

        // No standard location. Search upward for "cosmosroot"
        croot.resize(501);
        croot = data_getcwd();
        size_t cindex = croot.find("cosmos");
        if (cindex != string::npos)
        {
            croot.erase(cindex+6, string::npos);
            if (!set_cosmosroot(croot, create_flag))
            {
                return 0;
            }
        }

        cosmosroot.clear();
        return (DATA_ERROR_ROOT_FOLDER);
    }
    return 0;
}

//! Return COSMOS Root Directory
/*! Get the internal variable that points to where all COSMOS Resource files are
             * stored. Initialize variable if this is the first call to the function.
    \param create_flag Create directory if not already present.
             * \return Length of string, otherwise negative error.
            */
string get_cosmosroot(bool create_flag)
{
    string result;
    get_cosmosroot(result, create_flag);
    return result;
}

//! Return COSMOS Root Directory
/*! Get the internal variable that points to where all COSMOS Resource files are
             * stored. Initialize variable if this is the first call to the function.
             * \param result Full path to Root directory.
    \param create_flag Create directory if not already present.
             * \return Length of string, otherwise negative error.
            */
int32_t get_cosmosroot(string &result, bool create_flag)
{
    int32_t iretn = 0;

    result.clear();
    if (cosmosroot.empty())
    {
        iretn = set_cosmosroot(create_flag);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    result = cosmosroot;
    return 0;
}


//! Set Resources Directory
/*! Set the internal variable that points to where all COSMOS resource files
             * are stored.
                \param name Absolute or relative pathname of directory.
    \param create_flag Create directory if not already present.
                \return Zero, or negative error.
            */
int32_t set_cosmosresources(string name, bool create_flag)
{
    cosmosresources.clear();
    for (size_t i=0; i<name.length(); ++i)
    {
        if (name[i] == '\\')
        {
            name.replace(i, 1, "/");
        }
    }
    if (data_isdir(name))
    {
        cosmosresources = name;
        return 0;
    }
    else
    {
        if (create_flag)
        {
            if (COSMOS_MKDIR(name.c_str(), 00777) == 0 || errno == EEXIST)
            {
                cosmosresources = name;
                return 0;
            }
            else
            {
                return -errno;
            }
        }
        else
        {
            return DATA_ERROR_RESOURCES_FOLDER;
        }
    }
}

//! Find Resources Directory
/*! Set the internal variable that points to where all COSMOS
             * resource files are stored. This checks, in succession:
             * - a folder named "resources" in a path specified by the COSMOS environment variable
             * - the path specified by the COSMOSRESOURCES environment variable
             * - "~/cosmos/resources" (Unix); "c:\cosmos\resources" (Windows)
             * - up to 6 levels above the current directory, first in "cosmosresources", and then in "resources".
             * \return Zero, or negative error.
            */
int32_t set_cosmosresources(bool create_flag)
{
    string croot;
    char *troot;
    int32_t iretn = 0;

    if (cosmosresources.empty())
    {
        if ((troot = getenv("COSMOSRESOURCES")) != nullptr)
        {
            croot = troot;
            if ((iretn=set_cosmosresources(croot, create_flag)) == 0)
            {
                return 0;
            }
            else
            {
                return iretn;
            }
        }
        else
        {
            if ((iretn = set_cosmosroot(create_flag)) == 0)
            {
                croot = cosmosroot + "/resources";
                iretn = set_cosmosresources(croot, create_flag);
                return iretn;
            }
            else
            {
                return iretn;
            }
        }
    }

    return 0;
}

//! Return COSMOS Resources Directory
/*! Get the internal variable that points to where all COSMOS Resource files are
             * stored. Initialize variable if this is the first call to the function.
    \param create_flag Create directory if not already present.
             * \return Length of string, otherwise negative error.
            */
string get_cosmosresources(bool create_flag)
{
    string result="";
    get_cosmosresources(result, create_flag);
    return result;
}

//! Return COSMOS Resources Directory
/*! Get the internal variable that points to where all COSMOS Resource files are
             * stored. Initialize variable if this is the first call to the function.
             * \param result Full path to Resources directory.
    \param create_flag Create directory if not already present.
             * \return Length of string, otherwise negative error.
            */
int32_t get_cosmosresources(string &result, bool create_flag)
{
    int32_t iretn = 0;

    result.clear();
    if (cosmosresources.empty())
    {
        iretn = set_cosmosresources(create_flag);
        if (iretn < 0)
        {
            return iretn;
        }
    }

    result = cosmosresources;
    return 0;
}

//! Set Environment Variable for COSMOS resources
/*! \param path Full path of the COSMOS resources folder.
                \return Zero, or negative error.
            */
int32_t setEnvCosmosResources(string path){

    return setEnv("COSMOSRESOURCES", path);
}


//! Set Environment Variable for COSMOS nodes
/*! \param path Full path of the COSMOS nodes folder.
                \return Zero, or negative error.
            */
int32_t setEnvCosmosNodes(string path){

    return setEnv("COSMOSNODES", path);
}

//! Set Environment Variable for COSMOS
/*! \param var environment variable to set (ex. COSMOSRESOURCES)
             *  \param path Full path of the COSMOS variable folder.
                \return Zero, or negative error.
            */
int32_t setEnv(string var, string path){

    uint32_t iretn = 0;

#ifdef COSMOS_WIN_OS
    // windows
    iretn = _putenv_s(var.c_str(),path.c_str());
#else
    // mac, linux
    iretn = setenv(var.c_str(),
                   path.c_str(),1);
#endif
    char *pathReturned = getenv(var.c_str());
    if (pathReturned == nullptr)
    {
        return DATA_ERROR_RESOURCES_FOLDER;
    }

    //    if (pathReturned!=NULL){
    //        std::cout << var << " set to " << pathReturned << std::endl;
    //    } else {
    //        std::cout << var << " not set " << std::endl;
    //        return DATA_ERROR_RESOURCES_FOLDER;
    //    }

    return iretn;
}

//! Set Environment Variable for COSMOS Automatically
//! These variables are just temporarily created while the
//! program runs.
/*! \param path full path of the COSMOS variable folder.
                \return Zero, or negative error.
            */
int32_t setEnvCosmos(string path){

    uint32_t iretn = 0;

    iretn = setEnv("COSMOSRESOURCES", path + "resources");
    iretn = setEnv("COSMOSNODES", path + "nodes");

    return iretn;
}


//! Set Nodes Directory
/*! Set the internal variable that points to where all COSMOS resource files
             * are stored.
                \param name Absolute or relative pathname of directory.
    \param create_flag Create directory if not already present.
                \return Zero, or negative error.
            */
int32_t set_cosmosnodes(string name, bool create_flag)
{
    cosmosnodes.clear();
    for (size_t i=0; i<name.length(); ++i)
    {
        if (name[i] == '\\')
        {
            name.replace(i, 1, "/");
        }
    }
    if (data_isdir(name))
    {
        cosmosnodes = name;
        return 0;
    }
    else
    {
        if (create_flag)
        {
            if (COSMOS_MKDIR(name.c_str(), 00777) == 0 || errno == EEXIST)
            {
                cosmosnodes = name;
                return 0;
            }
            else
            {
                return -errno;
            }
        }
        else
        {
            return DATA_ERROR_NODES_FOLDER;
        }
    }
}

//! Find COSMOS Nodes Directory on Windows, Linux or MacOS
/*! Set the internal variable that points to where all COSMOS node files
             * are stored. This either uses the value in COSMOSNODES, or looks for the directory
             * up to 6 levels above the current directory, first in "cosmosnodes", and then in "nodes".
             * \return Zero, or negative error.
            */
int32_t set_cosmosnodes(bool create_flag)
{
    string croot;
    char *troot;
    int32_t iretn = 0;

    if (cosmosnodes.empty())
    {
        // find environment variable COSMOSNODES
        if ((troot = getenv("COSMOSNODES")) != nullptr)
        {
            croot = troot;
            if ((iretn=set_cosmosnodes(croot, create_flag)) == 0)
            {
                return 0;
            }
            else
            {
                return iretn;
            }
        }
        else // look for the default path: $COSMOS/nodes or $HOME/cosmos/nodes
        {
            if ((iretn = set_cosmosroot(create_flag)) == 0)
            {
                croot = cosmosroot + "/nodes";
                iretn = set_cosmosnodes(croot, create_flag);
                return iretn;
            }
            else
            {
                return iretn;
            }
        }
    }

    return 0;
}

//! Return COSMOS Nodes Directory
/*! Get the internal variable that points to where all COSMOS Node files are
             * stored. Initialize variable if this is the first call to the function.
    \param create_flag Create directory if not already present.
             * \return Length of string, otherwise negative error.
            */
string get_cosmosnodes(bool create_flag)
{
    string result="";
    get_cosmosnodes(result, create_flag);
    return result;
}

//! Get COSMOS Nodes Directory
/*! Get the internal variable that points to where all COSMOS files are
             * stored.
             * \param result String to place path in.
    \param create_flag Create directory if not already present.
             * \return Zero, or negative error.
            */
int32_t get_cosmosnodes(string &result, bool create_flag)
{
    int32_t iretn = 0;

    result.clear();
    if (cosmosnodes.empty())
    {
        iretn = set_cosmosnodes(create_flag);
        if (iretn < 0)
        {
            //            std::cerr << "error " << DATA_ERROR_NODES_FOLDER << ": could not find cosmos/nodes folder" << std::endl;
            return iretn;
        }
    }

    if (cosmosnodes.back() == '/')
    {
        result = cosmosnodes;
    }
    else
    {
        result = cosmosnodes + '/';
    }
    return 0;
}

//! Get Current Node Directory
/*! Get the internal variable that points to where node files are
             * stored for the current Node.
             * \param node Name of current Node
             * \param create_flag Whether or not to create node directory if it doesn't already exist.
             * \return Pointer to character string containing path to Node, otherwise nullptr.
            */
string get_nodedir(string node, bool create_flag)
{
    nodedir.clear();
    if (!set_cosmosnodes(create_flag))
    {
        nodedir = cosmosnodes + "/" + node;

        // if the node folder does not exist
        if (!data_isdir(nodedir))
        {
            // if the create folder flag is not on then
            // exit this function without a nodedir
            if (!create_flag)
            {
                nodedir.clear();
            }
            else // let's create the node directory (good for on the fly nodes)
            {
                if (COSMOS_MKDIR(nodedir.c_str(),00777) != 0)
                {
                    nodedir.clear();
                }
            }
        } else {
            // node folder exists
        }
    }

    // if the node folder exists or was created let's return the path
    return (nodedir);
}

//! Move data file - filestruc version.
/*! Move files previously created using ::file_struc to their final location, optionally
 * compressing with gzip. The routine will move the file specified in filestruc to {node}/{dstlocation}/{agent}.
 * \param filestruc File information.
 * \param location Destination location name.
 * \param compress Wether or not to compress with gzip.
 */
int32_t data_move_file(filestruc file, string location, bool compress)
{
    int32_t iretn = log_move_file(file.path, data_base_path(file.node, location, file.agent, file.name), compress);
    return iretn;
}

//! Move data file - filepath version.
/*! Move filerepresented by path previously created with ::data_path, optionally
 * compressing with gzip. The routine will move the file specified in filepath to {node}/{dstlocation}/{agent}.
 * The path is assumed to be of the form {cosmosnodes}/{node}/{location}/{agent}/{filename}
 * \param filepath File information.
 * \param location Destination location name.
 * \param compress Wether or not to compress with gzip.
 */
int32_t data_move_file(string path, string location, bool compress)
{
    vector<string> parts = string_split(path, "/");
    if (parts.size() >= 5)
    {
        int32_t iretn = log_move_file(path, data_base_path(parts[parts.size()-4], location, parts[parts.size()-2], parts[parts.size()-1]), compress);
        return iretn;
    }
    else
    {
        return GENERAL_ERROR_ARGS;
    }
}

//! Load data from archive
/*! Load JSON entries of specified type from data archive for specified Node and Agent.
             * Will return all data that is available within specified date range, in files
             * {COSMOSNODES}/{Node}/date/{Agent}/{yyyy}/{ddd}/{*}.type.
             * \param node Name of Node.
             * \param agent Name of Agent.
             * \param utcbegin Starting UTC.
             * \param utcend Ending UTC.
             * \param type Type extension.
             * \param result Vector of JSON strings.
             * \return 0 or negative error.
             */
int32_t data_load_archive(string node, string agent, double utcbegin, double utcend, string type, vector<string> &result)
{
    std::ifstream tfd;
    string tstring;
    vector <filestruc> files;


    result.clear();

    for (double mjd = floor(utcbegin); mjd <= floor(utcend); ++mjd)
    {
        files = data_list_archive(node, agent, mjd, type);
        for (size_t i=0; i<files.size(); ++i)
        {
            if (mjd == floor(utcbegin) && i < files.size()-2 && files[i+1].utc < utcbegin)
            {
                continue;
            }
            else if (mjd == floor(utcend) && files[i].utc > utcend)
            {
                continue;
            }

            tfd.open(files[i].path);
            if (tfd.is_open())
            {
                while (std::getline(tfd,tstring))
                {
                    result.push_back(tstring);
                }
                tfd.close();
            }
        }
    }
    return 0;
}

int32_t data_load_archive(string node, string agent, double mjd, string type, vector<string> &result)
{
    int32_t iretn = 0;
    iretn = data_load_archive(node, agent, floor(mjd), floor(mjd)+.999999, type, result);
    return iretn;
}

//int32_t data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc *cinfo)
//{
//    int32_t iretn = 0;

//    iretn = data_load_archive(cinfo->node.name, "soh", mjd, "telemetry", telem);
//    if (iretn < 0)
//    {
//        return iretn;
//    }
//    iretn = data_load_archive(cinfo->node.name, "soh", mjd, "event", event);

//    return iretn;
//}

//! Find last day in archive
/*! Searches through data archives for this Node to find most recent
             * day for which data is available. This is then stored in lastday.
             \return MJD of last day in archive, or zero.
            */
double findlastday(string name)
{
    DIR *jdp;
    struct dirent *td;
    int year, jday;
    char dtemp[356];
    struct tm mytm;
    time_t mytime;

    year = jday = 0;
    if (get_nodedir(name).size())
    {
        sprintf(dtemp,"%s/data/soh", nodedir.c_str());
        if ((jdp=opendir(dtemp))!=nullptr)
        {
            while ((td=readdir(jdp))!=nullptr)
            {
                if (td->d_name[0] != '.')
                {
                    if (atol(td->d_name) > year)
                        year = atol(td->d_name);
                }
            }
            closedir(jdp);
            sprintf(dtemp,"%s/data/soh/%04d",nodedir.c_str(),year);
            if ((jdp=opendir(dtemp))!=nullptr)
            {
                while ((td=readdir(jdp))!=nullptr)
                {
                    if (td->d_name[0] != '.')
                    {
                        if (atol(td->d_name) > jday)
                            jday = atol(td->d_name);
                    }
                }
                closedir(jdp);
            }
        }

        if (year == 0. || jday == 0.)
        {
            return (0.);
        }

        mytm.tm_year = year - 1900;
        mytm.tm_hour = mytm.tm_min = mytm.tm_sec = 0;
        mytm.tm_mon = mytm.tm_mday = mytm.tm_wday = 0;
        mytm.tm_mday = 1;
        mytm.tm_mon = 0;
        mytm.tm_isdst = 0;
        mytime = mktime(&mytm);
        mytime += static_cast<int>(((jday-1) * 86400.));
#ifdef COSMOS_WIN_OS
        struct tm *temptm;
        temptm = localtime(&mytime);
        if(temptm!=nullptr)
        {
            mytm = *temptm;
        }
#else
        localtime_r(&mytime,&mytm);
#endif
        return cal2mjd(year,mytm.tm_mon+1,mytm.tm_mday);
    }
    else
    {
        return 0.;
    }
}

//! Find first day in archive
/*! Searches through data archives for this Node to find oldest
             * day for which data is available. This is then stored in firstday.
             \return MJD of last day in archive.
            */
double findfirstday(string name)
{
    DIR *jdp;
    struct dirent *td;
    int year, jday;
    char dtemp[356];
    struct tm mytm;
    time_t mytime;

    if (get_nodedir(name).size())
    {
        year = jday = 9000;
        sprintf(dtemp,"%s/data/soh", nodedir.c_str());
        if ((jdp=opendir(dtemp))!=nullptr)
        {
            while ((td=readdir(jdp))!=nullptr)
            {
                if (td->d_name[0] != '.')
                {
                    if (atol(td->d_name) < year)
                        year = atol(td->d_name);
                }
            }
            closedir(jdp);
            sprintf(dtemp,"%s/data/soh/%04d",nodedir.c_str(),year);
            if ((jdp=opendir(dtemp))!=nullptr)
            {
                while ((td=readdir(jdp))!=nullptr)
                {
                    if (td->d_name[0] != '.')
                    {
                        if (atol(td->d_name) < jday)
                            jday = atol(td->d_name);
                    }
                }
                closedir(jdp);
            }
        }

        if (year == 9000. || jday == 9000.)
        {
            return (0.);
        }

        mytm.tm_year = year - 1900;
        mytm.tm_hour = mytm.tm_min = mytm.tm_sec = 0;
        mytm.tm_mon = mytm.tm_mday = mytm.tm_wday = 0;
        mytm.tm_mday = 1;
        mytm.tm_mon = 0;
        mytm.tm_isdst = 0;
        mytime = mktime(&mytm);
        mytime += (int)((jday-1) * 86400.);
#ifdef COSMOS_WIN_OS
        struct tm *temptm;
        temptm = localtime(&mytime);
        mytm = *temptm;
#else
        localtime_r(&mytime,&mytm);
#endif

        return (cal2mjd(year,mytm.tm_mon+1,mytm.tm_mday));
    }
    else
    {
        return 0.;
    }
}

bool data_isdir(string path, bool create_flag)
{
    struct stat st;

    if (path.empty())
    {
        return false;
    }

    if (create_flag)
    {
        vector<string> dirs = string_split(path, "/");
        string tpath;
        if (path[0] == '/')
        {
            tpath = "/";
        }
        for (string subdir : dirs)
        {
            tpath += subdir;
            if (COSMOS_MKDIR(tpath.c_str(), 00777) < 0 && errno != EEXIST)
            {
                return false;
            }
            tpath += "/";
        }
    }

    if (!stat(path.c_str(), &st) && S_ISDIR(st.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool data_ischardev(string path)
{
    struct stat st;

    if (!stat(path.c_str(), &st) && S_ISCHR(st.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool data_isblkdev(string path)
{
    struct stat st;

    if (!stat(path.c_str(), &st) && S_ISBLK(st.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool data_issymlink(string path)
{
    struct stat st;

    if (!stat(path.c_str(), &st) && S_ISLNK(st.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }

}

//! Check if file exists at given path.
//! Optionally checks if the file is the specified size.
//! \param path File path
//! \param size Check if file is this size. Optional.
//! \return true if file exists and, if size was specified, is equal to the specified size.
bool data_isfile(string path, off_t size)
{
    struct stat st;

    if (!stat(path.c_str(), &st) && S_ISREG(st.st_mode) && (!size || (size == st.st_size)))
    {
        return true;
    }
    else
    {
        return false;
    }

}

double data_ctime(string path)
{
    struct stat st;

    if (!stat(path.c_str(), &st))
    {
        struct timeval unixtime;
#ifdef COSMOS_WIN_OS
        unixtime.tv_sec = st.st_ctime;
        unixtime.tv_usec = 0;
#elif defined(COSMOS_LINUX_OS)
        unixtime.tv_sec = st.st_ctim.tv_sec;
        unixtime.tv_usec = st.st_ctim.tv_nsec / 1000;
#endif
        return unix2utc(unixtime);
    }
    else
    {
        return 0.;
    }
}

off_t data_size(string path)
{
    struct stat stat_buf;

    if ((stat(path.c_str(), &stat_buf)) == 0)
    {
        return  stat_buf.st_size;
    }
    else
    {
        return 0;
    }

}

int32_t data_execute(vector<uint8_t> cmd, float timer)
{
    string result;
    return data_execute(cmd, result, timer);
}

int32_t data_execute(string cmd, float timer)
{
    string result;
    return data_execute(cmd, result, timer);
}

int32_t data_execute(vector<uint8_t> cmd, string& result, float timer, string shell)
{
    return data_execute(string(cmd.begin(), cmd.end()), result, timer, shell);
}

int32_t data_execute(string cmd, string& result, float timer, string shell)
{
#if defined(COSMOS_WIN_OS)
    if (!data_isfile(cmd))
    {
        return GENERAL_ERROR_UNDEFINED;
    }

    /* child process's STDIN is the user input or data that you enter into the child process - READ */
    void * g_hChildStd_IN_Rd = NULL;
    /* child process's STDIN is the user input or data that you enter into the child process - WRITE */
    void * g_hChildStd_IN_Wr = NULL;
    /* child process's STDOUT is the program output or data that child process returns - READ */
    void * g_hChildStd_OUT_Rd = NULL;
    /* child process's STDOUT is the program output or data that child process returns - WRITE */
    void * g_hChildStd_OUT_Wr = NULL;
    SECURITY_ATTRIBUTES saAttr;

    cmd.insert(0, "cmd.exe /c ");

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    //child process's STDOUT is the program output or data that child process returns
    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
    {
        return -1;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        return -1;
    }

    //child process's STDIN is the user input or data that you enter into the child process
    // Create a pipe for the child process's STDIN.
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    {
        return -1;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited.
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        return -1;
    }

    // Create child process
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure.

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.

    bSuccess = CreateProcess(NULL,
                             (LPSTR)cmd.c_str(),                // command line
                             NULL,               // process security attributes
                             NULL,               // primary thread security attributes
                             TRUE,               // handles are inherited
                             CREATE_NO_WINDOW,   // creation flags
                             NULL,               // use parent's environment
                             NULL,               // use parent's current directory
                             &siStartInfo,       // STARTUPINFO pointer
                             &piProcInfo);       // receives PROCESS_INFORMATION

    // If an error occurs, exit the application.
    if (bSuccess)
    {
        // Close handles to the child process and its primary thread.
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        CloseHandle(g_hChildStd_OUT_Wr);

        COMMTIMEOUTS ct;
        int size_recv = 0;
        unsigned long dwRead;
        BOOL bSuccess = TRUE;
        string accum;

        //Set timeouts for stream
        ct.ReadIntervalTimeout = 0;
        ct.ReadTotalTimeoutMultiplier = 0;
        ct.ReadTotalTimeoutConstant = 10;
        ct.WriteTotalTimeoutConstant = 0;
        ct.WriteTotalTimeoutMultiplier = 0;
        SetCommTimeouts(g_hChildStd_OUT_Rd, &ct);


        //This accumulates each read into one buffer,
        //and copies back into rsp before leaving
        size_t size = result.size();
        accum.resize(size);
        result.clear();

        do
        {
            //Reads stream from child stdout
            bSuccess = ReadFile(g_hChildStd_OUT_Rd, (LPVOID)&accum[0], size-1, &dwRead, NULL);
            if (!bSuccess || dwRead == 0)
            {
                return 0;//successful - reading is done
            }

            accum[dwRead] = 0;
            size_recv = accum.length();


            if(size_recv == 0)
            {
                //should not get here for streaming
                return result.size();
            }
            else
            {
                //New Chunk:
                accum[size_recv]=0;
                result += accum;
            }


        }while(1);
    }


#else
    int32_t iretn = 0;
    FILE * stream;
//    char buffer[198];
    result.clear();

    vector<string> cmds = string_split(cmd, " ");
    if (data_isfile(cmds[0]))
    {

    }
    else if (data_isfile("/bin/" + cmds[0]))
    {
        cmd.insert(0, "/bin/");
    }
    else if (data_isfile("/usr/bin/" + cmds[0]))
    {
        cmd.insert(0, "/usr/bin/");
    }
    else if (data_isfile("/sbin/" + cmds[0]))
    {
        cmd.insert(0, "/sbin/");
    }
    else if (data_isfile("/usr/sbin/" + cmds[0]))
    {
        cmd.insert(0, "/usr/sbin/");
    }
    else if (data_isfile(get_cosmosroot() + "/scripts/" + cmds[0]))
    {
        cmd.insert(0, get_cosmosroot() + "/scripts/");
    }
    else if (data_isfile(get_cosmosroot() + "/bin/" + cmds[0]))
    {
        cmd.insert(0, get_cosmosroot() + "/bin/");
    }
    else
    {
        return GENERAL_ERROR_UNDEFINED;
    }

    if (shell.empty())
    {
        char *eshell = getenv("SHELL");
        if ((eshell) != nullptr)
        {
            shell = eshell;
        }
    }

    if (shell != "/bin/sh" && data_isfile(shell))
    {
        cmd.insert(0, shell + " -c \"");
        if (shell.find("csh") != string::npos)
        {
            cmd.append(" |& cat\"");
        }
        else
        {
            cmd.append(" 2>&1\"");
        }
    }
    else
    {
        cmd.append(" 2>&1");
    }

    stream = popen(cmd.c_str(), "r");
    fcntl(fileno(stream), F_SETFL, O_NONBLOCK);
    if (stream)
    {
        ElapsedTime et;
        do
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(fileno(stream), &set);
            timeval timeout;
            timeout.tv_sec = static_cast<int32_t>(timer);
            timeout.tv_usec = static_cast<int32_t>(1000000. * (timer - timeout.tv_sec));
            int rv = select(fileno(stream)+1, &set, nullptr, nullptr, &timeout);
            if (rv == -1)
            {
                iretn = -errno;
                break;
            }
            else if (rv == 0)
            {
                iretn = GENERAL_ERROR_TIMEOUT;
                break;
            }
            else
            {
                if (FD_ISSET(fileno(stream), &set))
                {
                    uint8_t c;
                    int response = read(fileno(stream), &c, 1);
                    if (response > 0)
                    {
                        result.push_back(c);
                    }
                    else
                    {
                        if (response < 0)
                        {
                            iretn = -errno;
                            break;
                        }
                        else
                        {
                            iretn = GENERAL_ERROR_TIMEOUT;
                            break;
                        }
                    }
                }
            }
        } while (et.split() < timer);

//        while (!feof(stream))
//        {
//            if (fgets(buffer, 198, stream) != nullptr)
//            {
//                result.append(buffer);
//            }
//        }
        if ((iretn=pclose(stream)) < 0)
        {
            iretn = -errno;
        }
        iretn = result.size();
    }
    else
    {
        iretn = -errno;
    }
#endif

    return iretn;
}

int32_t data_task(string command, string outpath, float timeout, string shell)
{
    string result;
    int32_t iretn = data_execute(command, result, timeout, shell);
    FILE* fp = fopen(outpath.c_str(), "w");
    if (fp != nullptr)
    {
        fwrite(result.data(), result.size(), 1, fp);
        fclose(fp);
        return iretn;
    }
    else
    {
        return -errno;
    }
}

int32_t data_shell(string command_line, string outpath, string inpath, string errpath)
{
    printf("Data: Shell Command=%s Out=%s Err=%s\n", command_line.c_str(), outpath.c_str(), errpath.c_str());
    fflush(stdout);
    int32_t iretn=0;
    int devin, devout, deverr;
    int prev_stdin, prev_stdout, prev_stderr;

    if (command_line.empty())
    {
        return GENERAL_ERROR_EMPTY;
    }

    if (outpath.empty())
    {
        devout = dup(STDOUT_FILENO);
    }
    else
    {
        devout = open(outpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
        if (devout == -1)
        {
            devout = dup(STDOUT_FILENO);
        }
    }
    // Redirect.
    prev_stdout = dup(STDOUT_FILENO);
    dup2(devout, STDOUT_FILENO);
    close(devout);

    if (inpath.empty())
    {
        devin = open("/dev/null", O_RDWR);
    }
    else
    {
        devin = open(inpath.c_str(), O_RDONLY, 00666);
        if (devin == -1)
        {
            devin = open("/dev/null", O_RDWR);
        }
    }
    prev_stdin = dup(STDIN_FILENO);
    dup2(devin, STDIN_FILENO);
    close(devin);

    prev_stderr = dup(STDERR_FILENO);
    if (errpath.empty())
    {
        deverr = devout;
    }
    else
    {
        deverr = open(errpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
        if (deverr == -1)
        {
            deverr = devout;
        }
    }
    dup2(deverr, STDERR_FILENO);
    close(deverr);

    // Execute the command.
    iretn = system(command_line.c_str());

    // Reset standard file handles
    dup2(prev_stdin, STDIN_FILENO);
    dup2(prev_stdout, STDOUT_FILENO);
    dup2(prev_stderr, STDERR_FILENO);
    close(prev_stdin);
    close(prev_stdout);
    close(prev_stderr);

    return iretn;
}

// Define the static member variables here
map<string, uint8_t> NodeData::node_ids;

//! Loads node table from nodeids.ini configuration file
//! nodeids is a vector of node name strings indexed by a node_id
int32_t NodeData::load_node_ids()
{
    if (NodeData::node_ids.size() == 0)
    {
        char buf[103];
        FILE *fp = data_open(get_cosmosnodes()+"/nodeids.ini", "rb");
        if (fp)
        {
            // Loop until eof
            while (fgets(buf, 102, fp) != nullptr)
            {
                uint16_t nodeid = 0;
                string node_name;
                // Turn whitespace into null terminators, then grab node names and idxs
                if (buf[strlen(buf)-1] == '\n')
                {
                    buf[strlen(buf)-1] = 0;
                }
                if (buf[1] == ' ')
                {
                    buf[1] = 0;
                    nodeid = atoi(buf);
                    node_name = &buf[2];
                }
                else if (buf[2] == ' ')
                {
                    buf[2] = 0;
                    nodeid = atoi(buf);
                    node_name = &buf[3];
                }
                else if (buf[3] == ' ')
                {
                    buf[3] = 0;
                    nodeid = atoi(buf);
                    node_name = &buf[4];
                }
                else
                {
                    continue;
                }
                node_ids[node_name] = nodeid;
            }
            fclose(fp);
        }
        else
        {
            return -errno;
        }
    }

    return NodeData::node_ids.size();
}

//! Check if a node_id is in the node table
//! \param node_id
//! \return node_id on success, NODEIDUNKNOWN (0) if not found, negative on error
int32_t NodeData::check_node_id(NODE_ID_TYPE node_id)
{

    if (NodeData::load_node_ids() <= 0)
    {
        return NODEIDUNKNOWN;
    }

    for (auto it = node_ids.begin(); it != node_ids.end(); ++it)
    {
        if (it->second == node_id)
        {
            return node_id;
        }
    }
    return NODEIDUNKNOWN;
}

//! Gets the node_id associated with a node name
//! \return node_id on success, NODEIDUNKNOWN (0) if not found, negative on error
int32_t NodeData::lookup_node_id(string node_name)
{
    int32_t iretn = 0;

    if ((iretn=NodeData::load_node_ids()) <= 0)
    {
        return NODEIDUNKNOWN;
    }

    auto it = node_ids.find(node_name);
    if (it == node_ids.end())
    {
        return NODEIDUNKNOWN;
    }
    return it->second;
}

//! Find the node name associated with the given node id in the node table.
//! \param node_id Node ID
//! \return Node name on success, or empty string on failure
string NodeData::lookup_node_id_name(NODE_ID_TYPE node_id)
{
    if (node_id == NodeData::NODEIDORIG)
    {
        return "Origin";
    }
    else if (node_id == NodeData::NODEIDDEST)
    {
        return "Destination";
    }
    else if (node_id == NodeData::NODEIDUNKNOWN || NodeData::load_node_ids() <= 0)
    {
        return "";
    }

    for (auto it = node_ids.begin(); it != node_ids.end(); ++it)
    {
        if (it->second == node_id)
        {
            return it->first;
        }
    }

    return "";
}

const string data_getcwd()
{
    size_t buf_size = 1024;
    char* buf = NULL;
    char* r_buf;

    do
    {
        buf = static_cast<char*>(realloc(buf, buf_size));
        r_buf = getcwd(buf, buf_size);
        if (!r_buf)
        {
            if (errno == ERANGE)
            {
                buf_size *= 2;
            }
            else
            {
                free(buf);
                return "";
                // Or some other error handling code
            }
        }
    } while (!r_buf);

    string str(buf);
    free(buf);
    return str;
}

void rmdir(const string& dirpath)
{
    // (In)sanity checks before running rm -r
    if (data_isdir(dirpath)
    && std::count(dirpath.begin(), dirpath.end(), '/') > 3
    && !std::count(dirpath.begin(), dirpath.end(), ' ')
    && !std::count(dirpath.begin(), dirpath.end(), '\t')
    && !std::count(dirpath.begin(), dirpath.end(), '\v')
    && !std::count(dirpath.begin(), dirpath.end(), '\n')
    && !std::count(dirpath.begin(), dirpath.end(), '\r')
    && !std::count(dirpath.begin(), dirpath.end(), '\\')
    && !std::count(dirpath.begin(), dirpath.end(), '|')
    && !std::count(dirpath.begin(), dirpath.end(), '-')
    && !std::count(dirpath.begin(), dirpath.end(), '.'))
    {
        data_execute("rm -r " + dirpath);
    }
}

// Removes all contents of a folder, but keeps the folder itself
void rmdir_contents(const string& dirpath)
{
    // (In)sanity checks before running rm -r
    if (data_isdir(dirpath)
    && std::count(dirpath.begin(), dirpath.end(), '/') > 3
    && !std::count(dirpath.begin(), dirpath.end(), ' ')
    && !std::count(dirpath.begin(), dirpath.end(), '\t')
    && !std::count(dirpath.begin(), dirpath.end(), '\v')
    && !std::count(dirpath.begin(), dirpath.end(), '\n')
    && !std::count(dirpath.begin(), dirpath.end(), '\r')
    && !std::count(dirpath.begin(), dirpath.end(), '\\')
    && !std::count(dirpath.begin(), dirpath.end(), '|')
    && !std::count(dirpath.begin(), dirpath.end(), '-')
    && !std::count(dirpath.begin(), dirpath.end(), '.'))
    {
        data_execute("rm -r " + dirpath + "/*");
    }
}

void GITTEST::f()	{
    return;
}

//! @}
