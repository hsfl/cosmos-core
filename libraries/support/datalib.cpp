/*! \file datalib.cpp
	\brief Data support functions
*/

#include "datalib.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include "zlib/zlib.h"

#include <stdio.h>
//#ifdef COSMOS_WIN_BUILD_MSVC
//#include <filesystem> // it is being proposed for standardization for C++
//#elif
#include <dirent.h>
//#endif
#include <sys/stat.h>
#include <iostream>
#include <fstream>

//! \ingroup datalib
//! \defgroup datalib_statics Static variables for Data functions.
//! @{

//static vector<cosmosstruc> nodes;

//! Path to project Node directory
string nodebase;
//! Path to COSMOS Root directory
string cosmosbase;
//! Path to COSMOS Resource directory
string resdir;
//! Path to COSMOS Nodes directory
string nodedir;
//! Path to current COSMOS Node directory
string cnodedir;

//! @}

// MSVC lacks these POSIX macros and other compilers may too:
#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

//! \ingroup datalib
//! \defgroup datalib_functions Data Management support functions
//! @{

void log_write(string node, int type, double utc, const char *record)
{
	FILE *fout;
	string path;

	if (utc == 0.)
		return;

	switch (type)
	{
	case DATA_LOG_TYPE_SOH:
		path = data_type_path(node, "temp", "soh", utc, "telemetry");
		break;
	case DATA_LOG_TYPE_EVENT:
		path = data_type_path(node, "temp", "soh", utc, "event");
		break;
    case DATA_LOG_TYPE_BEACON:
        path = data_type_path(node, "temp", "beacon", utc, "beacon");
        break;
//    case DATA_LOG_TYPE_PROGRAM:
//        path = data_extra_type_path(node, "temp", "log", utc, "" , "log");
//        break;
	default:
		path = data_type_path(node, "temp", "soh", utc, "log");
		break;
	}

	if ((fout = data_open(path, (char *)"a+")) != NULL)
	{
		fprintf(fout,"%s\n",record);
		fclose(fout);
	}
}

void log_write(string node, string agent, double utc, string type, const char *record)
{
	FILE *fout;
	string path;

	if (utc == 0.)
		return;

	path = data_type_path(node, "temp", agent, utc, type);

	if ((fout = data_open(path, (char *)"a+")) != NULL)
	{
		fprintf(fout,"%s\n",record);
		fclose(fout);
	}
}

// MN: created new log write function to be able to write "extra" on the log file name, later should be merged with the other log functions?
void log_write(string node, string agent, double utc, string extra, string type, string record)
{
    FILE *fout;
    string path;

    if (utc == 0.)
        return;

    path = data_extra_type_path(node, "temp", agent, utc, extra, type);

    if ((fout = data_open(path, (char *)"a+")) != NULL)
    {
        fprintf(fout,"%s\n",record.c_str());
        fclose(fout);
    }
}

