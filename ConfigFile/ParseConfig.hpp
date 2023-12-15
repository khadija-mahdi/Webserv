#pragma once

#include "../Webserv.hpp"

class ParseConfig
{
private:
	std::stringstream	errorMessage;
	std::string			lines;
	bool				BLOCK_FORMAT;

public:
	ParseConfig();
	ConfServer	parseServerBlock(std::string &);
	Location	parseLocationBlock(std::string &); 
	void		syntaxForm(std::string &);
	void		readConfigFile(std::string const&);
	void		CurlyBrackets();
	void		PreProcessingFile(std::string &);
	void		pacingConfigFile(char *);
	void		parsingValues();
	void		parseHttpBlock(std::string &);
	void		ServerInHttp(std::string &, Http &);
	void		locationInServer(std::string &, ConfServer &);
	void		parseEventsBlock(std::string &);
	void		includeMimeTypes(std::string &);
};

std::map<std::string, std::string>	extractKeyValuesIN(const std::string&);
std::map<std::string, std::string>	extractKeyValues(const std::string &,std::map<int , std::string> &);
std::vector<std::string>			BlocksExtra(const std::string &, const std::string &);
std::string							&skepComment(std::string &);
std::string							Blocks(const std::string &, const std::string &);
int									linesLength(std::string&);
bool								isDigit(std::string &);
bool								lineSpace(std::string &);
void								syntaxForm(std::string &);
bool								isInFormat(std::string&, std::string&);
void								processErrorPage(const std::string& , std::string &, int&);
void								includeMimeTypes(std::string &);
bool								isDigitStr(std::string &);
bool								isDigit(std::string &v);
int									linesLength(std::string&);
void								splitKeyValue(std::string &, std::string &, std::string &, std::string word, int);
void								singleData(ConfServer &, std::string &);
void								locationValues(Location &, std::string &);
bool								processErrors(std::string &, int &, std::string &);
bool								isDigitStr(std::string &);