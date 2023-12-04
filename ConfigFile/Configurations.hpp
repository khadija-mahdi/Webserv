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
	std::string max_body_size;
	std::string default_type;
	std::map<int, std::string> error_pages;
	std::vector<ConfServer> ConfServers;
	std::map<std::string, std::string> Includes;

public:
	void setMax_body_size(std::string &);
	void setError_pages(std::string &_value, int _key);
	void addServer(ConfServer const &);
	void setIncludes(std::string &, std::string);
	void setDefault_type(std::string &);

	std::vector<ConfServer> getConfServes()
	{
		return ConfServers;
	}
	std::string getMax_body_size();
	std::string getDefault_type();
	std::map<int, std::string> getError_pages();
	std::map<std::string, std::string> getIncludes();

	void printErrorPages()
	{
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it)
		{
			std::cout << "http error : " << it->first << " => " << it->second << "\n";
		}
	}
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

public:
	std::vector<std::pair<std::string, std::pair<int, int> > > &BlocksS(const std::string &lines, const std::string &blockName)
	{
		int pos = 0;

		while (pos < lines.length())
		{
			int start = lines.find(blockName, pos);
			if (start == std::string::npos)
				break;
			int openBrace = lines.find('{', start);
			if (openBrace == std::string::npos)
				break;
			int closeBrace = openBrace + 1;
			int bracesCount = 1;

			while (bracesCount > 0 && closeBrace < lines.length())
			{
				if (lines[closeBrace] == '{')
				{
					bracesCount++;
				}
				else if (lines[closeBrace] == '}')
				{
					bracesCount--;
				}
				closeBrace++;
			}
			if (bracesCount == 0)
			{
				std::string block = lines.substr(openBrace, closeBrace - openBrace);
				extractedBlocks.push_back(std::make_pair(block, std::make_pair(start, closeBrace)));
				pos = closeBrace;
			}
			else
			{
				break;
			}
		}
		return extractedBlocks;
	}
	void printPairVect(std::string &lines)
	{
		extractedBlocks = BlocksS(lines, "server");
		std::cout << extractedBlocks.size() << std::endl;
		for (size_t i = 0; i < extractedBlocks.size(); ++i)
		{
			std::cout << "Element " << i + 1 << ": ";
			std::cout << extractedBlocks[i].first << " (" << extractedBlocks[i].second.first << ", " << extractedBlocks[i].second.second << ")\n";
		}
	}
};

std::map<std::string, std::string> extractKeyValues(const std::string &Block);
std::vector<std::string> BlocksExtra(const std::string &lines, const std::string &blockName);
std::string Blocks(const std::string &lines, const std::string &blockName);
bool isDigit(std::string &value);
int linesLength(std::string &Block);
void splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag);
void singleData(ConfServer &server, std::string &serverBlock);
void includeMimeTypes(std::string &fileName);
void CurlyBrackets(std::string &lines);
void readConfigFile(std::string &lines, int flag);
void syntaxForm(std::string &line, int check);
std::string &skepComment(std::string &line);
bool lineSpace(std::string &line);
void locationValues(Location &location, std::string &locationBlock);
bool processErrors(std::string &path, int &status, std::string &key);
bool isDigitStr(std::string &value);
std::string PreProcessingFile();
void pacingConfigFile();