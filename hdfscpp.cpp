#include "hdfscpp.h"

#include <string>
#include <assert.h>

/**********************************************************************/

using namespace tmacam::hdfs;

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

// vim: et ai sts=4 ts=4 sw=4