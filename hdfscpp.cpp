#include "hdfscpp.h"

#include <string>
#include <assert.h>


/**********************************************************************/

namespace tmacam {
namespace hdfs {


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


tSize File::Pread(tOffset position, void* buffer, tSize length)
{
    tSize result = hdfsPread(fs_, fh_, position, buffer, length);
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


/**********************************************************************/


void GetFileBlockLocations(hdfsFS fs, const char* path, tOffset start,
        tOffset size, BlockLocationList* blocks_location)
{
    BlockLocationList result;

    char*** blocks_info = hdfsGetHosts(fs, path, start, size);
    if(!blocks_info) {
        throw HDFSError("hdfsGetHosts failed.");
    }
    for(size_t b = 0; blocks_info[b]; ++b) {
        HostList hosts;
        for(size_t h = 0; blocks_info[b][h]; ++h) {
            hosts.push_back(std::string(blocks_info[b][h]));
        }
        result.push_back(hosts);
    }
    hdfsFreeHosts(blocks_info);

    // Return the output swapping data with the input blocks_info
    blocks_location->swap(result);

}


void ListDirectoryEntries(tmacam::hdfs::FileSystem* fs, const char* path, 
        std::vector<std::string>* files)
{
    using namespace tmacam;

    assert(files);
    files->clear();

    // We are dealing with a directory.. right? XXX check

    hdfs::FileInfoList info_list;
    fs->ListDirectory(path, &info_list);
    if (info_list.empty()) { 
        return;
    } else {
        files->reserve(info_list.size());
        for (int i = 0; i < info_list.size(); ++i) {
            files->push_back(info_list[i].mName);
        }
        std::sort(files->begin(), files->end());
    }
}

void ReadFullHDFSFile(FileSystem* fs, const char* path,
        std::vector<char>* file_data) 
{

	FileInfoList file_info;
	fs->GetPathInfo(path, &file_info);
	tSize file_size = file_info->mSize;
	std::vector<char> buffer(file_size);

	File file(*fs, path, O_RDONLY);
	tSize bytes_read = 0;
	while (bytes_read < file_size) {
		tSize read_length = 0;
        read_length = file.Read(&buffer[bytes_read], file_size - bytes_read);
		bytes_read += read_length;
	}
    // Replace file_data contents with the contents read from the file
	file_data->swap(buffer);
}


}; // namespace hdfs
}; // namespace tmacam


// vim: et ai sts=4 ts=4 sw=4
