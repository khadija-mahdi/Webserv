
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "MethodsHandlers/MethodGet.hpp"
#include "MethodsHandlers/MethodDelete.hpp"


class MethodGet;
class MethodDelete;

enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0,
};


class Request {
	int									serverIndex;
	int									REQUEST_STATE;
	RequestParser						requestParser;
	HeaderData							*headerData;
	MethodGet 							getMethod;
	MethodDelete 						deleteMethod;

public:
	Request();
	Request(HeaderData	*headerData);
	~Request();

	bool	RequestHandler(std::string);
	bool	processRequest();
	bool	parseHeaderErrors();
	bool	methodParser();
	bool	processRedirectionAndAllowance();

};

#endif