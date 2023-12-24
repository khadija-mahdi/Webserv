#include "MethodDelete.hpp"

MethodDelete::MethodDelete(HeaderData *Data): headerData(Data){}

MethodDelete::MethodDelete(/* args */){}

MethodDelete::~MethodDelete(){}


// bool MethodDelete::checkErrorPage(int const &error, std::map<int, std::string> &error_pages, int respType){

// 	std::map<int, std::string>::iterator it = error_pages.begin();
// 	for (; it != error_pages.end(); ++it)
// 	{
// 		if (it->first == error)
// 		{
// 			if (directoryStatus(it->second) == 2){
// 				fd = open(it->second.c_str(), O_RDONLY, 0664);
// 				headerData->response.StatusCode = error;
// 				headerData->response.ResponseType = respType;
// 				headerData->response.fileFd = fd;
// 				return true;
// 			}
// 		}
// 	}
// 	headerData->response.StatusCode = error;
// 	headerData->response.ResponseType = 3;
// 	return false;
// }

// bool MethodDelete::requestParser.checkInHttp(int const &error, int respType){
// 	std::map<int, std::string> error_pages = headerData->currentLocation.getError_pages();
	
// 	if (!checkErrorPage(error, error_pages, respType)){
// 		error_pages = headerData->currentServer.getError_pages();
// 		if (!checkErrorPage(error, error_pages, respType)){
// 			error_pages = Configurations::http.getError_pages();
// 			if (!checkErrorPage(error, error_pages, respType))
// 				return true;
// 		}
// 	}
// 	return false;
// }


bool hasWritePermission(const std::string& path) {
    struct stat directoryInfo;
    if (stat(path.c_str(), &directoryInfo) == 0) {
        if (S_ISDIR(directoryInfo.st_mode)){
			if (directoryInfo.st_mode & S_IRUSR)
				return true;
			return false;
		}
    } 
	return true;
}


bool deleteFolderContents(const std::string& folderPath) {
    DIR* dir = opendir(folderPath.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                std::string filePath = folderPath + "/" + entry->d_name;

                int status = directoryStatus(filePath);
                if (status == 1) {
                    if (!deleteFolderContents(filePath)) {
                        closedir(dir);
                        return false;
                    }
                }
                if (unlink(filePath.c_str()) != 0 && rmdir(filePath.c_str()) != 0) {
                    std::cerr << "Error deleting: " << filePath << std::endl;
                    closedir(dir);
                    return false;
                }
            }
        }
        closedir(dir);
    } 
	else 
	{
        std::cerr << "Error opening directory: " << folderPath << std::endl;
        return false;
    }
    return true;
}

bool MethodDelete::DeleteDirectoryHandler(){
	DEBUGOUT(1 , COLORED("Delete Method the Path Is a Directory : "  << headerData->Path , Green));
	if (headerData->Path[headerData->Path.length() - 1 ] != '/'){
		DEBUGOUT(1 , COLORED("Delete Method 409  dir without / ", Green));
		return requestParser.checkInHttp(409, 1);
	}
	else
	{
		if (deleteFolderContents(headerData->Path)){
			DEBUGOUT(1 , COLORED("success remove all Directory content : ", Green));
        	return requestParser.checkInHttp(204, 1);
		}
		else{
			if (hasWritePermission(headerData->Path))
				return requestParser.checkInHttp(500, 1);	
        	return requestParser.checkInHttp(403, 1);
		}

	}
	return false;
}

bool MethodDelete::DeleteFileHandler(){
	DEBUGOUT(1 , COLORED("Delete Method the Path Is a File : "  << headerData->Path , Green));
	if (std::remove(headerData->Path.c_str()) == 0)
        return requestParser.checkInHttp(204, 1);
    else 
        return requestParser.checkInHttp(403, 1);
	return false;

}

bool MethodDelete::DeleteMethodHandler(){
	DEBUGOUT(1 , COLORED("Delete Method Handler : " , Blue));
	if (directoryStatus(headerData->Path) == 1)
		return DeleteDirectoryHandler();
	if (directoryStatus(headerData->Path) == 2)
		return DeleteFileHandler();
	else
		return requestParser.checkInHttp(404, 1);

	return false;
}

bool MethodDelete::processRequest()
{
	std::cout << "hi from delete method\n";
	return DeleteMethodHandler();
}