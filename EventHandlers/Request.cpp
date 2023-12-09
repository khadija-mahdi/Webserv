#include "Request.hpp"

Request::Request(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	locationIndex = -1;
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


void Request::parseHeaderErrors(Response &response){
	if (!GetHeadersValue(Headers, "Transfer-Encoding").empty() && GetHeadersValue(Headers, "Transfer-Encoding") != "chunked")
		response.setStatusCode(501, 1);
	else if (Method == "Post" && GetHeadersValue(Headers, "Transfer-Encoding").empty() && GetHeadersValue(Headers, "Content-Length").empty())
		response.setStatusCode(400, 1);
	else if (!IsValidSetOfCharacter(Path))
		response.setStatusCode(400, 1);
	else if (Path.length() > 2048)
		response.setStatusCode(414, 1);
	// if (response.getStatusCode() != 200) //-> add 4014 max body size
	// 	ThrowErrorCode (response.getStatusCode());
	else
	{
		fd = open(Path.c_str(), O_RDONLY, 0664);
		if (fd == -1)
			response.setStatusCode(404, 1);
		else
			response.setFD(fd);
	}
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
				std::cout << "is found in server " << i << std::endl;
				break;
			}
		}
		else if (confServers[i].getListen() == Port)
		{
			serverIndex = i;
			std::cout << "is found in server with just the port" << i << std::endl;
		}
	}
}
int getCurrentLocationIndex(std::vector<Location> &confLocation, std::string &Path, int &pos){
	for (size_t i = 0; i < confLocation.size(); i++)
	{
		Path = "/" + Path;
		pos = Path.find(confLocation[i].getPath());
		if (pos != std::string::npos){
			std::cout << "Path is matched in location " << i << confLocation[i].getPath() << std::endl;
			return i;
		}
	}
	return -1;
}

int Request::pareRedirection(Location &confLocation, Response &response){ // need more details so more :)
	if (!confLocation.getRedirection().ReturnLocation.empty()){
		response.setStatusCode(confLocation.getRedirection().statusCode , 0);
		response.RedirectionPath = confLocation.getRedirection().ReturnLocation; 
		// Path = confLocation.getRedirection().ReturnLocation;
		return true;
	}
	std::vector<std::string> allowMethod = confLocation.getAllow();
	int allow = std::find(allowMethod.begin(), allowMethod.end(), Method) != allowMethod.end();
	if(!allow)
	{
		response.setStatusCode(405, 1);
		return true;
	}
	return false;
}

void Request::getCurrentLocation(std::vector<ConfServer> &confServers, Response &response){
	std::vector<Location> confLocation = confServers[serverIndex].getLocations();
	std::string newPath;
	int pos1;
	Path = "/" + Path;
	locationIndex =  getCurrentLocationIndex(confLocation, Path, pos1);
	if (confLocation.size() > 0)
		pareRedirection(confLocation[locationIndex], response);
	pareRedirection(confLocation[locationIndex], response);
	if (!confLocation[locationIndex].getRoot().empty())
		Path = confLocation[locationIndex].getRoot() + Path.substr(pos1);
	else if (!confServers[serverIndex].getRoot().empty())
		Path = confServers[serverIndex].getRoot() + Path.substr(pos1);
	else
	{
		response.setStatusCode(40, 1);
		return;
	}
}



void Request::ParseUrl(Response &response)
{
	std::vector<ConfServer> confServers = Configurations::http.getConfServes();

	getCurrentServer(confServers);
	getCurrentLocation(confServers, response);
}

void Request::ParseHeaders(Response &response)
{
	HeaderIterator it;
	response.setStatusCode(200, 1);// default
	DEBUGOUT(1, "Request ParseHeaders been called again.");

	fillHeaderData();
	parseHeaderErrors(response);
	it = Headers.begin();
	DEBUGOUT(1, COLORED("Path [" << this->Path << "]\n", Yellow));
	DEBUGOUT(1, COLORED("Method [" << this->Method << "]\n", Yellow));
	for (; it != Headers.end(); ++it)
		DEBUGOUT(1, COLORED(it->first << " = " << it->second << " . ", Yellow));
	ParseUrl(response);
	REQUEST_STATE = REQUEST_HANDLER_STAGE;
}

void Request::methodParser(Response &response){
	if (Method == "Get"){

	}
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
		methodParser(response);
		break;
	default:
		break;
	}
	return (false);
}
