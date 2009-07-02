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


/**RAII-wrapper for hdfsFile handles
 *
 * XXX Our constructor and the form we use this class is not consistent
 * with FileInfoList, i.e., there is no "uninitialized" state, you
 * cannot open files directly from FileSystem instances, etc. But, you
 * know what, this is way simpler and turning this into a auto_ptr like,
 * with reset() methods and such.
 *
 */
class File {
public:
    /**Default (and only) constructor.
     *
     * Open a hdfs file in given mode.
     *
     * @param fs The configured filesystem handle.
     * @param path The full path to the file.
     * @param flags Either O_RDONLY or O_WRONLY, for read-only or write-only.
     * @param bufferSize Size of buffer for read/write - pass 0 if you want
     * to use the default configured values.
     * @param replication Block replication - pass 0 if you want to use
     * the default configured values.
     * @param blocksize Size of block - pass 0 if you want to use the
     * default configured values.
     *
     * @throws HDFSError if we fail to open the file.
     */

    File(hdfsFS fs, const char* path,  int flags, int bufferSize=0,
            short replication=0, tSize blocksize=0);


    ~File() { hdfsCloseFile(fs_, fh_); }

    //! Implicitly act as a hdfsFile handle
    inline operator hdfsFile() { return fh_ ;};

    /**Read data from an open file.
     * @param buffer The buffer to copy read bytes into.
     * @param length The length of the buffer.
     * @return Returns the number of bytes actually read, possibly less
     * than than length.
     *
     * @throw HDFSError on error.
     */
    tSize Read(void* buffer, tSize length);

    /**Get the number of bytes that can be read from this
     * input stream without blocking.
     *
     * @return Returns available bytes; -1 on error. 
     */
    int Available() { return hdfsAvailable(fs_, fh_); }



private:
    hdfsFS fs_;
    hdfsFile fh_;

    DISALLOW_COPY_AND_ASSIGN(File);
    /** Default constructor is not available. Our only constructor is an
     * improved wrapper for hdfsOpenFile().
     */
    File();
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

    /**Checks if a given path exsits on the filesystem 
     * @param path The path to look for
     * @return Returns true on success, false on error.  
     */
    bool Exists(const char* path);

private:
    hdfsFS fs_;
    DISALLOW_COPY_AND_ASSIGN(FileSystem);
};




}; // namespace hdfs
}; // namespace tmacam







#endif // HDFSCPP_H_
// vim: et ai sts=4 ts=4 sw=4
