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

/*!
 * \file datalib.h
 * \brief File I/O, ring buffers, and zlib-compressed data storage.
 * \ingroup cosmos_micro_agent_data
 * \defgroup cosmos_datalib Data
 *
 * Provides the COSMOS data-directory conventions (paths for nodes, agents,
 * logs, and resources) as well as ring-buffer helpers and wrappers for writing
 * zlib-compressed telemetry files. Required by agents that persist telemetry
 * or share data files between processes.
 */

#ifndef _DATALIB_H
#define _DATALIB_H

// COSMOS libs
#include "support/configCosmos.h"
//#include "support/cosmos-errno.h"
#include "zlib.h"
//#include "support/jsondef.h"

// C libs
//#include <sys/stat.h>
//#include <fstream>

#ifdef _MSC_BUILD
#include <dirent.h>
#else
#include <dirent.h>
#endif

/*! \file datalib.h
*	\brief Data Management header file
*/

//! \ingroup support
//! \defgroup datalib Data Management
//! Data Management.
//!
//! Data within COSMOS is managed in a heirarchical structure. At the highest level, separate
//! directories are provided for Nodal information (nodes), and for general software Resources (resources).
//! These two directories can be together in a single COSMOS directory, or in separate locations. Their locations can
//! be defined through the use of environment variables, or through default values. The order of assignment is as follows:
//! - If the environment variables COSMOSNODES or COSMOSRESOURCES exist, then the
//! directories specified by these variables will be used for nodes or resources.
//! - If either COSMOSNODES and/or COSMOSRESOURCES is not set, but the environment variable COSMOS is, then nodes or
//! resources will be located in ${COSMOS}/nodes or ${COSMOS}/resources.
//! - If no environment variable is found, default locations are used, specific to the OS
//! + Windows: c:/cosmos/nodes and c:/cosmos/resources
//! + Unix: /usr/local/cosmos/nodes and /usr/local/cosmos/resources
//! + MacOS: /Applications/cosmos/nodes and /Applications/cosmos/resources
//! - If these directories are not found, then the software will look up to four levels above the directory it is running
//! in for directories called "nodes" or "cosmosnodes" and "resources" or "cosmosresources".
//! Once this search is completed successfully, the nodes folder for the current Node will be stored internally as
//! "cosmosnodes" and the resources folder as "cosmosresources".
//!
//! Within "nodes", there is a directory specific to each Node. Each Node specific directory mirrors the Node:Agent
//! arrangement laid down in the \ref jsonlib. A series of initialization files are incldued, as well as a set of directories
//! for Temporary data (temp), Incoming data (incoming), Outgoing data (outgoing), and Archival data (data).
//!
//! The initialization files include:
//! - node.ini: Describes the Node type, number of pieces, number of Devices, and number of ports.
//! - pieces.ini: Describes each Piece, including the number of any related Device.
//! - devices_general.ini: Describes common elements of each Device.
//! - devices_specific.ini: Describes any unique of each Device.
//! - ports.ini: Describes Ports available to each device.
//! - target.ini: Provides a list of any targets associated with this Node.
//! - state.ini: Provides a complete state vector for this Node at some specific time.
//!
//! Each of temp, incoming and outgoing contain sub-directories named after each active Agent. Each agent sub-directory
//! in the "data" directory is further subdivided by first year, then day.
//!
//! The software resources directory contains directories of files for use in different
//! aspects of COSMOS. The directory "general" contains files of
//! coefficients used for the various models used in COSMOS simulations.
//! The directory "logo" contains any special images used by COSMOS
//! software. The directory "mapping" contains all the DEM's and
//! ancillary files used by the \ref demlib. Finally. the directory "qt"
//! contains files, such as UI forms, used by the various Qt based Tools
//! withing COSMOS.
//!
//! The functions in this library support path discovery and creation, the automatic generation
//! of standard names, and the automatic creation of log files.

//! \ingroup cosmos_datalib
//! \defgroup datalib_functions Data Management function declarations
//! @{

#define DATA_LOG_TYPE_SOH 0
#define DATA_LOG_TYPE_EVENT 1
#define DATA_LOG_TYPE_BEACON 2
#define DATA_LOG_TYPE_PROGRAM 3 // to log program status information while running

typedef struct
{
    string node;
    string agent;
    string name;
    string type;
    string directory;
    string path;
    off_t size;
    uint16_t year;
    uint16_t jday;
    uint32_t seconds;
    double utc;
} filestruc;

