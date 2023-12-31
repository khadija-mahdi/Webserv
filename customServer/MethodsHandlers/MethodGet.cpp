#include "MethodGet.hpp"

MethodGet::MethodGet(DataPool	*Data): headerData(Data){}

MethodGet::~MethodGet(){}

int directoryStatus(const std::string& path) {
    struct stat directoryInfo;
    if (stat(path.c_str(), &directoryInfo) == 0) {
        if (S_ISDIR(directoryInfo.st_mode)){
			return 1; // is a directory
		}
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

bool MethodGet::handleDirectoryPath() {
	std::vector<std::string> indexes;
	if (headerData->currentLocation.getAutoindex() == "off")
		return requestParser.checkInHttp(403,1);
	if (headerData->locationIndex == -1) { // only server
		indexes = headerData->currentServer.getIndex();
		if (indexes.size() > 0){
			for (int i = 0;  i < indexes.size() ; ++i){
				std::string newPath = headerData->Path + indexes[i];
				if (directoryStatus(newPath) == 2){
					headerData->response.fileFd = open(newPath.c_str(), O_RDONLY, 0664);
					return  true;
				}
			};
			return requestParser.checkInHttp(404,1);
		}
	}
	indexes = headerData->currentLocation.getIndex();
	for (int i = 0;  i < indexes.size() ; ++i){
		std::string newPath = headerData->Path + indexes[i];
		if (directoryStatus(newPath) == 2){
			headerData->response.ResponseType =  1;
			DEBUGOUT(1 , COLORED("path is  : " << newPath , Red));
			headerData->response.fileFd = open(newPath.c_str(), O_RDONLY, 0664);
			return true;
		}
	}
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType =  2;
	headerData->response.Location = headerData->Path;
	return true;
}



bool MethodGet::GetFileHandler(){
	if(fileStatus(headerData->Path) == FORBIDDEN_READ)
		return requestParser.checkInHttp(403, 1);
	headerData->response.fileFd = open(headerData->Path.c_str(), O_RDONLY, 0664);
	headerData->response.ResponseType =  1;
	return true;
}

bool MethodGet::GetDirectoryHandler(){
	if (directoryStatus(headerData->Path) == DIRE && headerData->Path[headerData->Path.length() - 1] == '/')
		return handleDirectoryPath();
	DEBUGOUT(1 , COLORED("headerData->Path[headerData->Path.length() - 1] : " << headerData->Path[headerData->Path.length() - 1], Blue));
	if (headerData->Path[headerData->Path.length() - 1] != '/'){
		headerData->response.StatusCode = 301;
		headerData->response.ResponseType =  0;
		headerData->response.Location = headerData->url + "/" ;
		return true;	
	}
	return requestParser.checkInHttp(404, 1);
}

bool MethodGet::GetMethodHandler(){
	DEBUGOUT(1 , COLORED("GET Method Handler : " , Blue));
	DEBUGOUT(1 , COLORED("path  : " << headerData->Path << " , IS DIR : " << directoryStatus(headerData->Path) , Blue));
	if (directoryStatus(headerData->Path) == VALID_PATH)
		return GetFileHandler();
	return GetDirectoryHandler();
}

bool MethodGet::HandleRequest(std::string &data)
{
	REQUEST_STATE = REQUEST_HANDLER_STAGE;
	return GetMethodHandler();
	
}