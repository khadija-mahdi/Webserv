
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
	DataPool 							*headerData;

public:
	Request();
	Request(DataPool	*headerData);
	~Request();
	virtual bool HandleRequest(std::string &data) = 0;

};




#endif