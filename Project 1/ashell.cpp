#include "ashell.h"
#include "Tokenizer.h"
#include "pwd.h"
#include "ff.h"
#include "ls.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

Ashell::Ashell() {
	historyIndex = 0;
	history.SetMaxSize(MAX_HISTORY_SIZE);
	SetShellPrompt();
}

void Ashell::SetShellPrompt() {
	shellPrompt = GetCwd();
	if (shellPrompt.length() > MAX_PROMPT_SIZE) {
		std::size_t startOfLastDir = shellPrompt.rfind("/");
		shellPrompt = "/.../" + shellPrompt.substr(startOfLastDir + 1);
	}
	shellPrompt += "% ";
}

void Ashell::Launch() {
	char c;
	historyIndex = 0;

	SetNonCanonicalMode(STDIN_FILENO, &savedTermAttributes);	
	Write(shellPrompt);
	while (true) {
		read(STDIN_FILENO, &c, 1);
		if (c == 0x04) {
		// Ctrl+d
			Write("\r\n");
			break;
		} else {
			ProcessCharacter(c);
		}
	}
	ResetCanonicalMode(STDIN_FILENO, &savedTermAttributes);
}

void Ashell::SetNonCanonicalMode(int fd, struct termios *savedAttributes) {
	struct termios termAttributes;
	char *name;
	
	tcgetattr(fd, savedAttributes);
	tcgetattr(fd, &termAttributes);
	termAttributes.c_lflag &= ~(ICANON | ECHO);
	termAttributes.c_cc[VMIN] = 1;
	termAttributes.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSAFLUSH, &termAttributes);
}

void Ashell::ResetCanonicalMode(int fd, struct termios *savedAttributes) {
	tcsetattr(fd, TCSANOW, savedAttributes);
}

void Ashell::Write(std::string s) {
	write(STDOUT_FILENO, s.c_str(), s.length());
}

void Ashell::ProcessCharacter(char c) {
	if (c  == 0x7F || c == 0x08) {
	// Backspace
		DeleteChar();
	} else if (c == '\n') {
		OnEnterKeyPressed();
	} else if (isprint(c)) {
	// Can echo character typed
		write(STDOUT_FILENO, &c, 1);
		currentCommand += c;
	}  else if (c == 0x1B) {
	// Check if current character is the start of the delete key,
	// or up or down arrow key sequence.
		read(STDIN_FILENO, &c, 1);
		if (c == '[') {
			read(STDIN_FILENO, &c, 1);
			if (c == 'A') {
				OnUpArrowKeyPressed();
			} else if (c == 'B') { 
				OnDownArrowKeyPressed();
			} else if (c == '3') {
				read(STDIN_FILENO, &c, 1);
				if (c == '~') { 
					DeleteChar();
				}
			}
		} 
	}
}

void Ashell::DeleteChar() {
	if (currentCommand.length() <= 0) {
		write(STDOUT_FILENO, "\a", 1);
	} else {
		currentCommand.pop_back();
		write(STDOUT_FILENO, "\b \b", 3);
	}
}

void Ashell::OnUpArrowKeyPressed() {
	if (historyIndex >= history.GetSize()) {
		write(STDOUT_FILENO, "\a", 1);
	} else {
		historyIndex++;
		WriteCommandFromHistory(historyIndex - 1);
	}
}

void Ashell::OnDownArrowKeyPressed() {
	if (historyIndex <= 0) {
		write(STDOUT_FILENO, "\a", 1);
	} else {
		historyIndex--;
		WriteCommandFromHistory(historyIndex - 1);
	}
}

void Ashell::WriteCommandFromHistory(int index) {
	ClearCurrentCommand();
	std::string commandFromHist = history.GetCommand(index);
	currentCommand = commandFromHist;
	Write(currentCommand);
}

void Ashell::ClearCurrentCommand() {
	int len = currentCommand.length();
	for (int i = 0; i < len; i++) {
		DeleteChar();
	}
}

void Ashell::OnEnterKeyPressed() {
	Write("\r\n");
	if (!currentCommand.empty()) {
		history.AddCommand(currentCommand);
		Execute(currentCommand);
		currentCommand = "";
	}
	Write(shellPrompt);
}

