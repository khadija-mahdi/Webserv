#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "Webserv.hpp"
#include "Core.hpp"

typedef std::map<std::string, std::string> HeadersType;

enum
{
	IS_DIRECTORY = 0,
	IS_FILE = 0,
	VALID_PATH1 = 0,
	READ_PERMEATIONS = 0,
	WRITE_PERMEATIONS = 0,
	EXECUTE_PERMEATIONS = 0
};



class RequestParser
{
public:
	RequestParser(/* args */);
	~RequestParser();

	void getCurrentServer(std::vector<ConfServer> & , DataPool &headerData);
	void getCurrentLocationIndex(std::vector<Location> &, DataPool &headerData);
	void ParseRequest(DataPool &headerData, std::string buffer);
	void fillHeaderData(DataPool &headerData, std::string Buffer);
	void printHeaderdata(DataPool &headerData);
	int	 ParseUrl(DataPool &headerData);
	bool redirectionType(std::vector<Location> &, DataPool &headerData);
};

int directoryStatus(const std::string &);

#endif