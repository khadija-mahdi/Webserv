
#ifndef METHODGET_HPP
#define METHODGET_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../../Include/RequestParser.hpp"
#include "Request.hpp"

class Request;
enum
{
	DIRE = 1,
	VALID_PATH = 2,
	NOT_DIR = 0,
	NOT_VALID_PATH = -1,
	FORBIDDEN_READ = 4,
};

class MethodGet : public Request
{
private:
	DataPool *headerData;
	int fd;
	std::vector<std::string> env;
	std::vector<std::string> arguments;

public:
	MethodGet(DataPool *headerData);
	MethodGet();
	~MethodGet();
	bool handleDirectoryPath();
	bool GetMethodHandler();
	bool GetDirectoryHandler();
	bool GetFileHandler();
	virtual bool HandleRequest(std::string &data);
};

int directoryStatus(const std::string &);

#endif