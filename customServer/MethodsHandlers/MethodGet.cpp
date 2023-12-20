#include "MethodGet.hpp"

MethodGet::MethodGet(HeaderData	*Data): headerData(Data){}

MethodGet::~MethodGet(){}

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

bool MethodGet::checkErrorPage(int const &error, std::map<int, std::string> &error_pages, int respType){

	std::map<int, std::string>::iterator it = error_pages.begin();
	for (; it != error_pages.end(); ++it)
	{
		if (it->first == error)
		{
			if (directoryStatus(it->second) == 2){
				fd = open(it->second.c_str(), O_RDONLY, 0664);
				headerData->response.StatusCode = error;
				headerData->response.ResponseType = respType;
				headerData->response.fileFd = fd;
				return true;
			}
		}
	}
	headerData->response.StatusCode = error;
	headerData->response.ResponseType = 3;
	return false;
}

bool MethodGet::checkInHttp(int const &error, int respType){
	std::map<int, std::string> error_pages = headerData->currentLocation.getError_pages();
	
	if (!checkErrorPage(error, error_pages, respType)){
		error_pages = headerData->currentServer.getError_pages();
		if (!checkErrorPage(error, error_pages, respType)){
			error_pages = Configurations::http.getError_pages();
			if (!checkErrorPage(error, error_pages, respType))
				return false;
		}
	}
	return true;
}

bool MethodGet::handleDirectoryPath() {
	std::vector<std::string> indexes;
	if (headerData->currentLocation.getAutoindex() == "off")
		return checkInHttp(403,1);
	if (headerData->locationIndex == -1) { // only server
		indexes = headerData->currentServer.getIndex();
		std::cout << "hi i'm here \n";
		if (indexes.size() > 0){
			for (int i = 0;  i < indexes.size() ; ++i){
				std::string newPath = headerData->Path + indexes[i];
				if (directoryStatus(newPath) == 2){
					fd = open(newPath.c_str(), O_RDONLY, 0664);
					return headerData->response.fileFd = fd;
				}
			};
			return checkInHttp(404,1);
		}
	}
	indexes = headerData->currentLocation.getIndex();
	for (int i = 0;  i < indexes.size() ; ++i){
		std::cout << "index [" << i << "] = " << indexes[i] << std::endl;
		std::string newPath = headerData->Path + indexes[i];
		if (directoryStatus(newPath) == 2){
			std::cout << "hee \n";
			fd = open(newPath.c_str(), O_RDONLY, 0664);
			return headerData->response.fileFd = fd;
		}
		return checkInHttp(404,1);
	}
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType =  2;
	headerData->response.Location = headerData->Path;
	return false;
}


bool MethodGet::GetMethodHandler(){
	if (directoryStatus(headerData->Path) == VALID_PATH){
		if(fileStatus(headerData->Path) == FORBIDDEN_READ)
			return checkInHttp(403, 1);
		fd = open(headerData->Path.c_str(), O_RDONLY, 0664);
		headerData->response.fileFd = fd;
		headerData->response.StatusCode = 200;
		headerData->response.ResponseType =  1;
		return true;
	}
	if (directoryStatus(headerData->Path) == DIRE && headerData->Path[headerData->Path.length() - 1] == '/'){
		handleDirectoryPath();
		return true;
	}
	headerData->response.StatusCode = 301;
	headerData->response.ResponseType =  0;
	headerData->response.Location = headerData->url + "/" ;
	return true;	
}