#include "RequestParser.hpp"

RequestParser::RequestParser(DataPool *headerData){
	this->headerData = headerData;
}

RequestParser::RequestParser(){}

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

void  	RequestParser::printHeaderdata(){
	std::map<std::string, std::string>::iterator it;
	it = headerData->Headers.begin();
	DEBUGOUT(1, COLORED("Path [" << headerData->Path << "]\n", Yellow));
	DEBUGOUT(1, COLORED("Method [" << headerData->Method << "]\n", Yellow));
	for (; it != headerData->Headers.end(); ++it)
		DEBUGOUT(1, COLORED(it->first << "----> " << it->second << " . \n\n", Yellow));
}

void RequestParser::getContentType() 
{
    int pos = headerData->Path.find(".");
    if (pos != std::string::npos) {
        std::string extention = headerData->Path.substr(pos + 1, headerData->Path.length());
        std::map<std::string, std::string> Types = Configurations::http.getIncludes();
        
        
        if (Types.find(extention) != Types.end())
            headerData->response.contentType = Types[extention];
        else
            headerData->response.contentType = Configurations::http.getDefault_type();
    }
}

void RequestParser::fillHeaderData()
{

	std::istringstream BufferStream(headerData->Buffer);
	std::string requestLine;
	std::getline(BufferStream, requestLine);

	std::istringstream requestLineStream(requestLine);
	requestLineStream >> headerData->Method >>headerData->Path;
	if (int pos =headerData->Path.find(" ") != std::string::npos)
		headerData->Path = headerData->Path.substr(pos);
	urlDecoding(headerData->Path);
	headerData->url = headerData->Path ;
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
			headerData->Headers[key] = value;
		}
	}
}

void RequestParser::getCurrentServer(std::vector<ConfServer> &confServers)
{
	size_t pos = headerData->Headers["Host"].find(":");
	std::string server_name = headerData->Headers["Host"].substr(0, pos);
	std::string Port = headerData->Headers["Host"].substr(pos + 1);
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < confServers.size(); i++)
	{	serverNames = confServers[i].getConfServer_names();
		int pos = std::find(serverNames.begin(), serverNames.end(),server_name) != serverNames.end();
		if (pos || confServers[i].getHost() == server_name)
		{
			if (confServers[i].getListen() == Port)
			{
				headerData->currentServer = confServers[i];
				break;
			}
		}
		else if (confServers[i].getListen() == Port){
			headerData->currentServer = confServers[i];
			break;

		}
	}
}

bool RequestParser::redirectionType(std::vector<Location> &confLocation){
	for (size_t i = 0;i < confLocation.size(); i++)
	{
		if (confLocation[i].getPath() == headerData->currentLocation.getRedirection().ReturnLocation)
			return 0;
	}
	return 1;
}

void RequestParser::getCurrentLocationIndex(std::vector<Location> &confLocation)
{
	size_t i = 0;
	int start = 0 ,end = 0;
	if ((headerData->Path ) == "/"){
		headerData->url =  headerData->url + "/";
		start = headerData->Path.find(confLocation[i].getPath());
		if (start != std::string::npos)
			headerData->locationIndex = headerData->currentServer.getDefaultLocation();
	}
	else{
		for (;i < confLocation.size(); i++)
		{
			std::string locPath = confLocation[i].getPath();
			size_t lastCharPos = locPath[locPath.length() + 1];
			start =headerData->Path.find(confLocation[i].getPath());
			if (start != std::string::npos){
				headerData->locationIndex = i;
				break;
			}
		}
	}
	if (headerData->locationIndex != -1){
		headerData->currentLocation= confLocation[headerData->locationIndex];
		if (!headerData->currentLocation.getRedirection().ReturnLocation.empty()){
			headerData->Path = headerData->currentLocation.getRedirection().ReturnLocation;
			if (!redirectionType(confLocation) && headerData->Method == "GET"){
				headerData->url = headerData->currentLocation.getRedirection().ReturnLocation; 
				getCurrentLocationIndex(confLocation);
			}
			else{
				headerData->REDIRECTION_STAGE = true;
			}
		}
		if (!headerData->REDIRECTION_STAGE){
			end = confLocation[headerData->locationIndex].getPath().length();
			headerData->newRoot = headerData->Path.substr(start  + end);
		}
	}
}

int RequestParser::ParseUrl() {
	int pos1 = 0;

	if (headerData->locationIndex == -1){
		if (!headerData->currentServer.getRoot().empty()){
			size_t lastSlashPos = headerData->Path.find_last_of("/");
			if (lastSlashPos != std::string::npos) {
       			std::string newRoot = headerData->Path.substr(lastSlashPos);
				headerData->Path = headerData->currentServer.getRoot() + newRoot;
			}
			DEBUGOUT(1, COLORED("\n the current Location is  parse url: " << headerData->Path << "\n", Cyan));
		}
		return 1;
	}
	if (!headerData->currentLocation.getRoot().empty())
		headerData->Path = headerData->currentLocation.getRoot()  + headerData->newRoot;
	else if (!headerData->currentServer.getRoot().empty())
		headerData->Path = headerData->currentServer.getRoot() + headerData->newRoot ;	
	return 0;
}

bool RequestParser::checkErrorPage(int const &error, std::map<int, std::string> &error_pages, int respType){

	std::map<int, std::string>::iterator it = error_pages.begin();
	for (; it != error_pages.end(); ++it)
	{
		if (it->first == error)
		{
			if (directoryStatus(it->second) == 2){
				headerData->response.fileFd = open(it->second.c_str(), O_RDONLY, 0664);
				headerData->response.StatusCode = error;
				headerData->response.ResponseType = respType;
				return true;
			}
		}
	}
	headerData->response.StatusCode = error;
	headerData->response.ResponseType = 3;
	return false;
}

bool RequestParser::checkInHttp(int const &error, int respType){
	std::map<int, std::string> error_pages = headerData->currentLocation.getError_pages();
	if (!checkErrorPage(error, error_pages, respType)){
		error_pages = headerData->currentServer.getError_pages();
		if (!checkErrorPage(error, error_pages, respType)){
			error_pages = Configurations::http.getError_pages();
			if (!checkErrorPage(error, error_pages, respType))
				return true;
		}
	}
	return true;
}

void RequestParser::ParseRequest(){
	fillHeaderData();
	printHeaderdata();
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();
	getCurrentServer(confServers);
	std::vector<Location> confLocation = headerData->currentServer.getLocations();
	if (confLocation.size() > 0)
		getCurrentLocationIndex(confLocation);
	if (!headerData->REDIRECTION_STAGE)
		ParseUrl();
	if (directoryStatus(headerData->Path.substr(1)) >= 1 && headerData->Path[0] == '/')
		headerData->Path = headerData->Path.substr(1);
}
