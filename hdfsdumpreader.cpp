#include "hdfscpp.h"

#include <iostream>

#include <assert.h>

#include "filebuf.h"
#include <vector>

#include <zlib.h>
/**********************************************************************/

/** Reads a int written by java's DataOutput#writeInt
 *
 * FIXME Move to propper namespace
 */
inline int32_t ReadInt(tmacam::filebuf* data) {
	assert(sizeof(int32_t) == 4);
	const char* bytes = data->read(sizeof(int32_t));
	return (((bytes[0] & 0xff) << 24) | ((bytes[1] & 0xff) << 16) |
		 ((bytes[2] & 0xff) << 8) | (bytes[3] & 0xff));
}

    
/**********************************************************************/
/*
class HdfsDumpReader {
private:
    // No copy, no atribution and no default constructor for this class
    DISALLOW_COPY_AND_ASSIGN(HdfsDumpReader);
    HdfsDumpReader();
};
*/

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

    // Path is a file, right?
    if (!fs.Exists(path)) {
        std::cerr << "Path '" << path << "' does not exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    hdfs::FileInfoList path_info;
    fs.GetPathInfo(path, &path_info);
    if (path_info->mKind != kObjectKindFile ) {
        std::cerr << "Path '" << path << "' is not a regular file."
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    /// BEGIN reader code
    const size_t buff_size = 1<<20; // 1 MB
    std::vector<char> buff(buff_size);

    size_t data_left_len = 0;

    hdfs::File file(fs, path, O_RDONLY, buff_size, 0, 0);
    for (size_t bytes_read = 0; bytes_read < path_info->mSize;) {
        /* Using Pread() and exceptions for doing this is ugly and lame.
         * We should have used Read() and proper tests but, you know what,
         * this works and is easy to understand -- and this scores +1 in
         * my book.
         */
        tSize read_length = file.Pread(bytes_read, &buff[0], buff_size);
        bytes_read += read_length;

        tmacam::filebuf available_data(&buff[0], read_length);

        std::cout << "READ " << read_length << std::endl;

        try {
            while (!available_data.eof()) {
                // Save current progress
                data_left_len = available_data.len();
                // Read header, payload and CRC. Abort if payload is too big
                int32_t payload_len = ReadInt(&available_data);
                assert(payload_len + 2*sizeof(int32_t) < buff_size);
                const char* payload_data = available_data.read(payload_len);
                int32_t expected_checksum = ReadInt(&available_data);
#ifndef DUMPREADER_FAST_PASS
                // Calc CRC32
                uLong crc = crc32(0L, (const Bytef*)payload_data, payload_len);
                if (expected_checksum != static_cast<int32_t>(crc)) {
                    std::cerr << "CRC MISSMATCH -- found " << crc << 
                        " expected" << expected_checksum <<
                        std::endl; 
                    exit(EXIT_FAILURE);
                }
#endif
                std::cout << "P: " <<  payload_len << std::endl;
            }
        } catch(std::out_of_range) {
            std::cout << "ooops " <<  std::endl;
            // not enough data... 
            // rewind reading position
            bytes_read -= data_left_len;
        }
    }


    /// END reader code
    
	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
