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
	DataPool *headerData;

public:
	RequestParser(/* args */);
	RequestParser(DataPool *);
	~RequestParser();

	void getCurrentServer(std::vector<ConfServer> &);
	void getCurrentLocationIndex(std::vector<Location> &);
	void ParseRequest();
	void getContentType();
	void fillHeaderData();
	void printHeaderdata();
	int ParseUrl();
	bool redirectionType(std::vector<Location> &);
};

int directoryStatus(const std::string &);

#endif