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
	bool Headers_Stage;
	std::string Path;
	int isRed;

public:
	std::string RedirectionPath;
	void		setStatusCode(int const &res, int flag);
	void		setRequest(std::string const &req){Path = req;}
	void		setFD(int const &res){fd = res;};
	void		setResponseBuffer(std::string const &res);
	std::string getResponseBuffer() const;
	void 		setResponseHeader(std::string const &res);
	std::string getResponseHeader() const;
	int			getStatusCode() const;
	int			getFD() const{return fd;};
	std::string DefaultErrorPage(int code);

	int PrepareNextChunk();
	int sendChunkResponse(int const &);
	void clearResponseBuffer();
	void getContentType(std::string const &);

	std::string& httpheader(int const &statusCode);
	int sendResponse(int const &statusCode);
	int sendResponse(int const &statusCode, std::string &FilePath);
	Response(/* args */);
	~Response(){};
};