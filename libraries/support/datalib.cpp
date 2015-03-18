/*! \file datalib.cpp
	\brief Data support functions
*/

#include "datalib.h"
#include <algorithm>

//! \ingroup datalib
//! \defgroup datalib_statics Static variables for Data functions.
//! @{

//static vector<cosmosstruc> nodes;

//! Path to Project Nodes directory
string cosmosnodes;
//! Path to COSMOS Resources directory
string cosmosresources;
//! Path to current COSMOS Node directory
string nodedir;

//! @}

// MSVC lacks these POSIX macros and other compilers may too:
#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

//! \ingroup datalib
//! \defgroup datalib_functions Data Management support functions
//! @{


//! Write log entry - full
/*! Append the provided string to a file in the {node}/{location}/{agent} directory. The file name
 * is created as {node}_yyyyjjjsssss_{extra}.{type}
 * \param node Node name.
 * ]param location Location name.
 * \param agent Agent name.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param extra Extra part  of name.
 * \param type Type part of name.
 * \param record String to be appended to file.
 */
void log_write(string node, string location, string agent, double utc, string extra, string type, string record)
{
	FILE *fout;
	string path;

	if (utc == 0.)
		return;

	if (extra.empty())
	{
		path = data_type_path(node, location, agent, utc, type);
	}
	else
	{
		path = data_type_path(node, location, agent, utc, extra, type);
	}

	if ((fout = data_open(path, (char *)"a+")) != nullptr)
	{
		fprintf(fout,"%s\n",record.c_str());
		fclose(fout);
	}
}

//! Write log entry - fixed location
/*! Append the provided string to a file in the {node}/temp/{agent} directory. The file name
 * is created as {node}_yyyyjjjsssss_{extra}.{type}
 * \param node Node name.
 * \param agent Agent name.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param extra Extra part  of name.
 * \param type Type part of name.
 * \param record String to be appended to file.
 */
