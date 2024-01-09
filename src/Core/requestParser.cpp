#include "Include/RequestParser.hpp"


RequestParser::RequestParser() {} 

RequestParser::~RequestParser() {}

void urlDecoding(std::string &Path)
{
	std::ostringstream decoded_ss;
	int Flag = 0;

	for (std::size_t i = 0; i < Path.length(); ++i)
	{
		char currentCh = Path[i];
		if (currentCh == '%')
		{
			if (i + 2 < Path.length())
			{
				std::string hexSp = Path.substr(i + 1, 2);
				if (isDigitStr(hexSp))
				{
					Flag++;
					std::istringstream hexStream(hexSp);
					int hexValue;
					hexStream >> std::hex >> hexValue;
					if (!hexStream.fail())
					{
						decoded_ss << static_cast<char>(hexValue);
						i += 2;
					}
					else
						decoded_ss << currentCh;
				}
			}
			else if (Flag > 0)
				decoded_ss << currentCh;
		}
		else
			decoded_ss << currentCh;
	}
	Path = decoded_ss.str();
}

void RequestParser::printHeaderdata(DataPool &headerData)
{
	std::map<std::string, std::string>::iterator it;
	it = headerData.Headers.begin();
	DEBUGMSGT(1, COLORED("Path [" << headerData.Path << "]\n", Yellow));
	DEBUGMSGT(1, COLORED("Method [" << headerData.Method << "]\n", Yellow));
	for (; it != headerData.Headers.end(); ++it)
		DEBUGMSGT(1, COLORED(it->first << "----> " << it->second << " . \n\n", Yellow));
}

void RequestParser::getContentType(DataPool &headerData)
{
	size_t pos = headerData.Path.find(".");
	if (pos != std::string::npos)
	{
		std::string extention = headerData.Path.substr(pos + 1, headerData.Path.length());
		std::map<std::string, std::string> Types = Configurations::http.getMimeTypes();

		if (Types.find(extention) != Types.end())
			headerData.response.contentType = Types[extention];
		if (Configurations::http.getDefault_type().empty())
			headerData.response.contentType = "application/octet-stream";
		else
			headerData.response.contentType = Configurations::http.getDefault_type();
	}
}

void RequestParser::fillHeaderData(DataPool &headerData)
{

	std::istringstream BufferStream(headerData.Buffer);
	std::string requestLine;
	std::getline(BufferStream, requestLine);

	std::istringstream requestLineStream(requestLine);
	requestLineStream >> headerData.Method >> headerData.Path;
	if (int pos = headerData.Path.find(" ") != std::string::npos)
		headerData.Path = headerData.Path.substr(pos);
	urlDecoding(headerData.Path);
	headerData.url = headerData.Path;
	std::string header;
	while (std::getline(BufferStream, header) && !header.empty())
	{
		size_t colonPos = header.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = header.substr(0, colonPos);
			std::string value = header.substr(colonPos + 2);
			size_t lastCharPos = value.find_last_not_of("\r\n");
			if (lastCharPos != std::string::npos)
				value.erase(lastCharPos + 1);
			headerData.Headers[key] = value;
		}
	}
	size_t index;
	if ((index = headerData.url.find("?")) != std::string::npos)
        headerData.Query = headerData.url.substr(index + 1);
}

void RequestParser::getCurrentServer(std::vector<ConfServer> &confServers, DataPool &headerData)
{
	size_t pos = headerData.Headers["Host"].find(":");
	std::string server_name = headerData.Headers["Host"].substr(0, pos);
	std::string Port = headerData.Headers["Host"].substr(pos + 1);
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < confServers.size(); i++)
	{
		serverNames = confServers[i].getConfServer_names();
		int pos = std::find(serverNames.begin(), serverNames.end(), server_name) != serverNames.end();
		if (pos || confServers[i].getHost() == server_name)
		{
			if (confServers[i].getListen() == Port)
			{
				headerData.currentServer = confServers[i];
				break;
			}
		}
		else if (confServers[i].getListen() == Port)
		{
			headerData.currentServer = confServers[i];
			break;
		}
	}
}

