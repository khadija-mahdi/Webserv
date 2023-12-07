
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Webserv.hpp"
#include "Response.hpp"

class Response;
enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0
};

class Request
{
private:
	int REQUEST_STATE;
	std::map<std::string, std::string> Headers;
	std::string Buffer;
	std::string Method;
	std::string Path;
	int fd;

public:
	std::map<int, std::string> StatusCodes;
	Request(/* args */);

	std::string getBuffer() const;
	std::string getMethod() const;
	std::string getPath() const;
	std::map<std::string, std::string> getHeaders() const;

	void setHeaders(std::string &_value, std::string _key);
	void setBuffer(std::string const &);
	void setMethod(std::string const &);
	void setPath(std::string const &);
	bool RequestParser(std::string Data, Response& response);
	void ParseHeaders(Response& response);
	void StorHeaderData();

	~Request();
};

#endif