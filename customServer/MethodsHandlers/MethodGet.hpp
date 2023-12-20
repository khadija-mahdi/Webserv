
#ifndef METHODGET_HPP
#define METHODGET_HPP

#include "../RequestParser.hpp"

enum
{
	DIRE = 1,
	VALID_PATH = 2,
	NOT_DIR = 0,
	NOT_VALID_PATH = -1,
	FORBIDDEN_READ = 4,
};

class MethodGet
{
private:
	HeaderData		*headerData;
	int				fd;
public:
	MethodGet(HeaderData *headerData);
	MethodGet(){}
	~MethodGet();
	bool	handleDirectoryPath();
	bool	GetMethodHandler();
	bool	checkErrorPage(int const &, std::map<int, std::string>& , int);
	bool	checkInHttp(int const &, int);
};

int directoryStatus(const std::string& );

#endif