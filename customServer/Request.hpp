
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Webserv.hpp"
#include "Response.hpp"
#include "RequestParser.hpp"


class Response;
enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0,
};

class Request {
	int									serverIndex;
	int									fd;
	int									REQUEST_STATE;
	RequestParser						requestParser;

public:
	HeaderData							headerData;
	Request();
	~Request();

	bool	RequestHandler(std::string , Response& );
	void	processRequest(Response& );
	bool	parseHeaderErrors(Response &);
	void	handleDirectoryPath(Response &);
	bool	methodParser(Response &);
	bool	processRedirectionAndAllowance(Response &);
	bool	checkErrorPage(Response &, int const &, std::map<int, std::string>& , int);
	bool	checkInHttp(Response & , int const &, int);
	bool	GetMethod(Response &);

};

#endif