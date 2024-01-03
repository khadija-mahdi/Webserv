#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <map>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <stack>
#include <vector>
#include <deque>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <sys/stat.h>
#include <utility>
#include <fcntl.h>
#include <algorithm>
#include <dirent.h>

typedef std::map<std::string, std::string>::iterator HeaderIterator;

class ThrowErrorCode : public std::exception
{
	std::map<int, std::string> StatusCodes;
	int status;
	int type;
	std::string messagesError;

public:
	ThrowErrorCode(int const &st, int const &_type)
	{
		StatusCodes[100] = "Continue";
		StatusCodes[101] = "Switching Protocols";
		StatusCodes[200] = "OK";
		StatusCodes[201] = "Created";
		StatusCodes[202] = "Accepted";
		StatusCodes[203] = "Non-Authoritative Information";
		StatusCodes[204] = "No Content";
		StatusCodes[205] = "Reset Content";
		StatusCodes[206] = "Partial Content";
		StatusCodes[300] = "Multiple Choices";
		StatusCodes[301] = "Moved Permanently";
		StatusCodes[302] = "Found";
		StatusCodes[303] = "See Other";
		StatusCodes[304] = "Not Modified";
		StatusCodes[305] = "Use Proxy";
		StatusCodes[307] = "Temporary Redirect";
		StatusCodes[400] = "Bad Request";
		StatusCodes[401] = "Unauthorized";
		StatusCodes[402] = "Payment Required";
		StatusCodes[403] = "Forbidden";
		StatusCodes[404] = "Not Found";
		StatusCodes[405] = "Method Not Allowed";
		StatusCodes[406] = "Not Acceptable";
		StatusCodes[407] = "Proxy Authentication Required";
		StatusCodes[408] = "Request Timeout";
		StatusCodes[409] = "Conflict";
		StatusCodes[410] = "Gone";
		StatusCodes[411] = "Length Required";
		StatusCodes[412] = "Precondition Failed";
		StatusCodes[413] = "Payload Too Large";
		StatusCodes[414] = "URI Too Long";
		StatusCodes[415] = "Unsupported Media Type";
		StatusCodes[416] = "Range Not Satisfiable";
		StatusCodes[417] = "Expectation Failed";
		StatusCodes[426] = "Upgrade Required";
		StatusCodes[500] = "Internal Server Error";
		StatusCodes[501] = "Not Implemented";
		StatusCodes[502] = "Bad Gateway";
		StatusCodes[503] = "Service Unavailable";
		StatusCodes[504] = "Gateway Timeout";
		StatusCodes[505] = "HTTP Version Not Supported";
		status = st;
		messagesError = StatusCodes[status];
		type = _type;
	}
	int getStatus() const { return status; }
	int getType() const { return type; }
	virtual ~ThrowErrorCode() throw(){};
	virtual const char *what() const throw()
	{
		return messagesError.c_str();
	}
};

std::string PreProcessingFile();
// Configurations parsingValues(std::string &lines);
template <typename T>
bool Containes(std::vector<T> Vec, T item)
{
	for (typename std::vector<T>::iterator i = Vec.begin(); i != Vec.end(); i++)
	{
		if (*i == item)
			return (true);
	}
	return (false);
}

std::string read_file_content(const std::string &file_path);