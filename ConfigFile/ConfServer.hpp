#pragma once

#include "Location.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iostream>


class ConfServer
{

	std::vector<std::string>	ConfServer_names;
	std::map<int, std::string>	error_pages;
	std::vector<Location>		Locations;
	std::string					host;
	std::string					listen;
	Redirection					redirection;
	std::string					root;
	int							isDefaultLocation;

public:
	ConfServer();
	std::map<int, std::string>	getError_pages() const;
	std::vector<Location>		getLocations() const;
	std::vector<std::string>	getConfServer_names() const;
	std::string					getHost() const;
	std::string 				getListen() const;
	std::string 				getRoot() const;
	Redirection					getRedirection() const;
	int							getDefaultLocation() const;

	void						setHost(std::string const &);
	void						setDefaultLocation(int const &);
	void						setListen(std::string const &);
	void						setRoot(std::string const &);
	void						setConfServer_names(std::string const &);
	void						setRedirection(std::string &, int );
	void						setError_pages(std::map<int , std::string>&);
	void						addLocation(Location const &);
	void						printErrorPages()
	{
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it)
		{
			std::cout << "ConfServer error : " << it->first << " => " << it->second << "\n";
		}
	}
};