void Ashell::Execute(std::string command) {
	std::vector<std::vector<std::string> > commands = Tokenizer::Tokenize(command);	

	if (commands.size() == 0) {
		return;
	} else if (commands[0].size() == 0) {
		return;
	}

	std::string commandName = commands[0][0];
	if (commandName == "cd") {
		ChangeDir(commands[0]);
		return;
	} else if (commandName == "exit") {
		Exit(EXIT_SUCCESS);
	}

	Execute(commands);	
}

void Ashell::ChangeDir(const std::vector<std::string>& argList) {
	int status;

	if (argList.size() <= 1) { 
	// No arg provided for cd
		status = chdir(getenv("HOME"));
	} else {
		status = chdir(argList[1].c_str());
	}

	if (status == 0) { 
	// Success
		SetShellPrompt();
	} else {
		Write("Erorr changing directory.\r\n");
	}
}

void Ashell::Exit(int exitCode) {
	ResetCanonicalMode(STDIN_FILENO, &savedTermAttributes);
	_exit(exitCode);
}

// Cited:
// http://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
void Ashell::Execute(std::vector<std::vector<std::string> > commands) {
	std::vector<int *> fds;
	int oldPipe = 0;
	int newPipe = 0;

	// Create all pipes
	for (int i = 0; i < commands.size() - 1; i++)
	{
		int *fd = new int[2];
		pipe(fd);
		fds.push_back(fd);
	}

	// For each command
	for (int i = 0; i < commands.size(); i++) {
		pid_t pid = fork();

		oldPipe = newPipe;
		if ((i > 0) && (i + 1 < commands.size())) {
			newPipe++;
		}

		if (pid == 0) {
		// Child process
			std::vector<std::string> &currCommand = commands[i];
			HandleRedirects(currCommand);

			// Handle pipe
			// If there is a previous command
			if (i > 0) {
				dup2(fds[oldPipe][0], STDIN_FILENO);
			}

			// If there is another command
			if (i + 1 < commands.size()) {
				dup2(fds[newPipe][1], STDOUT_FILENO);
			} 

			CloseAllPipes(fds);
			ExecuteChild(currCommand); 
		}
	}

	CloseAllPipes(fds);

	// Wait for all children
	for (int i = 0; i < commands.size(); i++) {
		wait(NULL);
	}

	// Free space
	for (int i = 0; i < fds.size(); i++) {
		delete[] fds[i];
	}
}

void Ashell::HandleRedirects(std::vector<std::string> &command) {
	std::vector<std::string>::iterator it = command.begin();
	while (it != command.end()) {
		if (*it == "<") {
			int in_fd = open((it + 1)->c_str(), O_RDONLY);
			dup2(in_fd, STDIN_FILENO);
			close(in_fd);

			it = command.erase(it);
			it = command.erase(it);
		} else if (*it == ">") {
			int out_fd = open((it + 1)->c_str(), O_CREAT | O_WRONLY, 0644);
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);

			it = command.erase(it);
			it = command.erase(it);
		} else {
			it++;
		}
	}
}

void Ashell::CloseAllPipes(std::vector<int *> &fds) {
	for (int i = 0; i < fds.size(); i++) {
		close(fds[i][0]);
		close(fds[i][1]);
	}
}

void Ashell::ExecuteChild(std::vector<std::string> command) {
	if (command[0] == "pwd") {
		PrintCwd();
		_exit(EXIT_SUCCESS);
	} else if (command[0] == "ff") {
		ff(command);
		_exit(EXIT_SUCCESS);
	} else if (command[0] == "ls") {
		ls(command);
		_exit(EXIT_SUCCESS);
	} else {
		// Convert to char**
		std::vector<char *> args;
		for (int j = 0; j < command.size(); j++) {
			args.push_back(const_cast<char *>(command[j].c_str()));	
		}
		args.push_back(NULL);

		if (execvp(args[0], args.data()) < 0) {
		// Process failed to execute
			Write("Failed to execute " + command[0] + "\r\n");
			_exit(EXIT_FAILURE);
		}
	}
}