#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>

struct Redirection
{
	std::string	ReturnLocation;
	int			statusCode;
};

class Location
{
private:
	std::string							cgiAccepts;
	std::string							cgiPath;
	std::map<std::string, std::string>	cgiAccept;
	std::vector<std::string>			index;
	std::map<int, std::string>			error_pages;
	std::vector<std::string>			allow;
	std::string							root;
	Redirection							redirection;
	std::string							autoindex;
	std::string							upload;
	std::string							upload_stor;
	std::string							path;

public:
	std::map<std::string, std::string>	getCgiAccept() const;
	std::map<int, std::string>			getError_pages() const;
	std::vector<std::string>			getAllow() const;
	std::vector<std::string>			getIndex() const;
	Redirection							getRedirection() const;
	std::string							getRoot() const;
	std::string							getPath() const;
	std::string							getAutoindex() const;
	std::string							getUpload() const;
	std::string							getUpload_stor() const;

	void	setCgi(std::string &_value, std::string _key);
	void	setCgiAccept(std::map<std::string, std::string> &);
	void	setIndex(std::string const &);
	void	setRedirection(std::string &, int) ;
	void 	setError_pages(std::map<int , std::string>&);
	void 	setRoot(std::string const &);
	void 	setPath(std::string const &);
	void 	setAutoindex(std::string const &);
	void 	setAllow(std::string const &);
	void 	setUpload(std::string const &);
	void 	setUpload_stor(std::string const &);
	void	printErrorPages()
	{
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it)
		{
			std::cout << "error loc : " << it->first << " => " << it->second << "\n";
		}
	}
};