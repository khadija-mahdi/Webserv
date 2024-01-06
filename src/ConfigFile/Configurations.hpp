#pragma once

#include <map>
#include <vector>
#include <string>
#include "ConfServer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

class Http
{
private:
	std::map<std::string, std::string> MimeTypes;
	std::map<std::string, std::string> ReverseMimeTypes;

	std::vector<ConfServer> ConfServers;
	std::map<int, std::string> error_pages;
	std::string max_body_size;
	std::string default_type;
	

public:
	void setMax_body_size(std::string &);
	void setError_pages(std::map<int, std::string> &);
	void addServer(ConfServer const &);
	void setMimeTypes(std::string const &, std::string const &);
	void SetReverseMimeTypes(std::string const &, std::string const &);
	void setDefault_type(std::string &);

	std::map<std::string, std::string> getMimeTypes() const;
	std::vector<ConfServer> getConfServes();
	unsigned long long getMax_body_size();
	std::string getDefault_type();
	std::map<int, std::string> getError_pages();
	std::string GetFileExtention(std::string ContentType);
};

class Configurations
{
public:
	class Events
	{
		static int worker_connections;

	public:
		static void setWorkerConnections(int);
		static int getWorkerConnections();
	};

	static Http http;
};

class Values
{
	std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
	std::vector<std::string> paths;

public:
	std::vector<std::pair<std::string, std::pair<int, int> > > &BlocksS(const std::string &lines, const std::string &blockName);
	std::vector<std::string> getPaths() const;
	void setPaths(std::string const &);
};
unsigned long long ConvertToBytes(std::string &value);