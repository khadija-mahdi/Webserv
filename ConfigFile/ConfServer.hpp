#pragma once

#include "Location.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iostream>


class ConfServer
{

	std::string host; // localhost / 127.0.0.1 /0.0.0.0
	std::string	listen;		  //(port)
	std::vector<std::string> ConfServer_names;
	std::map<int, std::string> error_pages;
	Redirection redirection; // path , status code
	std::string root;
	std::vector<Location> Locations;

public:
	std::string getHost() const;
	std::string getListen() const;
	std::string getRoot() const;
	std::vector<std::string> getConfServer_names() const;
	std::map<int, std::string> getError_pages() const;
	Redirection getRedirection() const;

	void setHost(std::string const &);
	void setListen(std::string const &);
	void setRoot(std::string const &);
	void setConfServer_names(std::string const &_name);
	void setRedirection(std::string &_value, int _key);
	void setError_pages(std::string &_value, int _key);
	void addLocation(Location const &);
	void printErrorPages()
	{
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it)
		{
			std::cout << "ConfServer error : " << it->first << " => " << it->second << "\n";
		}
	}
};