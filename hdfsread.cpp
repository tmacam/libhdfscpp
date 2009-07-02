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

    const size_t buff_size = 10*1024;
    char buff[buff_size + 1];
    buff[buff_size] = '\0';
    hdfs::File file(fs, path, O_RDONLY, buff_size, 0, 0);
    for (size_t bytes_read = 0; bytes_read < path_info->mSize;) {
        tSize read_length = read_length = file.Read(buff, buff_size);
        bytes_read += read_length;
        buff[read_length] = '\0';
        std::cout << buff;
        //std::cout << read_length << std::endl;
    }
    
	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
