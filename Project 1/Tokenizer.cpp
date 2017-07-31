#include "Tokenizer.h"

std::vector<std::vector<std::string> > Tokenizer::Tokenize(std::string s) {
	std::vector<std::vector<std::string> > commands;
	std::vector<std::string> tokens;
	std::string curTok = "";

	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ' ' || s[i] == '|' || s[i] == '<' || s[i] == '>') {
			if (!curTok.empty()) {
				tokens.push_back(curTok);
				curTok = "";
			}
		} else if (s[i] == '\\') {
			i++;
			if (i < s.length()) {
				curTok += s[i];
			}
		} else {
			curTok += s[i];
		}

		if (s[i] == '|') {
			if (!tokens.empty()) {
				commands.push_back(tokens);
				tokens.clear();
			}
		} else if (s[i] == '<' || s[i] == '>') {
			tokens.push_back(std::string(1, s[i]));
		}
	}

	if (!curTok.empty()) {
		tokens.push_back(curTok);
	}

	if (!tokens.empty()) {
		commands.push_back(tokens);
	}

	return commands;
}