#include "Request.hpp"

Request::Request(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData.locationIndex = -1;
	fd = -1;
	headerData.REDIRECTION_STAGE = false;
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


int directoryStatus(const std::string& path) {
    struct stat directoryInfo;
    if (stat(path.c_str(), &directoryInfo) == 0) {
        if (S_ISDIR(directoryInfo.st_mode))
			return 1; // is a directory
		else if (S_ISREG(directoryInfo.st_mode))
			return 2; //is path exist;
		return 0; //is not
	}
	return -1; // is not exist
}

int fileStatus(const std::string& path) {
    struct stat fileInfo;

    // Use stat to get information about the file
    if (stat(path.c_str(), &fileInfo) == 0) {
        // Check if it's a regular file
        if (S_ISREG(fileInfo.st_mode)) {
            // Check for read permission
            if (fileInfo.st_mode & S_IRUSR) {
                return 3; // Read permission is granted for a regular file
            } else {
                return 4; // No read permission for a regular file
            }
        } else {
            // Handle non-regular files (directories, symbolic links, etc.) here
            return 0; // Not a regular file
        }
    } else {
        // Handle error accessing file information
        std::cerr << "Error accessing file information for " << path << std::endl;
        return -1; // Error code (you may want to choose a more meaningful value)
    }
}

void Request::handleDirectoryPath(Response &response) {
	std::vector<std::string> indexes;
	if (headerData.currentLocation.getAutoindex() == "off"){
		checkInHttp(response, 403,1);
		return;
	}
	if (headerData.locationIndex == -1) { // only server
		indexes = headerData.currentServer.getIndex();
		std::cout << "hi i'm here \n";
		if (indexes.size() > 0){
			for (int i = 0;  i < indexes.size() ; ++i){
				std::string newPath = headerData.Path + indexes[i];
				if (directoryStatus(newPath) == 2){
					fd = open(newPath.c_str(), O_RDONLY, 0664);
					response.setFD(fd);
					return;
				}
			};
			checkInHttp(response, 404,1);
			return;
		}
	}
	indexes = headerData.currentLocation.getIndex();
	for (int i = 0;  i < indexes.size() ; ++i){
		std::cout << "index [" << i << "] = " << indexes[i] << std::endl;
		std::string newPath = headerData.Path + indexes[i];
		if (directoryStatus(newPath) == 2){
			std::cout << "hee \n";
			fd = open(newPath.c_str(), O_RDONLY, 0664);
			response.setFD(fd);
			return;
		}
		checkInHttp(response, 404,1);
		return;
	}
	response.setStatusCode(200, 2);
	response.RedirectionPath = headerData.Path;
}


bool Request::parseHeaderErrors(Response &response){
	if (!GetHeadersValue(headerData.Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData.Headers, "Transfer-Encoding") != "chunked")
		return checkInHttp(response,501, 1);
	if (headerData.Method == "POST" && GetHeadersValue(headerData.Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData.Headers, "Content-Length").empty())
		return checkInHttp(response,400, 1);
	if (!IsValidSetOfCharacter(headerData.Path))
		return checkInHttp(response,400, 1);
	if (headerData.Path.length() > 2048)
		return checkInHttp(response,414, 1);
	// if (response.getStatusCode() != 200) //-> add 4014 max body size
	// 	ThrowErrorCode (response.getStatusCode());
	return false;
}


bool Request::processRedirectionAndAllowance(Response &response){

	int pos1 = 0; 
	std::string newPath;

	if (headerData.locationIndex != -1){
		std::vector<std::string> allowMethod = headerData.currentLocation.getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), headerData.Method) != allowMethod.end();
		if(!allow)
			return checkInHttp(response,405, 1);
		if (headerData.REDIRECTION_STAGE){
			response.RedirectionPath = headerData.Path; 
			response.setStatusCode(headerData.currentLocation.getRedirection().statusCode , 0);
			return true;
		}
	}
	return false;
}


bool Request::checkErrorPage(Response &response, int const &error, std::map<int, std::string> &error_pages, int respType){

	std::map<int, std::string>::iterator it = error_pages.begin();
	for (; it != error_pages.end(); ++it)
	{
		if (it->first == error)
		{
			if (directoryStatus(it->second) == 2){
				fd = open(it->second.c_str(), O_RDONLY, 0664);
				response.setStatusCode(error, respType);
				response.setFD(fd);
				return true;
			}
		}
	}
	response.setStatusCode(error,3);
	return false;
}


bool Request::checkInHttp( Response &response, int const &error, int respType){
	std::map<int, std::string> error_pages = headerData.currentLocation.getError_pages();
	
	if (!checkErrorPage(response, error, error_pages, respType)){
		error_pages = headerData.currentServer.getError_pages();
		if (!checkErrorPage(response, error, error_pages, respType)){
			error_pages = Configurations::http.getError_pages();
			if (!checkErrorPage(response, error, error_pages, respType))
				return false;
		}
	}
	return true;
}


bool Request::GetMethod(Response &response){
	if (directoryStatus(headerData.Path) == VALID_PATH){
		if(fileStatus(headerData.Path) == FORBIDDEN_READ)
			return checkInHttp(response, 403, 1);
		fd = open(headerData.Path.c_str(), O_RDONLY, 0664);
		response.setFD(fd);
		response.setStatusCode(200, 1);
		return true;
	}
	if (directoryStatus(headerData.Path) == DIRE && headerData.Path[headerData.Path.length() - 1] == '/'){
		handleDirectoryPath(response);
		return true;
	}
	response.setStatusCode(301, 0);
	response.RedirectionPath = headerData.url + "/" ;
	return true;	
}

bool Request::methodParser(Response &response){
	DEBUGOUT(1, COLORED(" check for Method : [" << headerData.Method << "]" <<  ", Path is : " << headerData.Path  << "\n", Red));
	if (headerData.Method == "GET")
		return GetMethod(response);
	if (headerData.Method == "POST")
		return GetMethod(response); //? same as get here need implement 
	if (headerData.Method == "DELETE");
	return false;
}

void Request::processRequest(Response &response)
{
	response.setStatusCode(200, 1);
	if(parseHeaderErrors(response))
		return;
	if (processRedirectionAndAllowance(response))
		return;
	if (!headerData.REDIRECTION_STAGE){
		if (directoryStatus(headerData.Path) >  0){
			std::cout << "hi is a dir :\n";
			if (methodParser(response))
				return;
		}
		checkInHttp(response, 404, 1);
	}
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
			DEBUGOUT(1, COLORED("\n the current Server is  : " << headerData.currentServer.getListen() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the current Location is  : " << headerData.currentLocation.getPath() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the Path that working on it is in pros-->   : " << headerData.Path << "\n", Red));
		}
	case REQUEST_HANDLER_STAGE:
		processRequest(response);
		break;
	default:
		break;
	}
	return (false);
}
