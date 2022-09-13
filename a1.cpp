#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <pwd.h>
#include <unistd.h>

int main(int argc, char** argv )
{
    struct stat fileInfo;

    if (argc < 2) {
        std::cout << "Usage: fileinfo <file name>\n";
        return(EXIT_FAILURE);
    }

    if (stat(argv[1], &fileInfo) != 0) {  // Use stat() to get the info
        std::cerr << "Error: " << strerror(errno) << '\n';
        return(EXIT_FAILURE);
    }

    std::string fileType;
    fileType = fileInfo.st_mode;

    // if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) { // From sys/types.h
    //     fileType = "Directory\n";
    // }
    // else {
    //     fileType = "File\n";
    // }

    // struct passwd *pwd;
    // std::string name;
    // pwd = getpwuid(fileInfo.st_uid);


    struct passwd *pws;
    pws = getpwuid(fileInfo.st_uid);
    std::string userName = pws->pw_name;

   
    std::cout << "Inode number   : " << fileInfo.st_ino << '\n';                // Inode number
    std::cout << "File type      : " << "temp" << '\n';                               // File Type
    std::cout << "User Name      : " << userName << '\n' ;                                  // User Name
    std::cout << "Size           : " << fileInfo.st_size << '\n';               // Size in bytes
    std::cout << "Modified       : " << std::ctime(&fileInfo.st_mtime);         // Last mod time IMPORTANT HERE
}