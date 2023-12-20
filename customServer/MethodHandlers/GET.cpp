// #include "GET.hpp"
// #include "../Request.hpp"

// void GET::handleDirectoryPath(Response &response) {
// 	std::vector<std::string> indexes;
// 	if (headerData.currentLocation.getAutoindex() == "off"){
// 		checkInHttp(response, 403,1);
// 		return;
// 	}
// 	if (headerData.locationIndex == -1) { // only server
// 		indexes = headerData.currentServer.getIndex();
// 		std::cout << "hi i'm here \n";
// 		if (indexes.size() > 0){
// 			for (int i = 0;  i < indexes.size() ; ++i){
// 				std::string newPath = headerData.Path + indexes[i];
// 				if (directoryStatus(newPath) == 2){
// 					fd = open(newPath.c_str(), O_RDONLY, 0664);
// 					response.setFD(fd);
// 					return;
// 				}
// 			};
// 			checkInHttp(response, 404,1);
// 			return;
// 		}
// 	}
// 	indexes = headerData.currentLocation.getIndex();
// 	for (int i = 0;  i < indexes.size() ; ++i){
// 		std::cout << "index [" << i << "] = " << indexes[i] << std::endl;
// 		std::string newPath = headerData.Path + indexes[i];
// 		if (directoryStatus(newPath) == 2){
// 			std::cout << "hee \n";
// 			fd = open(newPath.c_str(), O_RDONLY, 0664);
// 			response.setFD(fd);
// 			return;
// 		}
// 		checkInHttp(response, 404,1);
// 		return;
// 	}
// 	response.setStatusCode(200, 2);
// 	response.RedirectionPath = headerData.Path;
// }

// bool GET::GetMethod(Response &response){
// 	if (directoryStatus(headerData.Path) == VALID_PATH){
// 		if(fileStatus(headerData.Path) == FORBIDDEN_READ)
// 			return checkInHttp(response, 403, 1);
// 		fd = open(headerData.Path.c_str(), O_RDONLY, 0664);
// 		response.setFD(fd);
// 		response.setStatusCode(200, 1);
// 		return true;
// 	}
// 	if (directoryStatus(headerData.Path) == DIRE && headerData.Path[headerData.Path.length() - 1] == '/'){
// 		handleDirectoryPath(response);
// 		return true;
// 	}
// 	response.setStatusCode(301, 0);
// 	response.RedirectionPath = headerData.url + "/" ;
// 	return true;	
// }