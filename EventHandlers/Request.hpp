
#pragma once

#include "../Webserv.hpp"

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
	bool RequestParser(std::string Data, int &resp);
	void ParseHeaders(int &resp);
	void StorHeaderData();

	std::string send_response(int status_code, std::string content)
	{
		std::string status_phrase = StatusCodes[status_code];
		std::string response = "HTTP/1.1 " + SSRT(status_code) + " " + status_phrase + "\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: " +
							   SSRT(content.length()) + "\r\n\r\n" + content;
		return response;
	}
	void sendResponseError(int &);
	~Request();
};

class ThrowErrorCode : public std::exception
{
	std::string response;

public:
	ThrowErrorCode() {}
	virtual ~ThrowErrorCode() throw(){};
	virtual const char *what() const throw()
	{
		return ("Http ERRor");
	}
};
