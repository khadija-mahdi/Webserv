
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Webserv.hpp"
#include "Response.hpp"

class Response;
enum
{
	HEADERS_STAGE = 1,
	REQUEST_HANDLER_STAGE = 0
};

class Request{
	std::map<std::string, std::string>	Headers;
	int									REQUEST_STATE;
	std::string							Buffer;
	std::string							Method;
	std::string							Path;
	int									serverIndex;
	int								locationIndex;
	int									fd;
	std::string							url;

public:
	Request();
	~Request();

	std::string							getBuffer() const;
	std::string							getMethod() const;
	std::string							getPath() const;
	std::map<std::string, std::string>	getHeaders() const;

	void	setHeaders(std::string &_, std::string );
	void	setBuffer(std::string const &);
	void	setMethod(std::string const &);
	void	setPath(std::string const &);
	bool	RequestParser(std::string , Response& );
	void	ParseHeaders(Response& );
	int		ParseUrl(Response& );
	void	fillHeaderData();
	void	getCurrentServer(std::vector<ConfServer> &);
	int		getCurrentLocation(std::vector<ConfServer> &, Response &);
	int		pareRedirection(Location &, Response &);
	void	parseHeaderErrors(Response &);
	void	handleDirectoryPath(Response &);
	int		methodParser(Response &);

    void  	printHeaderdata(){

	fillHeaderData();
		HeaderIterator it;
		it = Headers.begin();
		DEBUGOUT(1, COLORED("Path [" << this->Path << "]\n", Yellow));
		DEBUGOUT(1, COLORED("Method [" << this->Method << "]\n", Yellow));
		for (; it != Headers.end(); ++it)
		DEBUGOUT(1, COLORED(it->first << "----> " << it->second << " . \n\n", Yellow));

	}
};

#endif