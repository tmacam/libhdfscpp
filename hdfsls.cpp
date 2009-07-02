#include "hdfscpp.h"

#include <iostream>

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
    } else {
        for (int i = 0; i < files.size(); ++i) {
            std::cout << char(files[i].mKind)  << "\t" 
                << files[i].mSize  << "\t" 
                << files[i].mName
                << std::endl;
        }
    }

	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
