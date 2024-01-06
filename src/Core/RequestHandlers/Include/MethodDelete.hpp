
#ifndef METHODDELETE_HPP
#define METHODDELETE_HPP

// #include "../RequestParser.hpp"
#include "Request.hpp"

class Request;
class MethodDelete : public Request
{
public:
	MethodDelete();
	MethodDelete(DataPool &headerData);
	~MethodDelete();

	bool DeleteMethodHandler();
	bool DeleteDirectoryHandler();
	bool DeleteFileHandler();
	virtual bool HandleRequest(std::string &data);
};

#endif