void log_move(string node, string agent)
{
	char buffer[8192];
	vector<filestruc> oldfiles;
	data_list_files(node, "temp", agent, oldfiles);
	for (auto oldfile: oldfiles)
	{
		string oldpath = oldfile.path;
		string temppath = oldfile.path + ".gz";
		string newpath = data_base_path(node, "outgoing", agent, oldfile.name + ".gz");
		FILE *fin = data_open(oldpath, (char *)"r");
		FILE *fout = data_open(temppath, (char *)"w");
		gzFile gzfout;
		gzfout = gzdopen(fileno(fout), "a");

		do
		{
			size_t nbytes = fread(buffer, 1, 8192, fin);
			if (nbytes)
			{
				gzwrite(gzfout, buffer, nbytes);
			}
		} while (!feof(fin));

		fclose(fin);
		gzclose_w(gzfout);
		rename(temppath.c_str(), newpath.c_str());
		remove(oldpath.c_str());
	}
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
int32_t data_list_files(string node, string location, string agent, vector<filestruc>& files)
{
	string dtemp;
	DIR *jdp;
	struct dirent *td;
	filestruc tf;

	tf.node = node;
	tf.agent = agent;
	dtemp = data_base_path(node, location, agent);
	if ((jdp=opendir(dtemp.c_str())) != NULL)
	{
		while ((td=readdir(jdp)) != NULL)
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
					for (uint16_t i=strlen(td->d_name)-1; i<strlen(td->d_name); --i)
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
	struct stat statbuf;

	rootd=get_nodebase();
	if (rootd.empty())
	{
		return (NODE_ERROR_ROOTDIR);
	}

	dtemp = rootd;
	if ((jdp=opendir(dtemp.c_str())) != NULL)
	{
		while ((td=readdir(jdp)) != NULL)
		{
			if (td->d_name[0] != '.' && !stat(((dtemp+"/")+td->d_name).c_str(), &statbuf) && S_ISDIR(statbuf.st_mode))
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

	rootd=get_nodebase();
	if (rootd.empty())
	{
		return (NODE_ERROR_ROOTDIR);
	}

	if ((tnode=json_create()) == NULL)
	{
		return (NODE_ERROR_NODE);
	}

	dtemp = rootd;
	if ((jdp=opendir(dtemp.c_str())) != NULL)
	{
		while ((td=readdir(jdp)) != NULL)
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
*	\return Filename string, otherwise NULL
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

/*
char *data_name(char *node, double mjd, char *type)
{
	static char dtemp[100];

	int year, month, seconds;
	double jday, day;

	mjd2ymd(mjd,&year,&month,&day,&jday);
	seconds = (int)(86400.*(jday-(int)jday));
	sprintf(dtemp,"%s_%04d%03d%05d.%s",node,year,(int32_t)jday,seconds,type);
	return (dtemp);
}
*/

string data_base_path(string node, string location, string agent, string filename)
{
	string path = data_base_path(node, location, agent);
	if (!path.empty())
	{

		path += "/" + filename;
	}
	return path;

}

string data_base_path(string node, string location, string agent)
{
	string tpath;
	string path;

//	printf("Node: %s Location: %s Agent: %s\n", node.c_str(), location.c_str(), agent.c_str());

	tpath = get_nodebase();
	tpath += "/" + node;

	if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
	{

		tpath += "/" + location;
		if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
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
	}
	return path;

}

string data_base_path(string node, string location)
{
	string tpath;
	string path;

	tpath = get_nodebase();
	tpath += "/" + node;

	if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
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

	tpath = get_nodebase();
	tpath += "/" + node;

	if (COSMOS_MKDIR(tpath.c_str(),00777) == 0 || errno == EEXIST)
	{

		path = tpath;
	}
	return path;

}

string data_archive_path(string node, string location, string agent, double mjd, string filename)
{
	string path = data_archive_path(node, location, agent, mjd);
	if (!path.empty())
	{

		path += "/" + filename;
	}
	return path;

}

string data_archive_path(string node, string location, string agent, double mjd)
{
	string tpath;
	char ntemp[COSMOS_MAX_NAME];
	string path;

	tpath = data_base_path(node, location, agent);
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
*	\return File path string, otherwise NULL
*/
string data_type_path(string node, string location, string agent, double mjd, string type)
{
	string path;

	path = data_extra_type_path(node, location, agent, mjd, "", type);

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
 * \return File path string, otherwise NULL
*/
string data_extra_type_path(string node, string location, string agent, double mjd, string extra, string type)
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
 * \return File path string, otherwise NULL
*/
string data_name_path(string node, string location, string agent, double mjd, string name)
{
	string path;
	string tpath;

	if (location == "data")
	{
		tpath = data_archive_path(node, location, agent, mjd);
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
 * \return fopen style file handle, or NULL if either a directory element can not be
 * created, or the file can not be opened.
 */

FILE *data_open(string path, char *mode)
{
	char dtemp[1024];
	uint32_t index, dindex, length;
	FILE *tfd;

	length = path.size();
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
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
				if (mkdir(dtemp,00777))
#else
                if (COSMOS_MKDIR(dtemp))
#endif
				{
					if (errno != EEXIST)
						return (NULL);
				}
			}
		}
	}

	if ((tfd = fopen(path.c_str(),mode)) != NULL)
	{
		return (tfd);
	}

	return (NULL);
}

//! Set Root Directory
/*! Set the internal variable that points to where all COSMOS files
 * are stored.
	\param name Absolute or relative pathname of directory.
*/
void set_cosmosbase(string name)
{
	cosmosbase = name;
}

//! Get COSMOS Base Directory
/*! Get the internal variable that points to where all COSMOS files are
 * stored.
 * \return Pointer to character string containing path to Root, otherwise NULL.
*/
string get_cosmosbase()
{
	string troot;
	string dir1;
	int i;
	struct stat sbuf;

	if (cosmosbase.empty())
	{
        char *croot = getenv("COSMOSRESOURCES");
		if (croot != NULL)
		{
			cosmosbase = croot;
		}
		else
		{
			troot = "../resources";
			for (i=0; i<6; i++)
			{
				dir1 = troot;
				if (stat(dir1.c_str(),&sbuf) == 0)
				{
					cosmosbase = troot;
					break;
				}
				troot = "../" + troot;
			}
		}

		if (!cosmosbase.empty())
		{
			resdir = cosmosbase;
		}
	}

	return (cosmosbase);
}

//! Get COSMOS Base Directory
/*! Get the internal variable that points to where all COSMOS files are
 * stored.
 * \return Pointer to character string containing path to Root, otherwise NULL.
*/
string get_nodebase()
{
	string troot;
	string dir1;
	int i;
	struct stat sbuf;

	if (nodebase.empty())
	{
        char *croot = getenv("COSMOSNODES");
		if (croot != NULL)
		{
			nodebase = croot;
		}
		else
		{
            troot = "../nodes";
			for (i=0; i<6; i++)
			{
				dir1 = troot;
				if (stat(dir1.c_str(),&sbuf) == 0)
				{
					nodebase = troot;
					break;
				}
				troot = "../" + troot;
			}
		}

		if (!nodebase.empty())
		{
			nodedir = nodebase;
		}
	}

	return (nodebase);
}

//! Set Resource Directory
/*! Set the internal variable that points to where resource files are stored for the various modeling
 * functions.
	\param name Absolute or relative pathname of directory.
*/
void set_resdir(char *name)
{
	resdir = name;
}

//! Get Resource Directory
/*! Get the internal variable that points to where resource files are
 * stored for the various modeling functions.
 * \return Pointer to character string containing path to Resources, otherwise NULL.
*/
string get_resdir()
{

	if (resdir.empty())
		get_cosmosbase();

	return (resdir);
}

//! Get Node Directory
/*! Get the internal variable that points to where node files are
 * stored for the various modeling functions.
 * \return Pointer to character string containing path to Nodes, otherwise NULL.
*/
string get_nodedir()
{
	if (nodedir.empty())
		get_nodebase();

	return (nodedir);
}

//! Get Current Node Directory
/*! Get the internal variable that points to where node files are
 * stored for the current Node.
 * \return Pointer to character string containing path to Node, otherwise NULL.
*/
string get_cnodedir(string name)
{
	if (nodedir.empty())
		get_nodebase();

	return (set_cnodedir(name));
}

//! Set Current Node Directory
/*! Set the internal variable that points to where node files are
 * stored for the current Node.
 * \param node Name of current Node
 * \return Pointer to character string containing path to Node, otherwise NULL.
*/
string set_cnodedir(string node)
{
	struct stat sbuf;

	if (nodedir.empty())
		get_nodebase();

	cnodedir = nodedir + "/" + node;
	if (stat(cnodedir.c_str(),&sbuf) != 0)
	{
		cnodedir.clear();
	}
	return (cnodedir);
}


int32_t data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc *cdata)
{
	DIR *jdp;
	struct dirent *td;
	int year, month;
	double day, jday;
	int len, dlen;
	char dtemp[356];
	ifstream tfd;
	string tstring;

	telem.clear();
	event.clear();

	mjd = (int)mjd;
	mjd2ymd(mjd,&year,&month,&day,&jday);

	get_cnodedir(cdata->node.name);
	if (cnodedir.size())
	{
		dlen = cnodedir.size() + 29 + strlen(cdata->node.name);
		sprintf(dtemp,"%s/data/soh/%4d/%03d",cnodedir.c_str(),year,(int32_t)jday);
		if ((jdp=opendir(dtemp))!=NULL)
		{
			while ((td=readdir(jdp))!=NULL)
			{
				if (td->d_name[0] != '.')
				{
					sprintf(dtemp,"%s/data/soh/%4d/%03d/%s",cnodedir.c_str(),year,(int32_t)jday,td->d_name);
					if (((len=strlen(dtemp)) > dlen))
						tfd.open(dtemp);
					if (tfd.is_open())
					{
						while (getline(tfd,tstring))
						{
							switch (dtemp[dlen])
							{
							//! Telemetry file
							case 't':
								if (!strcmp(&dtemp[dlen],"telemetry"))
								{
									telem.push_back(tstring);
								}
								break;
								//! Event file
							case 'e':
								//! Log file
							case 'l':
								//! Command file
							case 'c':
								//! Message file
							case 'm':
								if (!strcmp(&dtemp[dlen],"event") || !strcmp(&dtemp[dlen],"log") || !strcmp(&dtemp[dlen],"command") || !strcmp(&dtemp[dlen],"message"))
								{
									event.push_back(tstring);
								}
								break;
							}
						}
						tfd.close();
					}
				}
			}
			closedir(jdp);

			return 0;
		}
	}
	return (DATA_ERROR_ARCHIVE);
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
	if (get_cnodedir(name).size())
	{
		sprintf(dtemp,"%s/data/soh", cnodedir.c_str());
		if ((jdp=opendir(dtemp))!=NULL)
		{
			while ((td=readdir(jdp))!=NULL)
			{
				if (td->d_name[0] != '.')
				{
					if (atol(td->d_name) > year)
						year = atol(td->d_name);
				}
			}
			closedir(jdp);
			sprintf(dtemp,"%s/data/soh/%04d",cnodedir.c_str(),year);
			if ((jdp=opendir(dtemp))!=NULL)
			{
				while ((td=readdir(jdp))!=NULL)
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
		if(temptm!=NULL)
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

	if (get_cnodedir(name).size())
	{
	year = jday = 9000;
	sprintf(dtemp,"%s/data/soh", cnodedir.c_str());
	if ((jdp=opendir(dtemp))!=NULL)
	{
		while ((td=readdir(jdp))!=NULL)
		{
			if (td->d_name[0] != '.')
			{
				if (atol(td->d_name) < year)
					year = atol(td->d_name);
			}
		}
		closedir(jdp);
		sprintf(dtemp,"%s/data/soh/%04d",cnodedir.c_str(),year);
		if ((jdp=opendir(dtemp))!=NULL)
		{
			while ((td=readdir(jdp))!=NULL)
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

	utc = floor(cdata->node.loc.utc);

	string path = data_type_path((string)cdata->node.name, "outgoing", "google", utc, "points");
	fin = data_open(path, (char *)"a+");
	fprintf(fin,"%.5f,%.5f,%.5f\n",DEGOF(cdata->node.loc.pos.geod.s.lon),DEGOF(cdata->node.loc.pos.geod.s.lat),cdata->node.loc.pos.geod.s.h);

	path = data_type_path(cdata->node.name,(char *)"outgoing",(char *)"google",  utc,(char *)"kml");
	fout = data_open(path, (char *)"w");
	fprintf(fout,"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
	fprintf(fout,"<Document>\n");
	fprintf(fout,"<name>%s JD%5.0f</name>\n",cdata->node.name,utc);
	fprintf(fout,"<description>Track of node.</description>\n");
	fprintf(fout,"<Style id=\"yellowLineGreenPoly\">\n<LineStyle>\n<color>7f00ffff</color>\n<width>4</width>\n</LineStyle>\n");
	fprintf(fout,"<PolyStyle>\n<color>7f00ff00</color>\n</PolyStyle>\n</Style>\n");
	fprintf(fout,"<Placemark>\n<name>Node Path</name>\n<description>%s JD%5.0f</description>\n",cdata->node.name,utc);
	fprintf(fout,"<styleUrl>#yellowLineGreenPoly</styleUrl>\n<LineString>\n<extrude>1</extrude>\n<tessellate>1</tessellate>\n<altitudeMode>absolute</altitudeMode>\n");
	fprintf(fout,"<coordinates>\n");

	rewind (fin);
	while (fgets(buf, 500, fin) != NULL)
	{
		fputs(buf, fout);
	}
	fclose(fin);

	fprintf(fout,"</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n");
	fclose(fout);

	return 0;
}
//! @}
