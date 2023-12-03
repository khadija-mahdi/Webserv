#pragma once
// #include "../Webserv.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>


struct Redirection
{
	std::string ReturnLocation;
	int statusCode;
};

class Location
{
private:
	std::string root;
	std::vector<std::string> index;
	std::map<int, std::string> error_pages;
	Redirection redirection;		// path , status code
	std::string autoindex;			// on / of
	std::vector<std::string> allow; // method get post delete
	std::string upload;				// on , of
	std::string upload_stor;		// path to stor upload

public:
	std::vector<std::string> getIndex() const;
	std::map<int, std::string> getError_pages() const;
	Redirection getRedirection() const;
	std::string getRoot() const;
	std::string getAutoindex() const;
	std::vector<std::string> getAllow() const;
	std::string getUpload() const;
	std::string getUpload_stor() const;

	void setIndex(std::string const &_name);
	void setRedirection(std::string &_value, int _key);
	void setError_pages(std::string &_value, int _key);
	void setRoot(std::string const &);
	void setAutoindex(std::string const &);
	void setAllow(std::string const &);
	void setUpload(std::string const &);
	void setUpload_stor(std::string const &);
	void printErrorPages()
	{
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it)
		{
			std::cout << "error loc : " << it->first << " => " << it->second << "\n";
		}
	}
};