//! \brief Reopen the global log file after a rotation.
void log_reopen();

//! \brief Write a record to the node log file (legacy two-field form).
//! \param node Node name.
//! \param type Log type code (DATA_LOG_TYPE_*).
//! \param utc  MJD timestamp.
//! \param data Null-terminated record string.
//! \return Path of the log file written.
string log_write(string node, int type, double utc, const char* data);

//! \brief Write a record to a node/agent log file.
//! \param node  Node name. \param agent Agent name. \param utc MJD timestamp.
//! \param type  Log type string. \param data Null-terminated record string.
//! \return Path of the log file written.
string log_write(string node, string agent, double utc, string type, const char *data);

//! \brief Write a record to a node/agent log file with extra fields and location.
//! \param node     Node name. \param agent Agent name. \param utc MJD timestamp.
//! \param extra    Extra tag appended to the filename.
//! \param type     Log type string. \param record Log record string.
//! \param location Subdirectory within the node data tree; default "temp".
//! \return Path of the log file written.
string log_write(string node, string agent, double utc, string extra, string type, string record, string location="temp");

//! \brief Move completed log files for an agent from one location to another.
//! \param node        Node name. \param agent Agent name.
//! \param srclocation Source subdirectory (e.g. "temp").
//! \param dstlocation Destination subdirectory (e.g. "outgoing").
//! \param compress    If true, gzip files during move; default true.
//! \param age         Minimum file age in seconds before moving; default 60.
//! \return 0 on success, negative error code on failure.
int32_t log_move_agent_src(string node, string agent, string srclocation, string dstlocation, bool compress=true, float age=60.);

//! \brief Move completed temp log files for an agent to outgoing.
//! \param node  Node name. \param agent Agent name.
//! \param age   Minimum file age in seconds before moving; default 60.
//! \return 0 on success, negative error code on failure.
int32_t log_move_agent_temp(string node, string agent, float age=60.);

//! \brief Move a single log file to a new path, optionally compressing it.
//! \param oldpath  Source file path. \param newpath Destination file path.
//! \param compress If true, gzip the file; default true.
//! \param timeout  Maximum seconds to wait for a lock; default 10.
//! \return 0 on success, negative error code on failure.
int32_t log_move_file(string oldpath, string newpath, bool compress="true", double timeout=10.);

//! \brief Move all files in a source directory to a destination directory.
//! \param srcdir   Source directory path. \param dstdir Destination directory path.
//! \param compress If true, gzip files during move; default true.
//! \return 0 on success, negative error code on failure.
int32_t log_move_directory(string srcdir, string dstdir, bool compress="true");

//! \brief Read a line from an open gzipped log file.
//! \param file Open gzFile handle. \param num Maximum bytes to read.
//! \return Line string, or "" on EOF/error.
string log_read(gzFile &file, int num);

//! \brief Return a list of all node names found in the COSMOS nodes directory.
//! \return Vector of node name strings.
vector<string> data_list_nodes();

//! \brief Populate a vector with all node names found in the COSMOS nodes directory.
//! \param nodes Output vector.
//! \return Number of nodes found, or negative error code.
int32_t data_list_nodes(vector<string>& nodes);

//! \brief List all data files in a directory as filestruc entries.
//! \param directory Absolute path to scan.
//! \return Vector of filestruc descriptors.
vector<filestruc> data_list_files(string directory);

//! \brief Populate a vector with data files in a directory, up to \p limit entries.
//! \param directory Absolute path to scan. \param files Output vector. \param limit Maximum files; default UINT16_MAX.
//! \return Number of files found.
size_t data_list_files(string directory, vector<filestruc>& files, uint16_t limit=UINT16_MAX);

//! \brief List all data files in a node/location subdirectory.
//! \param node Node name. \param location Subdirectory (e.g. "incoming").
//! \return Vector of filestruc descriptors.
vector<filestruc> data_list_files(string node, string location);

//! \brief List all data files for a specific agent under node/location.
//! \param node Node name. \param location Subdirectory. \param agent Agent name.
//! \return Vector of filestruc descriptors.
vector<filestruc> data_list_files(string node, string location, string agent);

//! \brief Return the path of the most-recently-modified file for a node/location/agent.
//! \param node Node name. \param location Subdirectory. \param agent Agent name.
//! \return Absolute path string, or "" if none found.
string data_list_latest_file(string node, string location, string agent);

