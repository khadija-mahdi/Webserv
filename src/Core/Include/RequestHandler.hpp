
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"
#include "../RequestHandlers/Include/Request.hpp"
#include "../RequestHandlers/Include/MethodDelete.hpp"
#include "../RequestHandlers/Include/MethodGet.hpp"
#include "../RequestHandlers/Include/PostRequest.hpp"

class Request;
class RequestParser;

enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0,
};

class RequestHandler
{
private:
	// DataPool headerData;
	int serverIndex;
	int REQUEST_STATE;
	RequestParser requestParser;

public:
	Request *request;
	DataPool dataPool;
	// DataPool dataPool;
	RequestHandler(/* args */);
	// RequestHandler(DataPool &);

	~RequestHandler();

	bool parseHeaderErrors();
	bool HandlerRequest1(std::string);
	Request *handlerRequestMethods();
	Request *GetRequestHandler();
	DataPool &GetDataPool(void);
};

#endif