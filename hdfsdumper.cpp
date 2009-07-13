/**@file hdfsdumper.cpp
 * @brief CLI utility that wraps hdfsdumpreader code into a CLI program.
 *
 */
#include "hdfscpp.h"
#include "hdfsdumpreader.h"

#include <iostream>

void ShowUsage()
{
    std::cerr << "Usage: hdfsdumper <path> " << std::endl;
}

void ProcessFile(tmacam::hdfs::FileSystem* fs, const char* path)
{
    using namespace tmacam;

    HdfsDumpReader reader(fs, path);
    while(reader.HasNext()) {
        reader.GetNext();
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
            ProcessDirectory(&fs, path);
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
