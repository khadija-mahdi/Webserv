#include "ParseConfig.hpp"

bool isDigitStr(std::string &value){
	for (size_t i = 0; i < value.length() ; i++)
	{
		if (!std::isdigit(value[i]))
			return 0;
	}
	return true;
}

int linesLength(std::string& Block){
    size_t len = 0;
    for (size_t i = 0 ; Block[i]; i++)
    {
        if (Block[i] == '\n')  
            len++;
    }
    return len;
}

bool isDigit(std::string &value){
    for (size_t i = 0; i < value.length() -2  ; i++)
    {
        if (!std::isdigit(value[i]) && value[i] != ' ' && value[i] != '\t' )
			throw std::runtime_error(THROW_COLORED("key is not integer : " + value));
    }
    return true;
}

bool lineSpace(std::string &line) {
    for (size_t i =0; line[i]; i++){
        if (line[i] != ' ' && line[i] != '\t')
            return 0;
    }
    return 1;
}

std::string &skepComment(std::string &line) {
    for (size_t i =0; line[i]; i++){
        if (line[i] == '#')
            return line = line.substr(0, i);
    }
    return line;
}

std::string locationSyntax(std::string word){
    std::string locWord = "";

    size_t i;
    for(i = 0; word[i] &&( word[i] == ' ' || word[i] == '\t'); i++);
    for(; word[i] && word[i] != ' '; i++)
        locWord += word[i];
    for(; word[i] &&( word[i] == ' ' || word[i] == '\t'); i++);
	
    if (!word[i])
		throw std::runtime_error(THROW_COLORED("Error: Syntax error in : " + word));
    if (locWord == "location"){
        return locWord;
	}
	throw std::runtime_error(THROW_COLORED("Error: Syntax error in : " + word));
    return NULL;
}

bool isInFormat(std::string& word, std::string& line) {
	ConfServer ConfServer;
    if (word == "events " || word == "http " 
        || word == "	server " ||word ==  locationSyntax(word)){
        if (word + "{" != line)
			throw std::runtime_error(THROW_COLORED("Error: Syntax error in curly barrket: " + line));
    }
    return true;
}