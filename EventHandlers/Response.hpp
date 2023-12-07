#pragma once

#include "../Webserv.hpp"
// #include "Request.hpp"

// class Request;
class Response
{
private:
	std::string responseBuffer;
	std::string ContentType;
	std::string responseHeader;
	std::map<int, std::string> StatusCodes;
	int statusCode;
	int fd;
	std::string Path;

public:
	void		setStatusCode(int const &res);
	void		setRequest(std::string const &req){Path = req;}
	void		setFD(int const &res){fd = res;};
	void		setResponseBuffer(std::string const &res);
	std::string getResponseBuffer() const;
	void 		setResponseHeader(std::string const &res);
	std::string getResponseHeader() const;
	int			getStatusCode() const;
	std::string DefaultErrorPage(int code);

	int PrepareNextChunk();
	int sendChunkResponse(int const &);
	void clearResponseBuffer();
	void getContentType();

	std::string& httpheader(int const &statusCode);
	int sendResponse(int const &statusCode);
	int sendResponse(int const &statusCode, std::string &FilePath);
	Response(/* args */);
	~Response(){};
};