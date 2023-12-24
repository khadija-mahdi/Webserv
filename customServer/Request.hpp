
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestParser.hpp"

class MethodGet;
class MethodDelete;

enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0,
};

class Request {
protected:
	int									serverIndex;
	int									REQUEST_STATE;
	RequestParser						requestParser;
	HeaderData							*headerData;

public:
	Request();
	Request(HeaderData	*headerData);
	~Request();

	void prints(){
		std::cout << " -----------> path = " <<  headerData->Path << std::endl;
	}
	virtual bool	processRequest() = 0;
};


class RequestDefault : public Request{
protected:
	int									serverIndex;
	int									REQUEST_STATE;
	RequestParser						requestParser;
	HeaderData							*headerData;

public:
	RequestDefault();
	RequestDefault(HeaderData	*headerData);
	~RequestDefault();

	virtual bool	processRequest();
};

#endif