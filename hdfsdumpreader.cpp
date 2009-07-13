
#include "hdfsdumpreader.h"

#include <assert.h>
#include <zlib.h>

#include <iostream>


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
namespace tmacam {



HdfsDumpReader::HdfsDumpReader(hdfs::FileSystem* fs, const char* path,
                   size_t buffer_size) :
    fs_(fs),
    path_(path),
    file_(*fs, path, O_RDONLY, buffer_size, 0, 0),
    file_size_(0), // we will get to you in a momment, sir
    buffer_size_(buffer_size),
    buffer_(buffer_size),
    bytes_read_(0),
    available_data_(),
    empty_file_buffer_()
{
    hdfs::FileInfoList file_info;
    fs_->GetPathInfo(path, &file_info);
    file_size_ = file_info->mSize;
}

bool HdfsDumpReader::HasNext()
{
    // We won't be done untill we have read all the file and
    // there is nothing left in available_data_ to be read.
    return (bytes_read_ < file_size_) || !available_data_.eof();
}

tmacam::filebuf HdfsDumpReader::GetNext()
{
    /* First of all, remember that this code originally was something like
     * this:
     *
     * for (bytes_read_ = 0; bytes_read_ < file_size_;) {
     *   (...)
     *   try {
     *      while(!available_data_.eof()) {
     *          (...)
     *          // yield data unit
     *      }
     *  } catch (std::out_of_range) {
     *      (...)
     *  }
     *
     * Missing closures now? 
     *
     * Using Pread() and exceptions for doing this is ugly and lame.
     * We should have used Read() and proper tests but, you know what,
     * this works and is easy to understand -- and this scores +1 in
     * my book.
     */

    assert(HasNext()); // Simplify corner cases for the next if

    // Do we need to perform a read operation? Can we perform a read?
    if (available_data_.eof() && (bytes_read_ < file_size_)) {
        tSize read_length = file_.Pread(bytes_read_, &buffer_[0], buffer_size_);
        bytes_read_ += read_length;
        available_data_ = tmacam::filebuf(&buffer_[0], read_length);
    }

    // Save current progress
    size_t data_left_len = available_data_.len();

    try {
        // Try to read a full pkt (hdr + payload + crc trailer)
        assert(data_left_len);
        const char* pkt_start = available_data_.current;
        // Read header, payload and CRC. Abort if payload is too big
        int32_t payload_len = ReadInt(&available_data_);
        assert(payload_len + 2*sizeof(int32_t) < buffer_size_);
        const char* payload_data = available_data_.read(payload_len);
        int32_t expected_checksum = ReadInt(&available_data_);
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
        // Ok, pkt content was read and is sane. Return it.
        size_t pkt_len = data_left_len - available_data_.len();
        return tmacam::filebuf(pkt_start, pkt_len);
    } catch(std::out_of_range) {
        // Ooops! Not enough data... 
        // rewind reading position and reset available_data_
        bytes_read_ -= data_left_len;
        available_data_ = empty_file_buffer_;
        // and retry
        return GetNext();
    }
}

// Code left for historical and documentational purposes.

/* 
 * void HdfsDumpReader::ReadFile()
 * {
 *     size_t data_left_len = 0;
 * 
 *     for (bytes_read_ = 0; bytes_read_ < file_size_;) {
 *         !* Using Pread() and exceptions for doing this is ugly and lame.
 *          * We should have used Read() and proper tests but, you know what,
 *          * this works and is easy to understand -- and this scores +1 in
 *          * my book.
 *          *!
 *         tSize read_length = file_.Pread(bytes_read_,
 *                                         &buffer_[0],
 *                                         buffer_size_);
 *         bytes_read_ += read_length;
 * 
 *         available_data_ = tmacam::filebuf(&buffer_[0], read_length);
 * 
 *         std::cout << "READ " << read_length << std::endl;
 * 
 *         try {
 *             while (!available_data_.eof()) {
 *                 // Save current progress
 *                 data_left_len = available_data_.len();
 *                 // Read header, payload and CRC. Abort if payload is too big
 *                 int32_t payload_len = ReadInt(&available_data_);
 *                 assert(payload_len + 2*sizeof(int32_t) < buffer_size_);
 *                 const char* payload_data = available_data_.read(payload_len);
 *                 int32_t expected_checksum = ReadInt(&available_data_);
 * #ifndef DUMPREADER_FAST_PASS
 *                 // Calc CRC32
 *                 uLong crc = crc32(0L,
 *                                   (const Bytef*)payload_data,
 *                                   payload_len);
 *                 if (expected_checksum != static_cast<int32_t>(crc)) {
 *                     std::cerr << "CRC MISSMATCH -- found " << crc << 
 *                         " expected" << expected_checksum <<
 *                         std::endl; 
 *                     exit(EXIT_FAILURE);
 *                 }
 * #endif
 *                 std::cout << "P: " <<  payload_len << std::endl;
 *             }
 *         } catch(std::out_of_range) {
 *             std::cout << "ooops " <<  std::endl;
 *             // not enough data... 
 *             // rewind reading position
 *             bytes_read_ -= data_left_len;
 *         }
 *     }
 * }
 */


}; // namespace tmacam


// vim: et ai sts=4 ts=4 sw=4