//! \brief Populate a vector with data files for a node/location/agent, up to \p limit entries.
//! \param node Node name. \param location Subdirectory. \param agent Agent name.
//! \param files Output vector. \param limit Maximum files; default UINT16_MAX.
//! \return Number of files found.
size_t data_list_files(string node, string location, string agent, vector<filestruc>& files, uint16_t limit=UINT16_MAX);

//! \brief List archived log files for a node/agent matching a given UTC and type.
//! \param node  Node name. \param agent Agent name.
//! \param utc   MJD of the archive day to search.
//! \param type  Log type string to match.
//! \return Vector of matching filestruc descriptors.
vector<filestruc> data_list_archive(string node, string agent, double utc, string type);

//! \brief List all archived log files for a node/agent on a given day.
//! \param node Node name. \param agent Agent name. \param utc MJD of the archive day.
//! \return Vector of filestruc descriptors.
vector<filestruc> data_list_archive(string node, string agent, double utc);

//! \brief Return a sorted list of MJD values representing days with archived data.
//! \param node  Node name. \param agent Agent name.
//! \return Sorted vector of MJD day values (time = 0.0 within each day).
vector<double> data_list_archive_days(string node, string agent);

//! \brief Open a data file, creating intermediate directories as needed.
//! \param path  Absolute file path.
//! \param mode  fopen-style mode string (e.g. "a", "wb").
//! \param dmode Directory permission bits; default 0777.
//! \return Open FILE pointer, or nullptr on failure.
FILE* data_open(string path, const char *mode, uint32_t dmode=00777);

//! \brief Parse a COSMOS-format filename to extract year, jday, and seconds.
//! \param node     Node name (used to locate the file). \param filename Filename to parse.
//! \param year     Output Gregorian year. \param jday Output day-of-year. \param seconds Output seconds-of-day.
//! \return 0 on success, negative error code on parse failure.
int32_t data_name_date(string node, string filename, uint16_t &year, uint16_t &jday, uint32_t &seconds);

//! \brief Parse a COSMOS-format filename to extract MJD.
//! \param node     Node name. \param filename Filename to parse. \param utc Output MJD.
//! \return 0 on success, negative error code on parse failure.
int32_t data_name_date(string node, string filename, double &utc);

//! \brief Generate a COSMOS-standard filename from timestamp and metadata.
//! \param mjd   MJD timestamp encoded into the filename.
//! \param type  Log type tag appended to the name; default "".
//! \param node  Node name; default "". \param agent Agent name; default "". \param extra Extra tag; default "".
//! \return COSMOS-format filename string.
string data_name(double mjd, string type="", string node="", string agent="", string extra="");

//! \brief Return the base data path for a node (nodes_dir/node_name).
string data_base_path(string node);

//! \brief Return the base path for a node/location subdirectory.
string data_base_path(string node, string location);

//! \brief Return the base path for a node/location/agent subdirectory.
string data_base_path(string node, string location, string agent);

//! \brief Return the absolute path of a specific file within the data tree.
//! \param node Node name. \param location Subdirectory. \param agent Agent name. \param filename File name.
//! \return Absolute path string.
string data_base_path(string node, string location, string agent, string filename);

//! \brief Return the archive directory path for a node/agent at a given MJD.
//! \param node Node name. \param agent Agent name. \param mjd MJD value (year and jday extracted).
//! \return Absolute archive directory path.
string data_archive_path(string node, string agent, double mjd);

//! \brief Return an absolute data file path from flexible parameters.
//! \param location Location subdirectory. \param mjd MJD encoded into the name; default 0.
//! \param type     Log type tag; default "". \param node Node name; default "".
//! \param agent    Agent name; default "". \param extra Extra tag; default "".
//! \return Absolute file path.
string data_path(string location, double mjd=0., string type="", string node="", string agent="", string extra="");

//! \brief Return the typed archive file path for a node/location/agent/time/type/extra.
string data_type_path(string node, string location, string agent, double mjd, string type, string extra="");

//! \brief Return an absolute path constructed from name rather than timestamp.
string data_name_path(string node, string location="", string agent="", double mjd=0., string name="");

//! \brief Return a fully-populated filestruc for a named file in the data tree.
filestruc data_name_struc(string node, string location="", string agent="", double mjd=0., string name="");

