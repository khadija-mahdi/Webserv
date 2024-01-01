#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "../Webserv.hpp"

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


struct ResponseData{
	std::string							Location;
	int									StatusCode;
	int									ResponseType;
	int    								fileFd;
	int									contentType;
};

struct CGI_DATA
{
	std::string cgiPath;
};



struct HeaderData
{
    int									ResponseStatus; // NO-NO
    HeadersType							cgiHeaders; 
    std::string							Query; // example : ?name=ayoub&pass=*****
	
	int									locationIndex;
	std::map<std::string, std::string>	Headers;
	std::string							Buffer;
	std::string							Method;
	std::string							Path;
	Location							currentLocation;
	ConfServer							currentServer;
	std::string							newRoot;
	std::string							url;
	bool								REDIRECTION_STAGE;
	ResponseData						response;
	bool								RESPONSE__STATE;
	bool								CGI__STAGE;
	CGI_DATA 							cgiData;

};


class RequestParser
{
	// Request		*request;
	HeaderData	*headerData;
public:
	RequestParser(/* args */);
	RequestParser(HeaderData *);
	~RequestParser();

	void	getCurrentServer(std::vector<ConfServer> &);
	void	getCurrentLocationIndex(std::vector<Location> &);
	void	ParseRequest();
	void	fillHeaderData();
	void  	printHeaderdata();
	int		ParseUrl();
	bool	redirectionType(std::vector<Location>&);


	bool	checkInHttp(int const &, int);
	bool	checkErrorPage(int const &, std::map<int, std::string>& , int);
};

int directoryStatus(const std::string& );

#endif