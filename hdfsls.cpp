#include <iostream>
#include <hdfs.h>

#include <stdexcept>
#include <assert.h>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  TypeName& operator=(const TypeName&)

/**********************************************************************/

namespace tmacam {
namespace hdfs {

/*** Generic class to signal errors from HDFS run-time
 */
class HDFSError : public std::runtime_error {
    // I could try to get some information about the cause of the error
    // from errno but in most cases is just a EINTERNAL, so, really, who cares?
public:
    HDFSError(const std::string& arg) : std::runtime_error(arg) {};
};


class FileInfoList {
public:
    FileInfoList();
    ~FileInfoList();

    //! Implicitly act as a hdfsFileInfo pointer
    inline operator hdfsFileInfo*() { return files_info_ ;};

    inline hdfsFileInfo* operator->() { return files_info_ ;};

    inline int size() const {return n_;};

    inline int empty() const {return 0 == n_;};

    /**Forcibly deletes the managed hdfsFileInfo array
     *  @param  file_info  new hdfsFileInfo array to manage
     *  @param  num_entries  number of entries in this hdfsFileInfo
     *
     *  This object now @e owns the object pointed to by @a file_info.  The
     *  previous object has been deleted.
     */
    void reset(hdfsFileInfo* file_info=NULL, int num_entries=0);
private:
    int n_;
    hdfsFileInfo* files_info_;
    DISALLOW_COPY_AND_ASSIGN(FileInfoList);
};

/***RAII-wrapper for hdfsFS handles
 */
class FileSystem {
public:
    //!Default constructor
    FileSystem(const char* host = "default", tPort port = 0);

    ~FileSystem();

    //! Implicitly act as a hdfsFS handle
    inline operator hdfsFS() { return fs_ ;};

    void GetPathInfo(const char* path, FileInfoList* info);

    void ListDirectory(const char* path, FileInfoList* info);

private:
    hdfsFS fs_;
    DISALLOW_COPY_AND_ASSIGN(FileSystem);
};

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

}; // namespace hdfs
}; // namespace tmacam




/**********************************************************************/

void ShowUsage()
{
    std::cerr << "Usage: hdfsls <path> " << std::endl;
}

int main(int argc, char* argv[])
{
    using namespace tmacam;

    // Check command line arguments
    if ( argc != 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        ShowUsage();
        exit(EXIT_FAILURE);
    }
    const char* path = argv[1];

    // Connect to HDFS / get filesystem handle
    hdfs::FileSystem fs; 

    // Path is a directory, right?
    hdfs::FileInfoList path_info;
    fs.GetPathInfo(path, &path_info);
    if (path_info->mKind != kObjectKindDirectory ) {
        std::cerr << "Path '" << path << "' is not a directory." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    hdfs::FileInfoList files;
    fs.ListDirectory(path, &files);
    if (files.empty()) {
        std::cout << "Directory is empty" << std::endl;
    }
    for (int i = 0; i < files.size(); ++i) {
        std::cout << char(files[i].mKind)  << "\t" 
                  << files[i].mSize  << "\t" 
                  << files[i].mName
                  << std::endl;
    }

	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
