#ifndef HDFSDUMPREADER_H_
#define HDFSDUMPREADER_H_

#include "hdfscpp.h"
#include "filebuf.h"

namespace tmacam {

class HdfsDumpReader {
public:
    static const size_t kDefaultBufferSize =  10<<20; // 10 MB

    HdfsDumpReader(hdfs::FileSystem* fs,
                   const char* path,
                   size_t buffer_size = kDefaultBufferSize);

    bool HasNext();

    // Return a full pkt (hdr + payload + crc trailer)
    tmacam::filebuf GetNext();

private:
    hdfs::FileSystem* fs_;
    std::string path_;
    hdfs::File file_;
    tOffset file_size_;
    size_t buffer_size_;
    std::vector<char> buffer_;
    // Used for iteration control and state-keeping
    size_t bytes_read_;
    tmacam::filebuf available_data_;
    const tmacam::filebuf empty_file_buffer_;

    // No copy, no atribution and no default constructor for this class
    DISALLOW_COPY_AND_ASSIGN(HdfsDumpReader);
    HdfsDumpReader();
};


}; // namespace tmacam

#endif //HDFSDUMPREADER_H_
// vim: et ai sts=4 ts=4 sw=4
