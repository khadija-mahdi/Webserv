#pragma once

#include "../Webserv.hpp"
#include "Request.hpp"

class Response
{
private:
	std::string responseBuffer;
	std::string ContentType;
	std::map<int, std::string> StatusCodes;
	int statusCode;
	int fd;

public:
	void setStatusCode(int const &res);
	int getStatusCode() const;
	void setResponseBuffer(std::string const &res);
	std::string getResponseBuffer() const;
	std::string DefaultErrorPage(std::string Message, int code);

	int PrepareNextChunk();
	int sendChunkResponse(int const &, Request &request);
	void clearResponseBuffer();
	void getContentType(Request &request);

	std::string httpheader(int const &statusCode, Request &request);
	void sendResponse(int const &statusCode, Request &request);
	Response(/* args */);
	~Response(){};
};