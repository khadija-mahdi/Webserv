#include "ParseConfig.hpp"

ParseConfig::ParseConfig()
{
	BLOCK_FORMAT = false;
}

void ParseConfig::includeMimeTypes(std::string &_file)
{
	std::map<std::string, std::string> keyValues;
	std::string line = "";
	std::string _lines = "";
	std::fstream fileName(_file.c_str());
	BLOCK_FORMAT = false;
	if (fileName.is_open())
	{
		while (std::getline(fileName, line, '\n'))
		{
			line = skepComment(line);
			if (line.empty() || lineSpace(line))
				continue;
			while (line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
				line = line.erase(line.size() - 1);
			syntaxForm(line);
			_lines += line + "\n";
		}
		fileName.close();
		CurlyBrackets();
		_lines = Blocks(_lines, "type");
		keyValues = extractKeyValuesIN(_lines);
		std::map<std::string, std::string>::iterator it = keyValues.begin();
		for (; it != keyValues.end(); ++it)
		{
			Configurations::http.setMimeTypes(it->second, it->first);
			Configurations::http.SetReverseMimeTypes(it->first, it->second);
		}
	}
	else
		throw std::runtime_error(THROW_COLORED("\ncould not open file." + _file));
}

void ParseConfig::ParseConfig::syntaxForm(std::string &line)
{
	int flag = 0;
	std::string word = "";
	for (int i = 0; line[i]; i++)
	{
		if (line[i] == '{')
		{
			flag++;
			word = line.substr(0, i);
			if (BLOCK_FORMAT == 1)
				isInFormat(word, line);
		}
		if (line[i] == '}')
			flag++;
	}
	if (!flag)
	{
		if (line[line.size() - 1] != ';')
			throw std::runtime_error(THROW_COLORED("syntax error in no ; in end of line :" + line));
	}
}

void ParseConfig::readConfigFile(std::string const &file)
{

	std::string line = "";
	std::ifstream conf(file.c_str());
	if (!conf.is_open())
		throw std::runtime_error(THROW_COLORED("could not open file."));
	while (std::getline(conf, line, '\n'))
	{
		line = skepComment(line);
		if (line.empty() || lineSpace(line))
			continue;
		while (line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
			line = line.erase(line.size() - 1);
		syntaxForm(line);
		lines += line + "\n";
	}
	conf.close();
}

void ParseConfig::CurlyBrackets()
{
	int CloseBrackets = 0;
	int openBrackets = 0;
	for (int i = 0; lines[i]; i++)
	{
		if (lines[i] == '{')
		{
			openBrackets++;
			CloseBrackets++;
		}
		else if (lines[i] == '}')
			CloseBrackets--;
	}
	if (CloseBrackets || !openBrackets)
		throw std::runtime_error(THROW_COLORED("syntax error in curly Brackets."));
}

void ParseConfig::PreProcessingFile(std::string &file)
{
	BLOCK_FORMAT = true;
	readConfigFile(file);
	CurlyBrackets();
}

void ParseConfig::parseEventsBlock(std::string &events)
{
	Configurations::Events event;
	std::string value = "";
	std::string key = "";
	splitKeyValue(events, key, value, "worker_connections", 1);
	if (isDigit(key) || linesLength(events) == 2)
		event.setWorkerConnections(atoi(key.c_str()));
	else
		throw std::runtime_error(THROW_COLORED("events Block error!!"));
}

Location ParseConfig::parseLocationBlock(std::string &locationBlock)
{
	Location location;
	location.setUpload(0);
	locationValues(location, locationBlock);

	return location;
}

void ParseConfig::locationInServer(std::string &ServerBlock, ConfServer &ConfServerConfig)
{
	std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
	Values loc;
	int start, end;
	extractedBlocks = loc.BlocksS(ServerBlock, "location");
	std::vector<std::string> paths = loc.getPaths();
	if (extractedBlocks.size() == 0)
		return;
	for (size_t i = 0; i < extractedBlocks.size(); ++i)
	{
		Location location = parseLocationBlock(extractedBlocks[i].first);
		if (paths[i][0] != '/' || paths[i][paths[i].length() - 1] != '/')
			throw std::runtime_error(THROW_COLORED("Location Path  " + paths[i] + " should end and start with '/' :"));
		location.setPath(paths[i]);
		ConfServerConfig.addLocation(location);
		if (location.getPath() == "/")
			ConfServerConfig.setDefaultLocation(i);
		if (location.getAllow().empty())
			throw std::runtime_error(THROW_COLORED("Location Block  " + location.getPath() + " should have 1 method at least :"));
		start = extractedBlocks[0].second.first;
		end = extractedBlocks[i].second.second;
	}
	std::vector<Location> locations = ConfServerConfig.getLocations();
	for (size_t i = 0; i < locations.size(); i++)
	{
		if ((locations[i].getUpload() == true && locations[i].getUpload_stor().empty()))
			throw std::runtime_error(THROW_COLORED("upload on and no upload_store exist ! "));
	}
	ServerBlock = ServerBlock.substr(0, start) + ServerBlock.substr(end + 1);
}

ConfServer ParseConfig::parseServerBlock(std::string &serverBlock)
{
	ConfServer server;
	std::istringstream BlockStream(serverBlock);
	locationInServer(serverBlock, server);
	singleData(server, serverBlock);

	return server;
}

void ParseConfig::ServerInHttp(std::string &httpBlock, Http &httpConfig)
{
	ConfServer server;
	std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
	Values serv;
	int start, end;

	extractedBlocks = serv.BlocksS(httpBlock, "server");
	if (extractedBlocks.size() < 1)
		throw std::runtime_error(THROW_COLORED("Block server not found ."));
	for (size_t i = 0; i < extractedBlocks.size(); ++i)
	{
		server = parseServerBlock(extractedBlocks[i].first);
		httpConfig.addServer(server);
		start = extractedBlocks[0].second.first;
		end = extractedBlocks[i].second.second;
	}

	httpBlock = httpBlock.substr(0, start) + httpBlock.substr(end + 1);
}

void ParseConfig::parseHttpBlock(std::string &httpBlock)
{
	std::string line = "";
	std::string path = "";
	std::map<int, std::string> errs;
	std::map<std::string, std::string> cgi;
	std::map<std::string, std::string> values;
	ServerInHttp(httpBlock, Configurations::http);
	std::istringstream BlockStream(httpBlock);
	values = extractKeyValues(httpBlock, errs, cgi);
	std::map<std::string, std::string>::iterator it = values.begin();
	for (; it != values.end(); ++it)
	{
		if (it->first == "include")
			includeMimeTypes(it->second);
		else if (it->first == "default_type")
			Configurations::http.setDefault_type(it->second);
		else if (it->first == "client_max_body_size")
			Configurations::http.setMax_body_size(it->second);
		else if (it->first == "error_page")
		{
			if (errs.size())
				Configurations::http.setError_pages(errs);
		}
		else
			throw std::runtime_error(THROW_COLORED(" Http wrong key ! .\n"));
	}
}

void ParseConfig::parsingValues()
{
	std::istringstream BlockStream(lines);
	size_t pos = lines.find("events");
	if (pos != std::string::npos)
	{
		std::vector<std::string> events = BlocksExtra(lines, "events");
		if (events.size() != 1)
			throw std::runtime_error(THROW_COLORED("Events Block Duplicate !"));
		std::string event = events[0];
		parseEventsBlock(event);
		pos = 0;
	}
	pos = lines.find("http");
	if (pos != std::string::npos)
	{
		std::vector<std::string> https = BlocksExtra(lines, "http");
		if (https.size() != 1)
			throw std::runtime_error(THROW_COLORED("Http Block Duplicate !"));
		std::string httpBlock = https[0];
		parseHttpBlock(httpBlock);
	}
	else
		throw std::runtime_error(THROW_COLORED("NO Http Block Found !"));
}

void ParseConfig::pacingConfigFile(char *_file)
{
	std::string file = _file;
	PreProcessingFile(file);
	parsingValues();
	std::vector<ConfServer> Servers = Configurations::http.getConfServes();
	std::vector<Location> locations;
	for(size_t i = 0; i < Servers.size(); i++){
		locations = Servers[i].getLocations();
		if(Servers[i].getRoot().empty() && !locations.size())
			throw std::runtime_error(THROW_COLORED("Must be one root in Location or In the server at least !" + Servers[i].getListen()));
		for (size_t j = 0; j < locations.size(); j++)
		{
			if(Servers[i].getRoot().empty() && locations[j].getRoot().empty())
				throw std::runtime_error(THROW_COLORED("Must be one root in Location or In the server at least !" + Servers[i].getListen()));
		}
	}
}