#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Core.hpp"

class ResponseBuilder
{
public:
	ResponseBuilder(DataPool dataPool);
	ResponseBuilder(DataPool dataPool, HeadersType &ResponseHeaders);
	~ResponseBuilder();

private:
	std::string Buffer;
	DataPool dataPool;
	std::map<int, std::string> StatusCodes;
	HeadersType ResponseHeaders;

public:
	void FillBuffer();
	void FillHeaders(int StatusCode);
	int FlushBuffer(int SocketFd);
	void CreateStatusFile();
	void InitStatusCode();
	std::string GetDefaultErrorPagePath();
	std::map<int, std::string> &GetStatusCodes();
	bool GetErrorPage();
	bool checkErrorPage(std::map<int, std::string> error_pages);
};
#endif