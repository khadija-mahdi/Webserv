#include "ParseConfig.hpp"
std::stringstream errorMessage;

void processErrorPage(const std::string &line, std::string &value, int &errorCode)
{
	std::istringstream iss(line);
	std::string firstWord, secondWord;
	iss >> firstWord;
	if (!isDigitStr(firstWord) || (atoi(firstWord.c_str()) < 100 || atoi(firstWord.c_str()) > 599))
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "The status code not correct  : " << atoi(firstWord.c_str()) << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
	errorCode = atoi(firstWord.c_str());
	iss >> secondWord;

	std::string remaining;
	iss >> remaining;
	if (!remaining.empty())
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "Need just the status code and Path , Nothing More !"
					 << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
	value = secondWord;
}

std::map<std::string, std::string> extractKeyValuesIN(const std::string &Block)
{
	std::map<std::string, std::string> keyValues;
	std::istringstream BlockStream(Block);
	std::string line;
	int i = 0;

	while (std::getline(BlockStream, line))
	{
		std::vector<std::string> parts;
		std::istringstream lineStream(line);
		std::string part;

		while (lineStream >> part)
			parts.push_back(part);
		if (parts[0] == "{" || parts[0] == "}")
			continue;
		else if (parts.size() >= 2)
		{
			std::string key;
			for (size_t i = 1; i < parts.size(); i++)
				key += parts[i] + " ";
			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);

			if (!key.empty() && key[key.size() - 1] == ';')
				key.resize(key.length() - 1);

			std::string values = parts[0];
			std::istringstream keyStream(key);
			std::string individualKey;
			while (std::getline(keyStream, individualKey, ' '))
				keyValues[individualKey] = values;
		}
		else
			throw std::runtime_error("syntax error values value : " + Block);
	}

	return keyValues;
}

bool processRedirection(std::string &path, int &status, std::string &key)
{

	std::istringstream iss(key);

	std::string word;
	int i = 0;

	while (iss >> word)
	{
		if (isDigitStr(word) && i == 0)
		{
			status = atoi(word.c_str());
			i++;
		}
		else if (!isDigitStr(word))
		{
			i++;
			path = word;
		}
	}
	if (status !=  301 && status !=  302)
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "Please Correct The Redirection Syntax ! entree 301 or 302 in " << status << " " << path 
					 << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
	if (i == 1)
	{
		status = 301;
	}
	if (i == 1 || (i == 2) && status >= 100 && status <= 599)
		return true;
	else
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "Please Correct The Redirection Syntax !" << key << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
	return 0;
}

bool processErrors(std::string &path, int &status, std::string &key)
{

	std::istringstream iss(key);

	std::string word;
	int i = 0;
	while (iss >> word)
	{
		if (isDigitStr(word) && i == 0)
		{
			status = atoi(word.c_str());
			i++;
		}
		else if (!isDigitStr(word) && i == 1)
		{
			i++;
			path = word;
		}
		else if (i != 2)
			throw std::runtime_error("not a true error pages syntax: " + key);
	}
	if ((i == 2) && status >= 100 && status <= 599)
		return true;
	else
		throw std::runtime_error("not a true error pages syntax: " + key);
	return 0;
} //? it's does not wor

std::map<std::string, std::string> extractKeyValues(const std::string &Block, std::map<int, std::string> &errors, std::map<std::string, std::string> &cgi)
{
	std::map<std::string, std::string> keyValues;
	std::istringstream BlockStream(Block);
	std::string line;
	if (Block.empty())
		return keyValues;

	while (std::getline(BlockStream, line))
	{
		std::vector<std::string> parts;
		std::istringstream lineStream(line);
		std::string part;
		while (lineStream >> part)
		{
			parts.push_back(part);
		}
		if (parts[0] == "{" || parts[0] == "}")
			continue;

		if (parts.size() >= 2)
		{
			std::string key = parts[0];
			std::string value;
			for (size_t i = 1; i < parts.size(); i++)
			{
				value += parts[i] + " ";
			}
			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			if (!value.empty() && value[value.size() - 1] == ';')
			{
				value.resize(value.length() - 1);
			}
			if (key == "error_page")
			{
				std::string path = "";
				int status = 0;
				processErrorPage(value, path, status);
				errors[status] = path;
			}
			if (key == "cgi")
			{
				std::istringstream iss(value);
				std::string accept, cgi_path;
				iss >> accept >> cgi_path;
				if (accept == "" || cgi_path == "")
				{
					errorMessage << "\033[1;" << Red << "mError: "
								<< "Location Block Wrong Key Found :" << key + "\t" + value << "\033[0m" << std::endl;
					throw std::runtime_error(errorMessage.str());
				}
				cgi[accept] = cgi_path;
			}
			keyValues[key] = value;
		}
		else
		{
			errorMessage << "\033[1;" << Red << "mError: "
						 << "Syntax Error in  :" << line << "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
	}
	return keyValues;
}

void splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag)
{
	std::string keyValue = "";
	size_t pos = block.find(word);
	if (pos == std::string::npos && flag == 1)
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "Block " << block << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
	size_t end = block.find("}");
	// if(pos = std::string::npos)
	//     return;
	for (int i = pos; i < end; i++)
		keyValue += block[i];
	int vstart = 0;
	int vend = keyValue.find("\t");
	value = keyValue.substr(vstart, vend);
	for (; keyValue[vend] && keyValue[vend] == '\t'; vend++)
		;
	int kEnd = keyValue.find(";");
	key = keyValue.substr(vend, kEnd - vend);
}

