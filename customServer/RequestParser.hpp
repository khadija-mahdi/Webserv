#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "../Webserv.hpp"


struct HeaderData
{
	int									locationIndex;
	std::map<std::string, std::string>	Headers;
	std::string							Buffer;
	std::string							Method;
	std::string							Path;
	Location							currentLocation;
	ConfServer							currentServer;
	std::string							newRoot;
	std::string							url;
};

class Request;

class RequestParser
{
public:
	RequestParser(/* args */);
	~RequestParser();

	void	getCurrentServer(std::vector<ConfServer> &, HeaderData &headerData);
	void	getCurrentLocationIndex(std::vector<Location> &, HeaderData &headerData);
	void	ParseRequest(HeaderData &headerData);
	void	fillHeaderData(HeaderData &);
	void  	printHeaderdata(HeaderData &headerData);
	int		ParseUrl(HeaderData &headerData);
};

#endif