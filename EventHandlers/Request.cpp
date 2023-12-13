#include "Request.hpp"

Request::Request(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	locationIndex = -1;
	fd = -1;
}

Request::~Request() {}

std::string Request::getBuffer() const { return Buffer; }

std::string Request::getMethod() const { return Method; }

std::string Request::getPath() const { return Path; }

std::map<std::string, std::string> Request::getHeaders() const { return Headers; }

void Request::setHeaders(std::string &_value, std::string _key) { Headers[_key] = _value; }

void Request::setBuffer(std::string const &buff) { this->Buffer = buff; }

void Request::setMethod(std::string const &meth) { this->Method = meth; }

void Request::setPath(std::string const &path) { this->Path = path; }


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

bool IsValidSetOfCharacter(std::string str)
{
	std::string expectedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	return (str.find_last_of(expectedChars) != std::string::npos);
}

std::string GetHeadersValue(std::map<std::string, std::string> &headers, std::string Key)
{
	HeaderIterator it;
	it = headers.find(Key);
	if (it == headers.end())
		return ("");
	return (it->second);
}


void Request::fillHeaderData()
{

	std::istringstream BufferStream(Buffer);
	std::string requestLine;
	std::getline(BufferStream, requestLine);

	std::istringstream requestLineStream(requestLine);
	requestLineStream >> Method >> Path;
	if (int pos = Path.find("/") != std::string::npos)
		Path = Path.substr(pos);
	urlDecoding(Path);
	url = Path;

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
			Headers[key] = value;
		}
	}
}

int IsDirectory(const std::string& path) {
    struct stat directoryInfo;
    if (stat(path.c_str(), &directoryInfo) == 0) {
        // Check if it is a directory
        if (S_ISDIR(directoryInfo.st_mode))
			return 1; // is a directory
		else if (S_ISREG(directoryInfo.st_mode))
			return 2; //is path exist;
		return 0; //is not
	}
	return -1; // is not exist
}


void Request::parseHeaderErrors(Response &response){
	if (!GetHeadersValue(Headers, "Transfer-Encoding").empty() && GetHeadersValue(Headers, "Transfer-Encoding") != "chunked")
		response.setStatusCode(501, 1);
	else if (Method == "POST" && GetHeadersValue(Headers, "Transfer-Encoding").empty() && GetHeadersValue(Headers, "Content-Length").empty())
		response.setStatusCode(400, 1);
	// else if (!IsValidSetOfCharacter(Path))
	// 	response.setStatusCode(400, 1);
	else if (Path.length() > 2048)
		response.setStatusCode(414, 1);
	// if (response.getStatusCode() != 200) //-> add 4014 max body size
	// 	ThrowErrorCode (response.getStatusCode());
}

void Request::getCurrentServer(std::vector<ConfServer> &confServers)
{
	size_t pos = Headers["Host"].find(":");
	std::string server_name = Headers["Host"].substr(0, pos);
	std::string Port = Headers["Host"].substr(pos + 1);
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < confServers.size(); i++)
	{	serverNames = confServers[i].getConfServer_names();
		int pos = std::find(serverNames.begin(), serverNames.end(),server_name) != serverNames.end();
		if (pos || confServers[i].getHost() == server_name)
		{
			if (confServers[i].getListen() == Port)
			{
				serverIndex = i;
				std::cout << "is found in server  : " << i << std::endl;
				break;
			}
		}
		else if (confServers[i].getListen() == Port)
		{
			serverIndex = i;
			std::cout << "is found in server with just the port : " << i << std::endl;
		}
	}
}


int getCurrentLocationIndex(std::vector<Location> &confLocation, std::string &Path, int &pos)
{
    for (size_t i = 0; i < confLocation.size(); i++)
    {
		if (confLocation[i].getPath() == "/")
			continue;
        std::string locPath = confLocation[i].getPath();
        size_t lastCharPos = locPath[locPath.length() + 1];
        pos = Path.find(confLocation[i].getPath());
        if (pos != std::string::npos){
            return i;
        }
        std::cout << "pos path in loc " << pos << "path is : " << confLocation[i].getPath() << std::endl;
    }
    return -1;
}