//! \brief Return the absolute path to a named file in the COSMOS resources directory.
//! \param name Relative resource path (e.g. "general/wgs84.dat").
//! \return Absolute path string.
string data_resource_path(string name);

//! \brief Return true if the path exists (file or directory).
//! \param path Path to test (updated to canonical form on success).
bool data_exists(string& path);

//! \brief Return true if \p path is a directory, optionally creating it.
//! \param path        Path to test. \param create_flag If true, create the directory if missing; default false.
bool data_isdir(string path, bool create_flag=false);

//! \brief Return true if \p path is a character device.
bool data_ischardev(string path);

//! \brief Return true if \p path is a block device.
bool data_isblkdev(string path);

//! \brief Return true if \p path is a symbolic link.
bool data_issymlink(string path);

//! \brief Return true if \p path is a regular file, optionally checking its size.
//! \param path Path to test. \param size If > 0, also require at least this many bytes; default 0.
bool data_isfile(string path, off_t size=0);

//! \brief Return the inode change time (ctime) of a file as MJD.
//! \param path File path.
//! \return ctime as MJD, or 0 on error.
double data_ctime(string path);

//! \brief Execute a shell command from a byte-vector command line, with a timeout.
//! \param cmd     Command bytes. \param timeout Max seconds to wait; default 10.
//! \return Exit code, or negative error code on timeout/failure.
int32_t data_execute(vector<uint8_t> cmd, float timeout=10.);

//! \brief Execute a shell command string, with a timeout.
//! \param cmd     Command string. \param timeout Max seconds to wait; default 10.
//! \return Exit code, or negative error code on timeout/failure.
int32_t data_execute(string cmd, float timeout=10.);

//! \brief Execute a shell command and capture stdout into \p result.
//! \param cmd    Command bytes. \param result Receives stdout text.
//! \param timeout Max seconds; default 10. \param shell Shell path; default "".
//! \return Exit code, or negative error code.
int32_t data_execute(vector<uint8_t> cmd, string& result, float timeout=10., string shell="");

//! \brief Execute a shell command string and capture stdout into \p result.
//! \param cmd    Command string. \param result Receives stdout text.
//! \param timeout Max seconds; default 10. \param shell Shell path; default "".
//! \return Exit code, or negative error code.
int32_t data_execute(string cmd, string& result, float timeout=10., string shell="");

//! \brief Spawn a background task, redirecting stdout to a file.
//! \param cmd     Command string. \param outpath Path for stdout; default "".
//! \param timeout Max seconds to wait for startup; default 60. \param shell Shell path; default "".
//! \return Process exit code, or negative error code.
int32_t data_task(string cmd, string outpath, float timeout=60., string shell="");

//! \brief Run a shell command with optional stdin/stdout/stderr redirection.
//! \param command_line Shell command. \param outpath stdout file; default "".
//! \param inpath stdin file; default "". \param errpath stderr file; default "".
//! \return Exit code, or negative error code.
int32_t data_shell(string command_line, string outpath="", string inpath="", string errpath="");

//! \brief Return the size of a file in bytes.
//! \param path File path.
//! \return File size, or negative error code if not found.
off_t data_size(string path);

//! \brief Set the COSMOS root directory to an explicit path.
//! \param name        Path to set. \param create_flag Create if missing; default false.
//! \return 0 on success, negative error code on failure.
int32_t set_cosmosroot(string name, bool create_flag=false);

//! \brief Discover and set the COSMOS root directory automatically.
//! \param create_flag Create if missing; default false.
//! \return 0 on success, negative error code on failure.
int32_t set_cosmosroot(bool create_flag=false);

//! \brief Retrieve the current COSMOS root directory path into \p result.
//! \param result      Output string. \param create_flag Create if missing; default false.
//! \return 0 on success, negative error code on failure.
int32_t get_cosmosroot(string &result, bool create_flag=false);

//! \brief Return the COSMOS root directory path as a string.
string get_cosmosroot(bool create_flag=false);

//! \brief Set the COSMOS resources directory to an explicit path.
int32_t set_cosmosresources(string name, bool create_flag);

//! \brief Discover and set the COSMOS resources directory automatically.
int32_t set_cosmosresources(bool create_flag=false);

//! \brief Retrieve the COSMOS resources directory path into \p result.
int32_t get_cosmosresources(string &result, bool create_flag=false);

//! \brief Return the COSMOS resources directory path as a string.
string get_cosmosresources(bool create_flag=false);

