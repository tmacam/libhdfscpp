#include "hdfscpp.h"

#include <iostream>

#include <vector>
#include <string>


/**********************************************************************/

void ShowUsage()
{
    std::cerr << "Usage: hdfslistblocks <path> " << std::endl;
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

    // Get block distributions for ALL blocks
    std::cout << path << " has a block size of "
              << (path_info->mBlockSize >> 20) << " MB" << std::endl;
    hdfs::BlockLocationList blocks_location;
    hdfs::GetFileBlockLocations(fs, path, 0, path_info->mSize,
            &blocks_location);
    for(int i = 0; i < blocks_location.size(); ++i) {
        std::cout << "In block " << i << std::endl;
        for(int j = 0; j < blocks_location[i].size(); ++j) {
            std::cout << "\tavailable @ host " << blocks_location[i][j]
                      << std::endl;
        }
    }
    std::cout << "Foi" << std::endl;


    
	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
