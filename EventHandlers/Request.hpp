
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
	int									locationIndex;
	int									fd;

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
	void	ParseUrl(Response& );
	void	fillHeaderData();
	void	getCurrentServer(std::vector<ConfServer> &);
	void	getCurrentLocation(std::vector<ConfServer> &, Response &);
	int		pareRedirection(Location &, Response &);
	void	parseHeaderErrors(Response &);
	void	methodParser(Response &);
};

#endif