int Request::getCurrentLocation(std::vector<ConfServer> &confServers, Response &response){
	std::vector<Location> confLocation = confServers[serverIndex].getLocations();
	std::string newPath;
	int pos1 = 0;
	Path = "/" + Path;
	if (Path == "/"){
		std::cout << "DefaultLocation() " << confServers[serverIndex].getDefaultLocation() << std::endl; 
		locationIndex = confServers[serverIndex].getDefaultLocation();
	}
	else
		locationIndex =  getCurrentLocationIndex(confLocation, Path, pos1);

	if (confLocation.size() > 0 && confLocation[locationIndex].getRedirection().ReturnLocation != ""){
		response.RedirectionPath = confLocation[locationIndex].getRedirection().ReturnLocation; 
		response.setStatusCode(confLocation[locationIndex].getRedirection().statusCode , 0);
		return 1;
	}
	else if (locationIndex != -1){
		std::vector<std::string> allowMethod = confLocation[locationIndex].getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), Method) != allowMethod.end();
		if(!allow)
		{
			response.setStatusCode(405, 1);
			return 1;
		}
		std::string locPath = confLocation[locationIndex].getPath();
		int pos2 = locPath.length();
		std::string root = Path.substr(pos1 + pos2);
		if (!confLocation[locationIndex].getRoot().empty())
			Path = confLocation[locationIndex].getRoot()  + root;
		else if (!confServers[serverIndex].getRoot().empty())
			Path = confServers[serverIndex].getRoot() + root ;
		DEBUGOUT(1, COLORED("THE root is : " << root << ", the path is " << Path,  Green));
	}
	return 0;
}

int Request::ParseUrl(Response &response) {
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();

	getCurrentServer(confServers);
	std::cout << "location size : " << confServers[serverIndex].getLocations().size() << std::endl;
	if (confServers[serverIndex].getLocations().size() == 0){
		if (!confServers[serverIndex].getRoot().empty())
			Path = confServers[serverIndex].getRoot();
	}
	else{
		if (getCurrentLocation(confServers, response))
			return 1;
	}
	DEBUGOUT(1, COLORED("THE Path after Location is : " << Path, Red));
	return 0;
}


void Request::handleDirectoryPath(Response &response) {
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();
	if (locationIndex == -1) { // only server
		;// add index to the server and auto index ...
	}
	else {
		Location confLocation = confServers[serverIndex].getLocations()[locationIndex];
		std::vector<std::string> indexes = confLocation.getIndex();
		for (int i = 0;  i < indexes.size() ; ++i){
			DEBUGOUT(1, COLORED("indexes : " << indexes[i] << "\n", Green));
			std::string newPath = Path + indexes[i];
			if (IsDirectory(newPath) == 2){
				fd = open(newPath.c_str(), O_RDONLY, 0664);
				response.setFD(fd);
				return;
			}
		}
		std::cout << "aouto index is : [" << confLocation.getAutoindex() << "]" << std::endl;
		if (confLocation.getAutoindex() == "off"){
			response.setStatusCode(403,1);
		}
		else
		{
			response.setStatusCode(200, 2);
			response.RedirectionPath = Path;
		}
	}
}

void Request::ParseHeaders(Response &response)
{
	response.setStatusCode(200, 1);// default

	printHeaderdata();
	if (ParseUrl(response))
		return;
	else if (IsDirectory(Path) <=  0)
		response.setStatusCode(404,1);
	else if (IsDirectory(Path) == 2){
		fd = open(Path.c_str(), O_RDONLY, 0664);
		response.setFD(fd);
		return;
	}
	else
	{
		if (IsDirectory(Path) == 1 && Method == "GET"){
			std::cout << "check if have / in the end " << Path[Path.length() - 1] << std::endl;
			if (Path[Path.length() - 1] == '/'){
				std::cout << "it's ok is a directory and have this in the end / \n";
				handleDirectoryPath(response);
				return ;

			}
			else
			{
				response.setStatusCode(301, 0);
				response.RedirectionPath = url + "/";
				std::cout << "it's not ok is a directory and isn't have this in the end / \n";
				return ;	
			}
		}
		parseHeaderErrors(response);
		std::cout << "fd is " << fd << std::endl;
	}
	REQUEST_STATE = REQUEST_HANDLER_STAGE;
}

int Request::methodParser(Response &response){
DEBUGOUT(1, COLORED(" check for Method : [" << this->Method << "]\n", Red));
	// if (Method == "GET"){
	// 	if (IsDirectory(Path) == 1){
	// 		std::cout << "check if have / in the end " << Path[Path.length() - 1] << std::endl;
	// 		if (Path[Path.length() - 1] == '/'){
	// 			std::cout << "it's ok is a directory and have this in the end / \n";
	// 			fd == -2;
	// 			return 0;
	// 		}
	// 		else{
	// 			response.setStatusCode(301, 0);
	// 			response.RedirectionPath = url + "/";
	// 			std::cout << "it's not ok is a directory and isn't have this in the end / \n";
	// 			return 1;			
	// 		}
	// 	}
	// }
	return 0;
}

bool Request::RequestParser(std::string Data, Response &response)
{
	this->Buffer += Data;
	DEBUGOUT(1, COLORED(this->Buffer, Yellow));
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (this->Buffer.find("\r\n\r\n") != std::string::npos)
		{
			DEBUGOUT(1, this->Buffer);
			ParseHeaders(response);
		}
		// fall through
	case REQUEST_HANDLER_STAGE:
		// methodParser(response);
		break;
	default:
		break;
	}
	return (false);
}