//! \brief Set the COSMOSRESOURCES environment variable to \p path.
int32_t setEnvCosmosResources(string path);

//! \brief Set the COSMOSNODES environment variable to \p path.
int32_t setEnvCosmosNodes(string path);

//! \brief Set an arbitrary environment variable to \p path.
//! \param var  Variable name. \param path Value to assign.
//! \return 0 on success, negative error code on failure.
int32_t setEnv(string var, string path);

//! \brief Set the COSMOS environment variable to \p path.
int32_t setEnvCosmos(string path);

//! \brief Set the COSMOS nodes directory to an explicit path.
int32_t set_cosmosnodes(string name, bool create_flag=false);

//! \brief Discover and set the COSMOS nodes directory automatically.
int32_t set_cosmosnodes(bool create_flag=false);

//! \brief Retrieve the COSMOS nodes directory path into \p result.
int32_t get_cosmosnodes(string &result, bool create_flag=false);

//! \brief Return the absolute path to a specific node's directory, nested under
//! that node's realm (<realmdir>/nodes/<node>) rather than a global node tree.
//! \param realm       Realm name. \param node Node name.
//! \param create_flag Create if missing; default false.
//! \return Absolute node directory path, or "" on failure.
string get_nodedir(string realm, string node, bool create_flag=false);

//! \brief Return the COSMOS nodes directory path as a string.
string get_cosmosnodes(bool create_flag=false);

//! \brief Set the COSMOS realms directory to an explicit path.
int32_t set_cosmosrealms(string name="", bool create_flag=false);

//! \brief Discover and set the COSMOS realms directory automatically.
int32_t set_cosmosrealms(bool create_flag=false);

//! \brief Retrieve the COSMOS realms directory path into \p result.
int32_t get_cosmosrealms(string &result, bool create_flag=false);

//! \brief Return the COSMOS realms directory path as a string.
string get_cosmosrealms(bool create_flag=false);

//! \brief Return the absolute path to a specific realm's directory.
//! \param realm       Realm name. \param create_flag Create if missing; default false.
//! \return Absolute realm directory path.
string get_realmdir(string realm, bool create_flag=false);

//! \brief Return the COSMOS archive root: $COSMOSARCHIVE, or <cosmosroot>/archive.
//! \param create_flag Create if missing; default false.
//! \return Absolute archive root path, or "" on failure.
string get_cosmosarchive(bool create_flag=false);

//! \brief Return an archive day directory, <archive>/<realm>/<node>/<kind>/<yyyy>/<ddd>.
//! \param realm Realm name. \param node Node name.
//! \param kind  "telemetry" or "data". \param mjd Time within the day (UTC).
//! \param create_flag Create the whole path if missing; default false.
//! \return Absolute directory path, or "" on failure.
string get_archivedir(string realm, string node, string kind, double mjd, bool create_flag=false);

//! \brief Return an archive file name, <prefix>_<yyyydddhhmmss>.<ext>.
//! \param prefix e.g. "telemetry", "event", "passive", "active".
//! \param mjd    Time of the file's first record (UTC). \param ext Extension without the dot.
string archive_name(string prefix, double mjd, string ext);

//! One archive file, from its name <prefix>_<yyyydddhhmmss>.<ext>
struct archivefile
{
    string kind;
    string prefix;
    //! Time of the file's first record or frame (UTC)
    double utc = 0.;
    off_t size = 0;
    string path;
};

//! \brief Parse an archive file name, <prefix>_<yyyydddhhmmss>.<ext>.
//! \return True if the name has that form.
bool archive_parse_name(const string &name, string &prefix, double &utc);

//! \brief List the archive files of one kind that may hold records in a time window, by time.
//! A telemetry file holds up to an hour from its name's time; a data file (listed by its
//! .hdr) up to 10 minutes.
//! \param realm Realm name. \param node Node name. \param kind "telemetry" or "data".
//! \param utcbegin Start of the window (UTC). \param utcend End of the window (UTC).
//! \param prefix Only files with this prefix (e.g. "event"); empty for all.
//! \return The files, sorted by time.
vector<archivefile> archive_list(string realm, string node, string kind, double utcbegin, double utcend, string prefix="");

