#pragma once

#include "../Webserv.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"


enum
{
	REDIRECTION = 0,
	LISTEN_DIR = 2,
	CHUNKED	= 1,
	DEFAULT_ERRORS = 3,
};

class Response
{
private:
	std::map<int, std::string>	StatusCodes;
	std::string					responseBuffer;
	std::string					ContentType;
	std::string					responseHeader;
	std::string					Path;
	bool						Headers_Stage;
	HeaderData					*headerData;

public:
	Response(){};
	Response(HeaderData	*headerData);
	~Response(){};

	std::string&	httpheader(int const &);
	std::string 	getResponseBuffer() const;
	std::string		getResponseHeader() const;
	std::string		DefaultErrorPage(int );
	std::string		listenDirectory(const std::string &);

	void 			setResponseHeader(std::string const &);
	void			setResponseBuffer(std::string const &);
	void			clearResponseBuffer();
	void			getContentType(std::string const &);
	int				PrepareNextChunk();
	int				sendChunkResponse(int const &);
	int				sendResponse(int const &);
	bool			Write(std::string const &, int const &);
};