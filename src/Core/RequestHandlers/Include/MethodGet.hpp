
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

class MethodGet : public Request
{
public:
	MethodGet(DataPool &headerData);
	~MethodGet();

	bool processRedirection();
	bool handleDirectoryPath();
	bool GetDirectoryHandler();
	bool GetFileHandler();
	virtual bool HandleRequest(std::string &);

};

int directoryStatus(const std::string &);

#endif