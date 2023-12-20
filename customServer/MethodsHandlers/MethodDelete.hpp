
#ifndef METHODDELETE_HPP
#define METHODDELETE_HPP

#include "../RequestParser.hpp"

class MethodDelete
{
private:
	HeaderData		*headerData;
	int				fd;
public:
	MethodDelete();
	MethodDelete(HeaderData *headerData);
	~MethodDelete();

	bool	checkErrorPage(int const &, std::map<int, std::string>& , int);
	bool	checkInHttp(int const &, int);
	bool	DeleteMethodHandler();
	bool	DeleteDirectoryHandler();
	bool	DeleteFileHandler();
};




#endif