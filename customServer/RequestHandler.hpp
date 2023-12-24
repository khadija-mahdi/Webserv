
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"
#include "Request.hpp"
#include "MethodsHandlers/MethodDelete.hpp"
#include "MethodsHandlers/MethodGet.hpp"


class Request;
class RequestParser;

class RequestHandler
{
private:
	HeaderData	*headerData;
	int			serverIndex;
	int			REQUEST_STATE;
	Request		*request;
	RequestParser requestParser;
public:
	RequestHandler(/* args */);
	RequestHandler(HeaderData *);
	~RequestHandler();

	bool	processRedirectionAndAllowance();
	bool	parseHeaderErrors();
	bool	HandlerRequest(std::string);
	Request *handlerRequestMethods();

};




#endif