#include "ParseConfig.hpp"

bool isDigitStr(std::string &value){
	for (int i = 0; i < value.length() ; i++)
	{
		if (!std::isdigit(value[i]))
			return 0;
	}
	return true;
}

int linesLength(std::string& Block){
    int len = 0;
    for (size_t i = 0 ; Block[i]; i++)
    {
        if (Block[i] == '\n')  
            len++;
    }
    return len;
}

bool isDigit(std::string &value){
    for (int i = 0; i < value.length() -2  ; i++)
    {
        if (!std::isdigit(value[i]))
            throw std::runtime_error("key is not integer : " + value);
    }
    return true;
}

bool lineSpace(std::string &line) {
    for (int i =0; line[i]; i++){
        if (line[i] != ' ' && line[i] != '\t')
            return 0;
    }
    return 1;
}

std::string &skepComment(std::string &line) {
    for (int i =0; line[i]; i++){
        if (line[i] == '#')
            return line = line.substr(0, i);
    }
    return line;
}

std::string locationSyntax(std::string word){
    std::string locWord = "";
	std::stringstream errorMessage;

    int i;
    for(i = 0; word[i] &&( word[i] == ' ' || word[i] == '\t'); i++);
    for(; word[i] && word[i] != ' '; i++)
        locWord += word[i];
    for(; word[i] &&( word[i] == ' ' || word[i] == '\t'); i++);
	
    if (!word[i]){
        errorMessage << "\033[1;31mError: Syntax error in : " << word << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
    if (locWord == "location")
        return locWord;
	errorMessage << "\033[1;31mError: Syntax error in : " << word << "\033[0m" << std::endl;
	throw std::runtime_error(errorMessage.str());
    return NULL;
}

bool isInFormat(std::string& word, std::string& line) {
	std::stringstream errorMessage;
	ConfServer ConfServer;
    if (word == "events " || word == "http " 
        || word == "	server " ||word ==  locationSyntax(word)){
        if (word + "{" != line){
			errorMessage << "\033[1;31mError: Syntax error in : " << line << "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
    }
    return true;
}