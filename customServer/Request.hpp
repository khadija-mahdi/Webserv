
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Webserv.hpp"
#include "Response.hpp"
#include "RequestParser.hpp"


class Response;
enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0
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
	void	parseHeaderErrors(Response &);
	void	handleDirectoryPath(Response &);
	bool	methodParser(Response &);
	bool	processRedirectionAndAllowance(Response &);
};

#endif