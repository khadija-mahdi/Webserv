
#ifndef METHODDELETE_HPP
#define METHODDELETE_HPP

// #include "../RequestParser.hpp"
#include "Request.hpp"

class Request;
class MethodDelete : public Request
{
public:
	MethodDelete(DataPool &headerData);
	~MethodDelete();

	bool DeleteDirectoryHandler();;
	virtual bool HandleRequest(std::string &);
};

#endif