
#ifndef METHODDELETE_HPP
#define METHODDELETE_HPP

// #include "../RequestParser.hpp"
#include "../Request.hpp"

class Request;
class MethodDelete : public Request
{
private:
	HeaderData		*headerData;
	int				fd;
public:
	MethodDelete();
	MethodDelete(HeaderData *headerData);
	~MethodDelete();

	bool	DeleteMethodHandler();
	bool	DeleteDirectoryHandler();
	bool	DeleteFileHandler();
	virtual bool	processRequest();

};




#endif