void log_write(string node, string agent, double utc, string extra, string type, string record)
{
	log_write(node, "temp", agent, utc, extra, type, record);
	//	FILE *fout;
	//	string path;

	//	if (utc == 0.)
	//		return;

	//	path = data_type_path(node, "temp", agent, utc, extra, type);

	//	if ((fout = data_open(path, (char *)"a+")) != nullptr)
	//	{
	//		fprintf(fout,"%s\n",record.c_str());
	//		fclose(fout);
	//	}
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
void log_write(string node, string agent, double utc, string type, const char *record)
{
	log_write(node, "temp", agent, utc, "", type, record);
	//	FILE *fout;
	//	string path;

	//	if (utc == 0.)
	//		return;

	//	path = data_type_path(node, "temp", agent, utc, type);

	//	if ((fout = data_open(path, (char *)"a+")) != nullptr)
	//	{
	//		fprintf(fout,"%s\n",record);
	//		fclose(fout);
	//	}
}

//! Write log entry - fixed location, no extra, integer type and agent
/*! Append the provided string to a file in the {node}/temp/{agent_name} directory. The file name
 * is created as {node}_yyyyjjjsssss.{type_name}
 * \param node Node name.
 * \param type Integer specifying what type and agent of file.
 * \param utc UTC to be converted to year (yyyy), julian day (jjj) and seconds (sssss).
 * \param record String to be appended to file.
 */
void log_write(string node, int type, double utc, const char *record)
{
	//	FILE *fout;
	//	string path;

	//	if (utc == 0.)
	//		return;

	switch (type)
	{
	case DATA_LOG_TYPE_SOH:
		log_write(node, "temp", "soh", utc, "", "telemetry", record);
		//		path = data_type_path(node, "temp", "soh", utc, "telemetry");
		break;
	case DATA_LOG_TYPE_EVENT:
		log_write(node, "temp", "soh", utc, "", "event", record);
		//		path = data_type_path(node, "temp", "soh", utc, "event");
		break;
	case DATA_LOG_TYPE_BEACON:
		log_write(node, "temp", "beacon", utc, "", "beacon", record);
		//		path = data_type_path(node, "temp", "beacon", utc, "beacon");
		break;
	default:
		log_write(node, "temp", "soh", utc, "", "log", record);
		//		path = data_type_path(node, "temp", "soh", utc, "log");
		break;
	}

	//	if ((fout = data_open(path, (char *)"a+")) != nullptr)
	//	{
	//		fprintf(fout,"%s\n",record);
	//		fclose(fout);
	//	}
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
 */
void log_move(string node, string agent, string srclocation, string dstlocation, bool compress)
{
	char buffer[8192];
	vector<filestruc> oldfiles;
	data_list_files(node, srclocation, agent, oldfiles);
	for (auto oldfile: oldfiles)
	{
		string oldpath = oldfile.path;

		if (compress)
		{
			string temppath = oldfile.path + ".gz";
			string newpath = data_base_path(node, dstlocation, agent, oldfile.name + ".gz");
			FILE *fin = data_open(oldpath, (char *)"rb");
			FILE *fout = data_open(temppath, (char *)"wb");
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
			rename(temppath.c_str(), newpath.c_str());
			remove(temppath.c_str());
		}
		else
		{
			string newpath = data_base_path(node, dstlocation, agent, oldfile.name);
			rename(oldpath.c_str(), newpath.c_str());
		}
		remove(oldpath.c_str());
	}
}

//! Move log file - short version.
/*! Move files previously created with ::log_write to their final location.
 * The short version assumes a source location of "temp" and a destination
 * locations of "outgoing". The routine will find all files currently in
 * {node}/temp/{agent} and move them to {node}/outgoing/{agent}.
 * \param node Node name.
 * \param agent Agent name.
 */
void log_move(string node, string agent)
{
	log_move(node, agent, "temp", "outgoing", true);
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
							//							struct stat st;
							//							stat(dpath.c_str(), &st);
							//							if (S_ISDIR(st.st_mode))
							if (data_isdir(dpath))
							{
								double jday = atof(td->d_name);
								double mjd = cal2mjd2((int)year, 1, 0.) + jday;
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

//! Get a list of files in a Node archive.
/*! Generate a list of archived files for the indicated Node, Agent, and UTC.
 * The result is returned as a vector of ::filestruc, one entry for each file found.
 * \param node Node to search.
 * \param agent Subdirectory of location to search.
 * \param utc Date in archive as MJD.
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
						files.push_back(tf);
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
 * \return Number of files found, otherwise negative error.
 */
size_t data_list_files(string node, string location, string agent, vector<filestruc>& files)
{
	string dtemp;
	DIR *jdp;
	struct dirent *td;
	filestruc tf;

	tf.node = node;
	tf.agent = agent;
	dtemp = data_base_path(node, location, agent);
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
			}
		}
		closedir(jdp);
	}

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
	//	struct stat statbuf;

	int32_t iretn = get_cosmosnodes(rootd);
	if (iretn < 0)
	{
		return (iretn);
	}

	dtemp = rootd;
	if ((jdp=opendir(dtemp.c_str())) != nullptr)
	{
		while ((td=readdir(jdp)) != nullptr)
		{
			//			if (td->d_name[0] != '.' && !stat(((dtemp+"/")+td->d_name).c_str(), &statbuf) && S_ISDIR(statbuf.st_mode))
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

//! Get vector of Node structures.
/*! Scan the COSMOS root directory and return a ::cosmosstruc for each
 * Node that is found.
 * \param node Vector of ::cosmosstruc for each Node.
 * \return Zero or negative error.
 */
int32_t data_get_nodes(vector<cosmosstruc> &node)
{
	DIR *jdp;
	string dtemp;
	string rootd;
	struct dirent *td;
	cosmosstruc *tnode;

	int32_t iretn = get_cosmosnodes(rootd);
	if (iretn < 0)
	{
		return (iretn);
	}

	if ((tnode=json_create()) == nullptr)
	{
		return (NODE_ERROR_NODE);
	}

	dtemp = rootd;
	if ((jdp=opendir(dtemp.c_str())) != nullptr)
	{
		while ((td=readdir(jdp)) != nullptr)
		{
			if (td->d_name[0] != '.')
			{
				if (!node_init(td->d_name,tnode))
				{
					node.push_back(*tnode);
				}
			}
		}
		closedir(jdp);
	}
	return 0;
}

//! Create data file name
/*! Builds a filename up from the date of creation and its type. Format is:
*	yyyyjjjsssss.type, where yyyy is the Year, jjj is the Julian Day, sssss is
*	the Seconds, and type is any accepted COSMOS file type (eg. log, event,
*	telemetry, message, command.)
*	\param mjd UTC of creation date in Modified Julian Day
*	\param type Any valid extension type
*	\return Filename string, otherwise nullptr
*/
string data_name(string node, double mjd, string extra, string type)
{
	string name;
	char ntemp[100];

	int year, month, seconds;
	double jday, day;

	mjd2ymd(mjd,&year,&month,&day,&jday);
	seconds = (int)(86400.*(jday-(int)jday));
	sprintf(ntemp,"_%04d%03d%05d",year,(int32_t)jday,seconds);
	if (extra.empty())
	{
		name = node + ntemp + "." + type;
	}
	else
	{
		name = node + ntemp + "_" + extra + "." + type;
	}
	return (name);
}

string data_name(string node, double mjd, string type)
{
	return data_name(node, mjd, "", type);
}

//! Get date from file name.
/*! Assuming the COSMOS standard filename format from ::data_name, extract
 * the date portion and return it as year, julian day and seconds.
 * \param name File name.
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
 * \param name File name.
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
		utc = cal2mjd2(year, 1, seconds/86400.) + jday;
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
			path = tpath;
		}
		else
		{
			tpath += "/" + agent;
			if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
			{
				path = tpath;
			}
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
		tpath += "/" + location;
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
	string path;

	int32_t iretn = get_cosmosnodes(tpath);
	if (iretn >= 0)
	{
		tpath += "/" + node;

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
	char ntemp[COSMOS_MAX_NAME];
	string path;

	tpath = data_base_path(node, "data", agent);
	if (!tpath.empty())
	{
		int year, month;
		double jday, day;
		mjd2ymd(mjd,&year,&month,&day,&jday);
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
*	\param mjd UTC of creation date in Modified Julian Day
*	\param type Any valid extension type
*	\return File path string, otherwise nullptr
*/
string data_type_path(string node, string location, string agent, double mjd, string type)
{
	string path;

	path = data_type_path(node, location, agent, mjd, "", type);

	return (path);
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
string data_type_path(string node, string location, string agent, double mjd, string extra, string type)
{
	string path;
	string tpath;

	tpath = data_name_path(node, location, agent, mjd, data_name(node, mjd, extra, type));

	if (!tpath.empty())
	{
		path = tpath;
	}
	return path;
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
 * \mode fopen style mode
 * \return fopen style file handle, or nullptr if either a directory element can not be
 * created, or the file can not be opened.
 */

FILE *data_open(string path, char *mode)
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

	if (dindex < length)
	{
		for (index=0; index<=dindex; ++index)
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

//! Set Resources Directory
/*! Set the internal variable that points to where all COSMOS resource files
 * are stored.
	\param name Absolute or relative pathname of directory.
	\return Zero, or negative error.
*/
int32_t set_cosmosresources(string name)
{
	if (data_isdir(name))
	{
		cosmosresources = name;
		return 0;
	}
	else
	{
		cosmosresources.clear();
		return DATA_ERROR_RESOURCES_FOLDER;
	}
}

//! Find Resources Directory
/*! Set the internal variable that points to where all COSMOS resource files
 * are stored. This either uses the value in COSMOSRESOURCES, or looks for the directory
 * up to 6 levels above the current directory, first in "cosmosresources", and then in "resources".
 * \return Zero, or negative error.
*/
int32_t set_cosmosresources()
{
	string aroot;
	int i;
	struct stat sbuf;

	if (cosmosresources.empty())
	{
		char *croot = getenv("COSMOSRESOURCES");
		if (croot != nullptr && data_isdir(croot))
		{
			cosmosresources = croot;
		}
		else
		{
			aroot = "cosmosresources";
			for (i=0; i<6; i++)
			{
				if (stat(aroot.c_str(),&sbuf) == 0)
				{
					cosmosresources = aroot;
					break;
				}
				aroot = "../" + aroot;
			}
			if (cosmosresources.empty())
			{
				aroot = "resources";
				for (i=0; i<6; i++)
				{
					if (stat(aroot.c_str(),&sbuf) == 0)
					{
						cosmosresources = aroot;
						break;
					}
					aroot = "../" + aroot;
				}
			}
		}
	}

	// if cosmosresources is still empty then fail the program and inform the user
	if (cosmosresources.empty())
	{
		cerr << "error " << DATA_ERROR_RESOURCES_FOLDER << ": could not find cosmos/resources folder" << endl;
		return (DATA_ERROR_RESOURCES_FOLDER);
	}

	return 0;
}

//! Return COSMOS Resources Directory
/*! Get the internal variable that points to where all COSMOS Resource files are
 * stored. Initialize variable if this is the first call to the function.
 * \param result Full path to Resources directory.
 * \return Length of string, otherwise negative error.
*/
int32_t get_cosmosresources(string &result)
{
	int32_t iretn;

	result.clear();
	if (cosmosresources.empty())
	{
		iretn = set_cosmosresources();
		if (iretn < 0)
		{
			// if cosmosresources is still empty then fail the program and inform the user
			cerr << "error " << DATA_ERROR_RESOURCES_FOLDER << ": could not find cosmos/resources folder" << endl;
			return (DATA_ERROR_RESOURCES_FOLDER);
		}
	}
	result = cosmosresources;
	return 0;
}

//! Set Nodes Directory
/*! Set the internal variable that points to where all COSMOS resource files
 * are stored.
	\param name Absolute or relative pathname of directory.
	\return Zero, or negative error.
*/
int32_t set_cosmosnodes(string name)
{
	if (data_isdir(name))
	{
		cosmosnodes = name;
		return 0;
	}
	else
	{
		cosmosnodes.clear();
		return DATA_ERROR_RESOURCES_FOLDER;
	}
}

//! Find Nodes Directory
/*! Set the internal variable that points to where all COSMOS node files
 * are stored. This either uses the value in COSMOSNODES, or looks for the directory
 * up to 6 levels above the current directory, first in "cosmosnodes", and then in "nodes".
 * \return Zero, or negative error.
*/
int32_t set_cosmosnodes()
{
	string aroot;
	int i;

	if (cosmosnodes.empty())
	{
		char *croot = getenv("COSMOSNODES");
		if (croot != nullptr && data_isdir(croot))
		{
			cosmosnodes = croot;
		}
		else
		{
			aroot = "cosmosnodes";
			for (i=0; i<6; i++)
			{
				if (data_isdir(aroot))
				{
					cosmosnodes = aroot;
					break;
				}
				aroot = "../" + aroot;
			}
			if (cosmosnodes.empty())
			{
				aroot = "nodes";
				for (i=0; i<6; i++)
				{
					if (data_isdir(aroot))
					{
						cosmosnodes = aroot;
						break;
					}
					aroot = "../" + aroot;
				}
			}
		}
	}

	if (cosmosnodes.empty())
	{
		cerr << "error " << DATA_ERROR_NODES_FOLDER << ": could not find cosmos/nodes folder" << endl;
		return (DATA_ERROR_NODES_FOLDER);
	}
	else
	{
		return 0;
	}
}

//! Get COSMOS Nodes Directory
/*! Get the internal variable that points to where all COSMOS files are
 * stored.
 * \param result String to place path in.
 * \return Zero, or negative error.
*/
int32_t get_cosmosnodes(string &result)
{
	int32_t iretn;

	result.clear();
	if (cosmosnodes.empty())
	{
		iretn = set_cosmosnodes();
		if (iretn < 0)
		{
			cerr << "error " << DATA_ERROR_NODES_FOLDER << ": could not find cosmos/nodes folder" << endl;
			return (DATA_ERROR_NODES_FOLDER);
		}
	}

	result = cosmosnodes;
	return 0;
}

//! Get Current Node Directory
/*! Get the internal variable that points to where node files are
 * stored for the current Node.
 * \return Pointer to character string containing path to Node, otherwise nullptr.
*/
string get_nodedir(string name)
{
	return (set_nodedir(name));
}

//! Set Current Node Directory
/*! Set the internal variable that points to where node files are
 * stored for the current Node.
 * \param node Name of current Node
 * \return Pointer to character string containing path to Node, otherwise nullptr.
*/
string set_nodedir(string node)
{
	nodedir.clear();
	if (!set_cosmosnodes())
	{
		nodedir = cosmosnodes + "/" + node;
		if (!data_isdir(nodedir))
		{
			nodedir.clear();
		}
	}
	return (nodedir);
}

//! Load data from archive
/*! Load JSON entries of specified type from data archive for specified Node and Agent.
 * Will return all data that is available within specified date range, in files
 * {COSMOSNODES}/{Node}/date/{Agent}/{yyyy}/{ddd}/{*}.type.
 * \param node Name of Node.
 * \param Agent Name of Agent.
 * \param utcbegin Starting UTC.
 * \param utcend Ending UTC.
 * \param type Type extension.
 * \param result Vector of JSON strings.
 * \return 0 or negative error.
 */
int32_t data_load_archive(string node, string agent, double utcbegin, double utcend, string type, vector<string> &result)
{
	ifstream tfd;
	string tstring;
	vector <filestruc> files;


	result.clear();

	for (double mjd = floor(utcbegin); mjd <= floor(utcend); ++mjd)
	{
		files = data_list_archive(node, agent, mjd, type);
		for (filestruc file : files)
		{
			if ((mjd == floor(utcbegin) && file.seconds/86400. < utcbegin-mjd) || (mjd == floor(utcend) && file.seconds/86400. > utcend-mjd))
			{
				continue;
			}
			tfd.open(file.path);
			if (tfd.is_open())
			{
				while (getline(tfd,tstring))
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
	int32_t iretn;
	iretn = data_load_archive(node, agent, floor(mjd), floor(mjd)+.999999, type, result);
	return iretn;
}

int32_t data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = data_load_archive(cdata[0].node.name, "soh", mjd, "telemetry", telem);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = data_load_archive(cdata[0].node.name, "soh", mjd, "event", event);

//	DIR *jdp;
//	struct dirent *td;
//	int year, month;
//	double day, jday;
//	int len, dlen;
//	char dtemp[356];
//	ifstream tfd;
//	string tstring;

//	telem.clear();
//	event.clear();

//	mjd = (int)mjd;
//	mjd2ymd(mjd,&year,&month,&day,&jday);

//	get_nodedir(cdata[0].node.name);
//	if (nodedir.size())
//	{
//		dlen = nodedir.size() + 33 + strlen(cdata[0].node.name);
//		sprintf(dtemp,"%s/data/soh/%4d/%03d",nodedir.c_str(),year,(int32_t)jday);
//		if ((jdp=opendir(dtemp))!=nullptr)
//		{
//			while ((td=readdir(jdp))!=nullptr)
//			{
//				if (td->d_name[0] != '.')
//				{
//					sprintf(dtemp,"%s/data/soh/%04d/%03d/%s",nodedir.c_str(),year,(int32_t)jday,td->d_name);
//					if (((len=strlen(dtemp)) > dlen))
//						tfd.open(dtemp);
//					if (tfd.is_open())
//					{
//						while (getline(tfd,tstring))
//						{
//							switch (dtemp[dlen])
//							{
//							//! Telemetry file
//							case 't':
//								if (!strcmp(&dtemp[dlen],"telemetry"))
//								{
//									telem.push_back(tstring);
//								}
//								break;
//								//! Event file
//							case 'e':
//								//! Log file
//							case 'l':
//								//! Command file
//							case 'c':
//								//! Message file
//							case 'm':
//								if (!strcmp(&dtemp[dlen],"event") || !strcmp(&dtemp[dlen],"log") || !strcmp(&dtemp[dlen],"command") || !strcmp(&dtemp[dlen],"message"))
//								{
//									event.push_back(tstring);
//								}
//								break;
//							}
//						}
//						tfd.close();
//					}
//				}
//			}
//			closedir(jdp);

//			return 0;
//		}
//	}
//	return (DATA_ERROR_ARCHIVE);
	return iretn;
}

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
		mytime += (int)((jday-1) * 86400.);
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
		return cal2mjd2(year,mytm.tm_mon+1,mytm.tm_mday);
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

		return (cal2mjd2(year,mytm.tm_mon+1,mytm.tm_mday));
	}
	else
	{
		return 0.;
	}
}

//! Add to KML path
/*! Write a KML file to keep track of the path the node is following. Create the file if it doesn't alreay exist.
 * Append to it if it already exists.
 \param cdata COSMOS data structure.
 \param name Name of file to write.
 \return 0, otherwise negative error.
*/
int32_t kml_write(cosmosstruc *cdata)
{
	char buf[500];
	FILE *fin, *fout;
	double utc;

	utc = floor(cdata[0].node.loc.utc);

	string path = data_type_path((string)cdata[0].node.name, "outgoing", "google", utc, "points");
	fin = data_open(path, (char *)"a+");
	fprintf(fin,"%.5f,%.5f,%.5f\n",DEGOF(cdata[0].node.loc.pos.geod.s.lon),DEGOF(cdata[0].node.loc.pos.geod.s.lat),cdata[0].node.loc.pos.geod.s.h);

	path = data_type_path(cdata[0].node.name,(char *)"outgoing",(char *)"google",  utc,(char *)"kml");
	fout = data_open(path, (char *)"w");
	fprintf(fout,"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
	fprintf(fout,"<Document>\n");
	fprintf(fout,"<name>%s JD%5.0f</name>\n",cdata[0].node.name,utc);
	fprintf(fout,"<description>Track of node.</description>\n");
	fprintf(fout,"<Style id=\"yellowLineGreenPoly\">\n<LineStyle>\n<color>7f00ffff</color>\n<width>4</width>\n</LineStyle>\n");
	fprintf(fout,"<PolyStyle>\n<color>7f00ff00</color>\n</PolyStyle>\n</Style>\n");
	fprintf(fout,"<Placemark>\n<name>Node Path</name>\n<description>%s JD%5.0f</description>\n",cdata[0].node.name,utc);
	fprintf(fout,"<styleUrl>#yellowLineGreenPoly</styleUrl>\n<LineString>\n<extrude>1</extrude>\n<tessellate>1</tessellate>\n<altitudeMode>absolute</altitudeMode>\n");
	fprintf(fout,"<coordinates>\n");

	rewind (fin);
	while (fgets(buf, 500, fin) != nullptr)
	{
		fputs(buf, fout);
	}
	fclose(fin);

	fprintf(fout,"</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n");
	fclose(fout);

	return 0;
}

bool data_isdir(string path)
{
	struct stat st;

	if (!stat(path.c_str(), &st) && S_ISDIR(st.st_mode))
	{
		return true;
	}
	else
	{
		return false;
	}

}

//! @}
