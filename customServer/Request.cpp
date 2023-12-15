#include "Request.hpp"

Request::Request(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData.locationIndex = -1;
	fd = -1;
}

Request::~Request() {}


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
	if (!GetHeadersValue(headerData.Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData.Headers, "Transfer-Encoding") != "chunked")
		response.setStatusCode(501, 1);
	else if (headerData.Method == "POST" && GetHeadersValue(headerData.Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData.Headers, "Content-Length").empty())
		response.setStatusCode(400, 1);
	else if (!IsValidSetOfCharacter(headerData.Path))
		response.setStatusCode(400, 1);
	else if (headerData.Path.length() > 2048)
		response.setStatusCode(414, 1);
	// if (response.getStatusCode() != 200) //-> add 4014 max body size
	// 	ThrowErrorCode (response.getStatusCode());
}


bool Request::processRedirectionAndAllowance(Response &response){

	int pos1 = 0; 
	std::string newPath;

	if (headerData.locationIndex != -1){
		std::vector<std::string> allowMethod = headerData.currentLocation.getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), headerData.Method) != allowMethod.end();
		if(!allow)
		{
			response.setStatusCode(405, 1);
			return 1;
		}
		headerData.currentLocation.printErrorPages();
		if (!headerData.currentLocation.getRedirection().ReturnLocation.empty()){
			response.RedirectionPath = headerData.currentLocation.getRedirection().ReturnLocation; 
			response.setStatusCode(headerData.currentLocation.getRedirection().statusCode , 0);
			return true;
		}
	}
	return false;
}



void Request::handleDirectoryPath(Response &response) {
	std::vector<std::string> indexes;
	if (headerData.locationIndex == -1) { // only server
		if (headerData.Path[0] == '/')
			headerData.Path = headerData.Path.substr(1);
		indexes = headerData.currentServer.getIndex();
		for (int i = 0;  i < indexes.size() ; ++i){
			std::string newPath = headerData.Path + indexes[i];
			std::cout << "new path --> " << newPath << std::endl;
			if (IsDirectory(newPath) == 2){
				fd = open(newPath.c_str(), O_RDONLY, 0664);
				response.setFD(fd);
				return;
			}
			else
				response.setStatusCode(404,1);
		};// add index to the server and auto index ...
	}
	else {
		indexes = headerData.currentLocation.getIndex();
		for (int i = 0;  i < indexes.size() ; ++i){
			std::string newPath = headerData.Path + indexes[i];
			if (IsDirectory(newPath) == 2){
				fd = open(newPath.c_str(), O_RDONLY, 0664);
				response.setFD(fd);
				return;
			}
		}
		if (headerData.currentLocation.getAutoindex() == "off"){
			response.setStatusCode(403,1);
		}
		else
		{
			response.setStatusCode(200, 2);
			response.RedirectionPath = headerData.Path;
		}
	}
}

bool Request::checkErrorPage(int const &error, std::map<int, std::string> &error_pages, Response &response){

	std::map<int, std::string>::iterator it = error_pages.begin();
		std::cout << "size ---> :" << error_pages.size() << std::endl;
	for (; it != error_pages.end(); ++it)
	{
		std::cout << "first :" << it->first << std::endl;
		if (it->first == error)
		{
			std::cout << " path in error -- > " << it->second << std::endl;
			if (IsDirectory(it->second) == 2){
				fd = open(it->second.c_str(), O_RDONLY, 0664);
				std::cout << "fd is : " << fd << std::endl;
				response.setStatusCode(error,1);
				response.setFD(fd);
				return true;
			}
		}
		std::cout << "error loc : " << it->first << " => " << it->second << "\n";
	}
	response.setStatusCode(error,3);
	return false;
}

void Request::checkInHttp(int const &error, Response &response){
	std::map<int, std::string> error_pages = headerData.currentLocation.getError_pages();
	
	if (!checkErrorPage(error, error_pages, response)){
		error_pages = headerData.currentServer.getError_pages();
		if (!checkErrorPage(error, error_pages, response)){
			error_pages = Configurations::http.getError_pages();
			if (!checkErrorPage(error, error_pages, response));
		}
	}
	
}

bool Request::methodParser(Response &response){
	DEBUGOUT(1, COLORED(" check for Method : [" << headerData.Method << "]\n", Red));
	std::cout << "check path is : " << headerData.Path <<  "the last char : " << headerData.Path[headerData.Path.length() - 1] << std::endl;
	if (headerData.Method == "GET"){
		if (headerData.Path[headerData.Path.length() - 1] == '/'){
			handleDirectoryPath(response);
			return true;
		}
		else
		{
			response.setStatusCode(301, 0);
			response.RedirectionPath = headerData.url + "/";
			DEBUGOUT(1, COLORED("\n This is a directory and doesn't have a slash '/' at the end : [" << headerData.Path << "]\n", Red));
			return true;	
		}
	}
	return false;
}

void Request::processRequest(Response &response)
{
	Configurations::http.printErrorPages();
	headerData.currentServer.printErrorPages();
	headerData.currentLocation.printErrorPages();
	response.setStatusCode(200, 1);
	DEBUGOUT(1, COLORED("\n the current Server is  : " << headerData.currentServer.getListen() << "\n", Cyan));
	DEBUGOUT(1, COLORED("\n the current Location is  : " << headerData.currentLocation.getPath() << "\n", Cyan));


	if (processRedirectionAndAllowance(response)) // check for allowed method and return first;
		return;
	else if (IsDirectory(headerData.Path) <=  0)
		checkInHttp(404, response);
	else if (IsDirectory(headerData.Path) == 2){
		fd = open(headerData.Path.c_str(), O_RDONLY, 0664);
		std::cout << "fd is : " << fd << std::endl;
		response.setFD(fd);
	}
	else
	{
		if (IsDirectory(headerData.Path) == 1 && methodParser(response))
			return;
		parseHeaderErrors(response);
	}
	//-> need to handle errors pages from config file 
	// /*
		// if (response.getStatusCode() != 200){
		// 	if (confServer.get)
		// }
	// */
REQUEST_STATE = REQUEST_HANDLER_STAGE;
}



bool Request::RequestHandler(std::string Data, Response &response)
{
	headerData.Buffer += Data;
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (headerData.Buffer.find("\r\n\r\n") != std::string::npos){
			DEBUGOUT(1, headerData.Buffer);
			requestParser.ParseRequest(headerData);
		}
	case REQUEST_HANDLER_STAGE:
		processRequest(response);
		break;
	default:
		break;
	}
	return (false);
}
