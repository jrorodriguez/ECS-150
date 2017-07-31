#include "ff.h"
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void Search(std::string currentPath, std::string path, std::string term);
void Write(std::string str);

void ff(std::vector<std::string> argv) {
	if (argv.size() == 0) {
		return;
	} else if (argv.size() == 1) {
		Write("ff command requires a filename!\r\n");
		return;
	}

	std::string searchTerm = argv[1];
	std::string dirToStartSearchFrom;
	
	if (argv.size() == 3) {
		dirToStartSearchFrom = argv[2];
	} else {
		dirToStartSearchFrom = "./";
	}

	size_t index = dirToStartSearchFrom.find_last_of("/");
	if (index == std::string::npos || index != dirToStartSearchFrom.length() - 1) {
		dirToStartSearchFrom += "/";
	}

	Search(dirToStartSearchFrom, dirToStartSearchFrom, searchTerm);
}

void Search(std::string currentPath, std::string dirName, std::string fileToFind) {
	DIR *directory;
	struct dirent *curFile;
	struct stat stats;
	
	if(directory = opendir(dirName.c_str())) {
		while((curFile = readdir(directory)) != NULL) {
			std::string fileName = curFile->d_name;
			std::string fullFileName = currentPath + fileName;
			stat(fullFileName.c_str(), &stats);
			if((fileName != ".") && (fileName != "..") && 
			   ((stats.st_mode & S_IFMT) == S_IFDIR)) {
					Search(fullFileName + "/", fullFileName, fileToFind);
			} else if (fileToFind == fileName) {
				Write(fullFileName + "\r\n");
			}
		}
	} else {
		Write("Failed to open directory \"" + dirName + "\"\r\n");
		return;
	}

	if (curFile == NULL) {
		closedir(directory);
	}
}		

void Write(std::string str) {
	write(STDOUT_FILENO, str.c_str(), str.length());
}