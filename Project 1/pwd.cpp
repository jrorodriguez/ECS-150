#include <unistd.h>
#include <string>
#include "pwd.h"

std::string GetCwd() {
	char path[1024];
	if (getcwd(path, sizeof(path))) {
		return path;
	} else {
		return "";
	}
}

void PrintCwd() {
	std::string cwd = GetCwd();
	write(STDOUT_FILENO, cwd.c_str(), cwd.length());
	write(STDOUT_FILENO, "\r\n", 2);
}