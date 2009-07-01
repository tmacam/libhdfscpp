#include <iostream>
#include <hdfs.h>



/**********************************************************************/




/**********************************************************************/

void ShowUsage()
{
    std::cerr << "Usage: hdfsls <path> " << std::endl;
}

int main(int argc, char* argv[])
{
    // Check command line arguments
    if ( argc != 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        ShowUsage();
        exit(EXIT_FAILURE);
    }
    const char* path = argv[1];

    // Connect to HDFS / get filesystem handle
	hdfsFS fs = hdfsConnect("default", 0);
    if (!fs) {
        std::cerr << "Failed to connect to HDFS" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Path is a directory, right?
    hdfsFileInfo* path_info = hdfsGetPathInfo(fs, path);
    if (!path_info) {
        std::cerr << "Failed to get information about path '" << path 
                  << "'." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (path_info->mKind != kObjectKindDirectory ) {
        std::cerr << "Path '" << path << "' is not a directory." << std::endl;
        exit(EXIT_FAILURE);
    }
    hdfsFreeFileInfo(path_info, 1);

    
    int n_files = 0;
	hdfsFileInfo* files = hdfsListDirectory(fs, path, &n_files);
    if (!files) {
        std::cerr << "Error getting information about files in directory '"
                  << path << "'." << std::endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n_files; ++i) {
        std::cout << char(files[i].mKind)  << "\t" 
                  << files[i].mSize  << "\t" 
                  << files[i].mName
                  << std::endl;
    }
    hdfsFreeFileInfo(files, n_files);

    hdfsDisconnect(fs);

	return 0;
}

// vim: et ai sts=4 ts=4 sw=4
