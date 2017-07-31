#include "ls.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void Output(std::string str);
void PrintAttributes(std::string dirName, struct dirent *file);
void PrintPermissions(struct stat *stat_buf);

void ls(std::vector<std::string> argv) {
	std::string dirName;

	if (argv.size() == 0) {
		return;
	} else if (argv.size() == 1) {
		dirName = "./";
	} else {
		dirName = argv[1];
	}

	// Append "/" if not there
	size_t index = dirName.find_last_of("/");
	if (index == std::string::npos || index != dirName.length() - 1) {
		dirName += "/";
	}

	DIR *dir;
	struct dirent *curFile;

	if ((dir = opendir(dirName.c_str()))) {
		while ((curFile = readdir(dir)) != NULL) {
			PrintAttributes(dirName, curFile);
		}
		closedir(dir);
	} else {
		Output("Failed to open directory \"" + dirName + "\"\r\n");
	}
}

// Cited:
// http://linux.die.net/man/2/stat
void PrintAttributes(std::string dirName, struct dirent *file) {
	std::string fileName = file->d_name;
	std::string fullFileName = dirName + fileName;
	struct stat stat_buf;
	stat(fullFileName.c_str(), &stat_buf);

	switch (stat_buf.st_mode & S_IFMT) {
	    case S_IFBLK:  
			Output("b");            
			break;
	    case S_IFCHR:  
			Output("c");  
	        break;
	    case S_IFDIR:  
			Output("d");                 
			break;
	    case S_IFIFO:  
			Output("p");                 
			break;
	    case S_IFLNK:  
			Output("l");                   
			break;
	    case S_IFSOCK: 
			Output("s");                    
			break;
	    default:       
			Output("-");                  
    }

	PrintPermissions(&stat_buf);
	Output(" " + fileName + "\r\n");
}

// Cited:
// http://stackoverflow.com/questions/20450556/linux-ls-l-implementation-using-file-descriptors-in-c-c
void PrintPermissions(struct stat *stat_buf) {
	Output((stat_buf->st_mode & S_IRUSR) ? "r" : "-");
	Output((stat_buf->st_mode & S_IWUSR) ? "w" : "-");
	Output((stat_buf->st_mode & S_IXUSR) ? "x" : "-");
	Output((stat_buf->st_mode & S_IRGRP) ? "r" : "-");
	Output((stat_buf->st_mode & S_IXGRP) ? "w" : "-");
	Output((stat_buf->st_mode & S_IWGRP) ? "x" : "-");
	Output((stat_buf->st_mode & S_IROTH) ? "r" : "-");
	Output((stat_buf->st_mode & S_IWOTH) ? "w" : "-");
	Output((stat_buf->st_mode & S_IXOTH) ? "r" : "-");
}

void Output(std::string str) {
	write(STDOUT_FILENO, str.c_str(), str.length());
}