std::string Blocks(const std::string &lines, const std::string &blockName)
{
	int pos = 0;
	std::string extractedBlock = "";

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
			extractedBlock += lines.substr(openBrace, closeBrace - openBrace);
			pos = closeBrace;
		}
		else
		{
			break;
		}
	}
	return extractedBlock;
}

std::vector<std::string> BlocksExtra(const std::string &lines, const std::string &blockName)
{
	int pos = 0;
	std::vector<std::string> extractedBlocks;

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
				bracesCount++;
			else if (lines[closeBrace] == '}')
				bracesCount--;
			closeBrace++;
		}
		if (bracesCount == 0)
		{
			extractedBlocks.push_back(lines.substr(openBrace, closeBrace - openBrace));
			pos = closeBrace;
		}
		else
			break;
	}
	return extractedBlocks;
}

void singleData(ConfServer &ConfServer, std::string &ConfServerBlock)
{
	std::map<std::string, std::string> values;
	std::map<int, std::string> errs;
	std::map<std::string, std::string> cgi;
	std::istringstream BlockStream(ConfServerBlock);
	std::string line = "";
	int flag = 0;
	int st;
	std::string path = "";

	values = extractKeyValues(ConfServerBlock, errs, cgi);
	std::map<std::string, std::string>::iterator it = values.begin();
	for (; it != values.end(); ++it)
	{
		if (it->second == "")
		{
			errorMessage << "\033[1;" << Red << "mError: "
				<< "Not a Valid value : " << it->second << "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
		if (it->first == "listen")
		{
			flag++;
			int value;
			if (isDigitStr(it->second))
			{
				int value = atoi(it->second.c_str());
				if (value > 0 && value <= 65536)
					ConfServer.setListen(SSTR(value));
			}
			else
			{
				errorMessage << "\033[1;" << Red << "mError: "
							 << "Not a Valid Port : " << it->second << "\033[0m" << std::endl;
				throw std::runtime_error(errorMessage.str());
			}
		}
		else if (it->first == "host")
			ConfServer.setHost(it->second.c_str());
		else if (it->first == "root")
			ConfServer.setRoot(it->second.c_str());
		else if (it->first == "server_names")
		{
			std::string key = it->second;
			std::istringstream iss(key);
			std::string word;
			while (iss >> word)
				ConfServer.setConfServer_names(word);
		}
		else if (it->first == "return" && processRedirection(path, st, it->second))
			ConfServer.setRedirection(path, st);
		else if (it->first == "error_page")
		{
			if (errs.size())
				ConfServer.setError_pages(errs);
		}
		else if (it->first == "index")
		{
			std::string key = it->second;
			std::istringstream iss(key);
			std::string word;
			while (iss >> word)
				ConfServer.setIndex(word);
		}
		else
		{
			errorMessage << "\033[1;" << Red << "mError: "
						 << "Server Block Wrong Key found !"
						 << "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
	}
	if (flag != 1)
	{
		errorMessage << "\033[1;" << Red << "mError: "
					 << "Server Block obligation Keys Not Found !"
					 << "\033[0m" << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
}

void locationValues(Location &location, std::string &locationBlock)
{
	std::map<std::string, std::string> values;
	std::map<int, std::string> errs;
	std::map<std::string, std::string> cgi;
	int st;
	std::string path = "";
	values = extractKeyValues(locationBlock, errs, cgi);
	if (values.size() == 0)
		return;
	std::map<std::string, std::string>::iterator it = values.begin();
	for (; it != values.end(); ++it)
	{

		if (it->first == "root")
			location.setRoot(it->second.c_str());
		else if (it->first == "index")
		{
			std::string key = it->second;
			std::istringstream iss(key);
			std::string word;
			while (iss >> word)
				location.setIndex(word);
		}
		else if (it->first == "autoindex")
		{
			if (it->second == "on" || it->second == "off")
				location.setAutoindex(it->second);
			else
			{
				errorMessage << "\033[1;" << Red << "mError: "
							 << "Location Block : " << it->first << "\033[0m" << std::endl;
				throw std::runtime_error(errorMessage.str());
			}
		}
		else if (it->first == "allow")
		{
			std::string method = it->second;
			std::istringstream allow(method);
			std::string methodName;
			while (allow >> methodName)
			{
				if (methodName == "GET" || methodName == "POST" || methodName == "DELETE" || methodName == "ALL")
				{
					if (methodName == "ALL")
					{
						location.setAllow("POST");
						location.setAllow("GET");
						location.setAllow("DELETE");
					}
					else
						location.setAllow(methodName);
				}
				else
				{
					errorMessage << "\033[1;" << Red << "NO Method called : " << methodName << "\033[0m" << std::endl;
					throw std::runtime_error(errorMessage.str());
				}
			}
			location.setAllow(it->second.c_str());
		}
		else if (it->first == "upload")
		{
			if (it->second == "on" || it->second == "off")
				location.setUpload(it->second);
			else
			{
				errorMessage << "\033[1;" << Red << "mError: "
							 << "Location Block Wrong Value : " << it->first << " = " << it->second << "\033[0m" << std::endl;
				throw std::runtime_error(errorMessage.str());
			}
		}
		else if (it->first == "upload_stor")
			location.setUpload_stor(it->second.c_str());
		else if (it->first == "return" && processRedirection(path, st, it->second))
			location.setRedirection(path, st);
		else if (it->first == "cgi")
		{
			if (cgi.size())
				location.setCgiAccept(cgi);
		}
		else if (it->first == "error_page")
		{
			if (errs.size())
				location.setError_pages(errs);
		}
		else
		{
			errorMessage << "\033[1;" << Red << "mError: "
						 << "Location Block Wrong Key Found :" << it->first << "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
	}
}