//! \brief Load archived log records for an agent between two UTC values.
//! \param node     Node name. \param agent Agent name.
//! \param utcbegin Start of range (MJD, inclusive). \param utcend End of range (MJD, inclusive).
//! \param type     Log type string. \param result Output vector of record strings.
//! \return Number of records loaded, or negative error code.
int32_t data_load_archive(string node, string agent, double utcbegin, double utcend, string type, vector<string> &result);

//! \brief Load archived log records for an agent on a single UTC day.
//! \param node  Node name. \param agent Agent name. \param mjd MJD of the day.
//! \param type  Log type string. \param result Output vector of record strings.
//! \return Number of records loaded, or negative error code.
int32_t data_load_archive(string node, string agent, double mjd, string type, vector<string> &result);

//! \brief Move a filestruc-described file to a different location in the data tree.
//! \param file     Source file descriptor.
//! \param location Destination location subdirectory; default "outgoing".
//! \param compress If true, gzip the file; default true.
//! \return 0 on success, negative error code on failure.
int32_t data_move_file(filestruc file, string location="outgoing", bool compress=true);

//! \brief Move a file by path to a different location in the data tree.
int32_t data_move_file(string path, string location="outgoing", bool compress="true");

//! \brief Return the MJD of the last archived day for a node.
double findlastday(string realm, string node);

//! \brief Return the MJD of the first archived day for a node.
double findfirstday(string realm, string node);

//! \brief Return the current working directory as a string.
const string data_getcwd();

//! \brief Recursively delete a directory and all its contents.
//! \param dirpath Path to the directory to remove.
void rmdir(const string& dirpath);

//! \brief Delete all files and subdirectories within a directory without removing the directory itself.
//! \param dirpath Path to the directory to empty.
void rmdir_contents(const string& dirpath);

class DataLog
{
public:
    DataLog();
    int32_t Init(string node, string location="outgoing", string agent="self", string type="log", string extra="", double stride=0., bool fastmode=false, bool compress=true);
    int32_t Write(string& data);
    int32_t Write(vector<uint8_t>& data);
    int32_t Write(vector<uint8_t> data, string node, string location="outgoing", string agent="generic", string type="log", string extra="");
    int32_t Write(double utc, vector<uint8_t> data, string node, string agent="generic", string type="log", string extra="");
    int32_t Write(string data, string node, string agent="generic", string type="log", string extra="");
    int32_t Write(double utc, string data, string node, string location="outgoing", string agent="generic", string type="log", string extra="");
    int32_t SetStride(double seconds=0.);
    int32_t SetFastmode(bool state=false);
    int32_t SetStartdate(double mjd=0.);
    int32_t CloseIfStrideTime();

    string node;
    string location;
    string agent;
    string type;
    string extra;
    bool fastmode;
    bool compress;
    double stride;
    double enddate = 0.;
    double startdate = 0.;
    string path;
private:

    FILE* fout;
};

//! Writes JSON lines into the archive, one file per hour of record time:
//! <archive>/<realm>/<node>/telemetry/<yyyy>/<ddd>/<prefix>_<yyyydddhhmmss>.json, named by
//! the file's first record. A file of the same name is replaced.
class ArchiveLines
{
public:
    ArchiveLines(string realm, string node, string prefix);
    ArchiveLines(const ArchiveLines &) = delete;
    ArchiveLines &operator=(const ArchiveLines &) = delete;
    ArchiveLines(ArchiveLines &&other) noexcept;
    ~ArchiveLines();
    //! Write one line; starts a new file when mjd is past the current file's hour.
    //! \return Zero, or negative error.
    int32_t Write(double mjd, const string &line);
    void Close();

    string realm;
    string node;
    string prefix;
    //! Path of the open file, or empty
    string path;
private:
    FILE *fp = nullptr;
    double hourend = 0.;
};

//class NodeList
//{
//public:
//    //! A table of node_id:node_names, used in node-to-node communications
//    map<string, uint8_t> node_ids;
//    typedef uint8_t NODE_ID_TYPE;
//    int32_t load_node_ids(string realm="");
//    int32_t check_node_id(NODE_ID_TYPE node_id);
//    int32_t lookup_node_id(string node_name);
//    string lookup_node_id_name(NODE_ID_TYPE node_id);



//    static constexpr uint8_t NODEIDUNKNOWN = 0;
//    static constexpr uint8_t NODEIDORIG = 254;
//    static constexpr uint8_t NODEIDDEST = 255;
//};

class GITTEST	{
	public:
		void f();
};
//! @}

#endif // DATALIB_H
