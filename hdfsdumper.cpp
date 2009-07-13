/**@file hdfsdumper.cpp
 * @brief CLI utility that wraps hdfsdumpreader code into a CLI program.
 *
 */
#include "hdfscpp.h"
#include "hdfsdumpreader.h"

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>

#include <zlib.h>

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



void ShowUsage()
{
    std::cerr << "Usage: hdfsdumper <path> " << std::endl;
}

void ProcessFile(tmacam::hdfs::FileSystem* fs, const char* path)
{
    using namespace tmacam;

    HdfsDumpReader reader(fs, path);
    while(reader.HasNext()) {
        filebuf full_pkt = reader.GetNext();
        
        // Confirm we are receiving the whole thing
        int32_t payload_len = ReadInt(&full_pkt);
        const char* payload_data = full_pkt.read(payload_len);
        int32_t expected_checksum = ReadInt(&full_pkt);
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

        std::cout << ".";
        std::cout.flush();
    }
    std::cout<< std::endl;
}

void ProcessDirectory(tmacam::hdfs::FileSystem* fs, const char* path)
{
    using namespace tmacam;


    hdfs::FileInfoList files;
    fs->ListDirectory(path, &files);
    if (files.empty()) {
        std::cout << "Directory is empty" << std::endl;
    } else {
        for (int i = 0; i < files.size(); ++i) {
            std::cout << "# " << files[i].mName << std::endl;
            ProcessFile(fs, files[i].mName);
        }
    }
}

void ProcessDirectory2(tmacam::hdfs::FileSystem* fs, const char* path)
{
    using namespace tmacam;

    std::vector<std::string> files;
    ListDirectoryEntries(fs, path, &files);
    if (files.empty()) {
        std::cout << "Directory is empty" << std::endl;
    } else {
        for (int i = 0; i < files.size(); ++i) {
            std::cout << "# " << files[i] << std::endl;
            ProcessFile(fs, files[i].c_str());
        }
    }
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
    switch (path_info->mKind) {
        case kObjectKindFile:
            // reader.ReadFile();
            ProcessFile(&fs, path);
            break;
        case kObjectKindDirectory:
            ProcessDirectory2(&fs, path);
            break;
        default:
            std::cerr << "Path '" << path << "' is not a regular file " <<
                "nor a directory." << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
