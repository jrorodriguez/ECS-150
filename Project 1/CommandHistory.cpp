#include "CommandHistory.h"

CommandHistory::CommandHistory(int maxSize) {
	SetMaxSize(maxSize);
}

void CommandHistory::SetMaxSize(int size) {
	maxSize = size;
}

void CommandHistory::AddCommand(std::string command) {
	if (history.size() >= maxSize) {
		history.pop_back();
	}

	history.push_front(command);
}

std::string CommandHistory::GetCommand(int index) const {
	if (index < 0 || index >= history.size()) {
		return "";
	} else {
		return history[index];
	}
}

int CommandHistory::GetSize() const {
	return history.size();
}