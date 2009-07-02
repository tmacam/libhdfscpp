#include "hdfscpp.h"

#include <string>
#include <assert.h>

/**********************************************************************/

using namespace tmacam::hdfs;

/**********************************************************************/


FileInfoList::FileInfoList():
    n_(0),
    files_info_(NULL)
{
}

FileInfoList::~FileInfoList()
{
    reset();
}


void FileInfoList::reset(hdfsFileInfo* entries, int num_entries)
{
    // Check for inconsistencies in provided parameters, i.e.,
    // Or you provide a both a valid files_info AND a non-zero num_entries
    // or you provide both NULL and 0.
    if ((entries != NULL) xor (num_entries != 0)) {
        throw std::runtime_error("Invalid parameters for FileInfoList::reset");
    }

    // Release previously held hdfsFileInfo
    if (files_info_) {
        hdfsFreeFileInfo(files_info_, n_);
        files_info_ = NULL;
        n_ = 0;
    }

    if (entries) {
        files_info_ = entries;
        n_ = num_entries;
    }
}

/**********************************************************************/


File::File(hdfsFS fs, const char* path,  int flags, int bufferSize,
        short replication, tSize blocksize) :
    fs_(fs)
{
    fh_ = hdfsOpenFile(fs_,path, flags, bufferSize, replication, blocksize);
    if (!fh_) {
        std::string msg("Failed open path '");
        msg += path;
        msg += "'.";
        throw HDFSError(msg);
    }
}


tSize File::Read(void* buffer, tSize length)
{
    tSize result = hdfsRead(fs_, fh_, buffer, length);
    if( result < 0 ) {
        throw HDFSError("An error ocurred reading the file (hdfsRead()).");
    }

    return result;
}

/**********************************************************************/

FileSystem::FileSystem(const char* host, tPort port) :
    fs_(hdfsConnect(host, port))
{
    if (!fs_) {
        throw HDFSError("Error during HDFS connect");
    }
}

FileSystem::~FileSystem() {
    if (fs_) {
        if (hdfsDisconnect(fs_) ) {
            throw HDFSError("Failed during hdfsDisconnect()");
        }
    }
}

void FileSystem::GetPathInfo(const char* path, FileInfoList* info)
{
    assert(info);

    // Path is a directory, right?
    hdfsFileInfo* path_info = hdfsGetPathInfo(fs_, path);
    if (!path_info) {
        std::string msg("Failed to get information about path '");
        msg += path;
        msg += "'.";
        throw HDFSError(msg);
    }
    info->reset(path_info, 1);
}

void FileSystem::ListDirectory(const char* path, FileInfoList* info)
{
    assert(info);
    int n_entries = 0;
	hdfsFileInfo* entries = hdfsListDirectory(fs_, path, &n_entries);
    if (!entries && errno) {
        std::string msg("Error getting information about files in path '");
        msg += path;
        msg += "'.";
        throw HDFSError(msg);
    }

    info->reset(entries, n_entries);
}


bool FileSystem::Exists(const char* path) 
{
    return bool(hdfsExists(fs_, path) == 0);
}




// vim: et ai sts=4 ts=4 sw=4
