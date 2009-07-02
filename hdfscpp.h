#ifndef HDFSCPP_H_
#define HDFSCPP_H_

#include <hdfs.h>
#include <stdexcept>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// -- from google c++ style guide
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  TypeName& operator=(const TypeName&)

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




}; // namespace hdfs
}; // namespace tmacam







#endif // HDFSCPP_H_
// vim: et ai sts=4 ts=4 sw=4
