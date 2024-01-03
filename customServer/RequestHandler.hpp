
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
	DataPool 	*headerData;
	int			serverIndex;
	int			REQUEST_STATE;
	Request		*request;
	RequestParser requestParser;
public:
	RequestHandler(/* args */);
	RequestHandler(DataPool *);
	~RequestHandler();

	bool	processRedirectionAndAllowance();
	bool	parseHeaderErrors();
	bool	HandlerRequest1(std::string);
	Request *handlerRequestMethods();

};




#endif