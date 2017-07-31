#ifndef ASHELL_H
#define ASHELL_H

#include "CommandHistory.h"
#include <string>
#include <termios.h>
#include <vector>

#define MAX_PROMPT_SIZE 16
#define MAX_HISTORY_SIZE 10

class Ashell {
private:
	CommandHistory history;
	struct termios savedTermAttributes;
	std::string currentCommand;
	std::string shellPrompt;
	int historyIndex;

	void SetNonCanonicalMode(int fd, struct termios *savedAttributes);
	void ResetCanonicalMode(int fd, struct termios *savedAttributes);
	void Write(std::string s);
	void SetShellPrompt();
	void ProcessCharacter(char c);
	void DeleteChar();
	void OnEnterKeyPressed();
	void OnDownArrowKeyPressed();
	void OnUpArrowKeyPressed();
	void WriteCommandFromHistory(int index);
	void ClearCurrentCommand();
	void Execute(std::string command);
	void Execute(char **args);
	void Execute(std::vector<std::vector<std::string> > commands);
	void HandleRedirects(std::vector<std::string> &command);
	void CloseAllPipes(std::vector<int *> &fds);
	void ExecuteChild(std::vector<std::string> command);
	void ChangeDir(const std::vector<std::string>& argList);
	void Exit(int exitCode);
public:
	Ashell();
	void Launch();
};

#endif
