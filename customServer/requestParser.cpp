#include "RequestParser.hpp"

RequestParser::RequestParser(/* args */){}

RequestParser::~RequestParser(){}


void urlDecoding(std::string &Path)
{
	std::ostringstream decoded_ss;
	int Flag = 0;

	for (std::size_t i = 0; i <  Path.length(); ++i)
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
					{
						decoded_ss << currentCh;
					}
				}
			}
			else if (Flag > 0)
			{
				decoded_ss << currentCh; // Append '%' as is
			}
		}
		else
		{
			decoded_ss << currentCh;
		}
	}
	Path = decoded_ss.str();
}

void  	RequestParser::printHeaderdata(HeaderData &headerData){
	std::map<std::string, std::string>::iterator it;
	it = headerData.Headers.begin();
	DEBUGOUT(1, COLORED("Path [" << headerData.Path << "]\n", Yellow));
	DEBUGOUT(1, COLORED("Method [" << headerData.Method << "]\n", Yellow));
	for (; it != headerData.Headers.end(); ++it)
		DEBUGOUT(1, COLORED(it->first << "----> " << it->second << " . \n\n", Yellow));
}

void RequestParser::fillHeaderData(HeaderData &headers)
{

	std::istringstream BufferStream(headers.Buffer);
	std::string requestLine;
	std::getline(BufferStream, requestLine);

	std::istringstream requestLineStream(requestLine);
	requestLineStream >> headers.Method >>headers.Path;
	if (int pos =headers.Path.find(" ") != std::string::npos)
		headers.Path = headers.Path.substr(pos);
	urlDecoding(headers.Path);
	headers.url = headers.Path;

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
			headers.Headers[key] = value;
		}
	}
}

void RequestParser::getCurrentServer(std::vector<ConfServer> &confServers,HeaderData &headerData)
{
	size_t pos = headerData.Headers["Host"].find(":");
	std::string server_name = headerData.Headers["Host"].substr(0, pos);
	std::string Port = headerData.Headers["Host"].substr(pos + 1);
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < confServers.size(); i++)
	{	serverNames = confServers[i].getConfServer_names();
		int pos = std::find(serverNames.begin(), serverNames.end(),server_name) != serverNames.end();
		if (pos || confServers[i].getHost() == server_name)
		{
			if (confServers[i].getListen() == Port)
			{
				headerData.currentServer = confServers[i];
				break;
			}
		}
		else if (confServers[i].getListen() == Port){
			headerData.currentServer = confServers[i];
			break;

		}
	}
}

void RequestParser::getCurrentLocationIndex(std::vector<Location> &confLocation, HeaderData &headerData)
{
	size_t i = 0;
	int start = 0 ,end = 0;
	// headerData.Path = "/" + headerData.Path;
	if (headerData.Path == "/")
		headerData.locationIndex = headerData.currentServer.getDefaultLocation();
	else{
		for (;i < confLocation.size(); i++)
		{
			if (confLocation[i].getPath() == "/")
				continue;
			std::string locPath = confLocation[i].getPath();
			size_t lastCharPos = locPath[locPath.length() + 1];
			start =headerData.Path.find(confLocation[i].getPath());
			if (start != std::string::npos){
				headerData.locationIndex = i;
				break;
			}
		}
	}
	if (headerData.locationIndex != -1){
			end = confLocation[headerData.locationIndex].getPath().length();
			headerData.newRoot =headerData.Path.substr(start + end);
		headerData.currentLocation= confLocation[headerData.locationIndex];
	}
}

int RequestParser::ParseUrl(HeaderData &headerData) {
	int pos1 = 0;

	DEBUGOUT(1, COLORED("\n the current Server is  : " << headerData.currentServer.getListen() << "\n", Cyan));
	if (headerData.currentServer.getLocations().size() == 0){
		if (!headerData.currentServer.getRoot().empty())
			headerData.Path = headerData.currentServer.getRoot();
	}
	else{
		if (!headerData.currentLocation.getRoot().empty())
			headerData.Path = headerData.currentLocation.getRoot()  + headerData.newRoot;
		else if (!headerData.currentServer.getRoot().empty())
			headerData.Path = headerData.currentServer.getRoot() + headerData.newRoot ;
	}
	DEBUGOUT(1, COLORED("THE New Path : " << headerData.Path << "\n" , Green));
	return 0;
}

void RequestParser::ParseRequest(HeaderData &headerData){
	fillHeaderData(headerData);
	printHeaderdata(headerData);
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();
	getCurrentServer(confServers, headerData);
	std::vector<Location> confLocation = headerData.currentServer.getLocations();
	getCurrentLocationIndex(confLocation, headerData);
	ParseUrl(headerData);
}