bool RequestParser::redirectionType(std::vector<Location> &confLocation, DataPool &headerData)
{
	for (size_t i = 0; i < confLocation.size(); i++)
	{
		if (confLocation[i].getPath() == headerData.currentLocation.getRedirection().ReturnLocation)
			return 0;
	}
	return 1;
}

void RequestParser::getCurrentLocationIndex(std::vector<Location> &confLocation, DataPool &headerData)
{
	size_t i = 0;
	size_t start = 0, end = 0;
	if ((headerData.Path) == "/")
	{
		headerData.url = headerData.url + "/";
		start = headerData.Path.find(confLocation[i].getPath());
		if (start != std::string::npos)
			headerData.locationIndex = headerData.currentServer.getDefaultLocation();
	}
	else
	{
		for (; i < confLocation.size(); i++)
		{
			std::string locPath = confLocation[i].getPath();
			start = headerData.Path.find(confLocation[i].getPath());
			if (start != std::string::npos)
			{
				headerData.locationIndex = i;
				break;
			}
		}
	}
	if (headerData.locationIndex != -1)
	{
		headerData.currentLocation = confLocation[headerData.locationIndex];
		if (!headerData.currentLocation.getRedirection().ReturnLocation.empty()){
			headerData.REDIRECTION_STAGE = true;
			headerData.Path = headerData.currentLocation.getRedirection().ReturnLocation;
			headerData.response.StatusCode = headerData.currentLocation.getRedirection().statusCode;
		}
		end = confLocation[headerData.locationIndex].getPath().length();
		headerData.newRoot = headerData.Path.substr(start + end);
	}
}

int RequestParser::ParseUrl(DataPool &headerData)
{

	if (headerData.locationIndex == -1)
	{
		if (!headerData.currentServer.getRoot().empty())
		{
			size_t lastSlashPos = headerData.Path.find_last_of("/");
			if (lastSlashPos != std::string::npos)
			{
				std::string newRoot = headerData.Path.substr(lastSlashPos);
				headerData.Path = headerData.currentServer.getRoot() + newRoot;
			}
			DEBUGMSGT(1, COLORED("\n the current Location is  parse url: " << headerData.Path << "\n", Cyan));
		}
		return 1;
	}
	if (!headerData.currentLocation.getRoot().empty())
		headerData.Path = headerData.currentLocation.getRoot() + headerData.newRoot;
	else if (!headerData.currentServer.getRoot().empty())
		headerData.Path = headerData.currentServer.getRoot() + headerData.newRoot;
	return 0;
}

std::string GetHeaderAttr(HeadersType &Headers, std::string name)
{
	HeadersIterator it;

	it = Headers.find(name);
	if (it != Headers.end())
		return it->second;
	return ("");
}

void RequestParser::ParseRequest(DataPool &headerData)
{
	fillHeaderData(headerData);
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();
	getCurrentServer(confServers, headerData);
	std::vector<Location> confLocation = headerData.currentServer.getLocations();
	if (confLocation.size() > 0)
		getCurrentLocationIndex(confLocation, headerData);
	if (!headerData.REDIRECTION_STAGE && !headerData.currentServer.getRedirection().ReturnLocation.empty()){
		headerData.REDIRECTION_STAGE = true;
		headerData.Path = headerData.currentServer.getRedirection().ReturnLocation;
		headerData.response.StatusCode = headerData.currentServer.getRedirection().statusCode;

	}
	if (!headerData.REDIRECTION_STAGE)
		ParseUrl(headerData);
	if (directoryStatus(headerData.Path.substr(1)) >= 1 && headerData.Path[0] == '/')
		headerData.Path = headerData.Path.substr(1);
}
