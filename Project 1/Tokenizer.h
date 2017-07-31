#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

class Tokenizer {
public:
	static std::vector<std::vector<std::string> > Tokenize(std::string s);
};

#endif