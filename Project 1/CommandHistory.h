#ifndef COMMANDHISTORY_H
#define COMMANDHISTORY_H

#define DEFAULT_MAX_SIZE 10

#include <deque>
#include <string>

class CommandHistory {
private:
	std::deque<std::string> history;
	int maxSize;

public:
	CommandHistory(int maxSize = DEFAULT_MAX_SIZE);
	void AddCommand(std::string command);
	std::string GetCommand(int index) const;
	int GetSize() const;
	void SetMaxSize(int size);
};

#endif
