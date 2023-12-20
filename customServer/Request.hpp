
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "MethodsHandlers/MethodGet.hpp"


class MethodGet;

enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0,
};


class Request {
	int									serverIndex;
	int									REQUEST_STATE;
	RequestParser						requestParser;

public:
	HeaderData							*headerData;
	MethodGet 							getMethod;
	Request();
	Request(HeaderData	*headerData);
	~Request();

	bool	RequestHandler(std::string);
	void	processRequest();
	bool	parseHeaderErrors();
	bool	methodParser();
	bool	processRedirectionAndAllowance();

};

#endif