#include "MethodGet.hpp"

MethodGet::MethodGet(HeaderData	*Data): headerData(Data){}

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
					fd = open(newPath.c_str(), O_RDONLY, 0664);
					return headerData->response.fileFd = fd;
				}
			};
			return requestParser.checkInHttp(404,1);
		}
	}
	indexes = headerData->currentLocation.getIndex();
	for (int i = 0;  i < indexes.size() ; ++i){
		std::string newPath = headerData->Path + indexes[i];
		if (directoryStatus(newPath) == 2){
			fd = open(newPath.c_str(), O_RDONLY, 0664);
			return headerData->response.fileFd = fd;
		}
	}
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType =  2;
	headerData->response.Location = headerData->Path;
	return true;
}


char **FromVectorToArray(std::vector<std::string> vec)
{
    char **Array = new char *[vec.size() + 1];
    for (size_t i = 0; i < (vec.size()); i++)
    {
        Array[i] = new char[vec.at(i).length() + 1];
        std::strcpy(Array[i], vec.at(i).c_str());
    }
    Array[vec.size()] = NULL;
    return (Array);
}
std::string GetFileName(std::string FilePath)
{
    size_t index;

    if (FilePath.empty())
        return (FilePath);
    if ((index = FilePath.find_last_of("/")) == std::string::npos)
        return (FilePath);
    return (FilePath.substr(index + 1));
}
void MethodGet::FillCgiData()
{
    arguments.push_back("public/file.php");
    arguments.push_back("/bin/ls");

    env.push_back("REDIRECT_STATUS=1");
    env.push_back("REQUEST_METHOD=" + headerData->Method);
    env.push_back("SCRIPT_FILENAME=public/file.php");
    env.push_back("HTTP_COOKIE=" + headerData->Headers["Cookie"]);
    env.push_back("QUERY_STRING=" + NULL);

    if (headerData->Method == "POST")
    {
        env.push_back("CONTENT_LENGTH=" + headerData->Headers["Content-Length"]);
        env.push_back("CONTENT_TYPE=" + headerData->Headers["Content-Type"]);
    }
}

bool MethodGet::CGIHandler(){

	// FillCgiData();

	// pid_t pid = fork();

	// if (pid == -1) {
	// 	perror("fork");
	// 	return false;
	// } else if (pid == 0) {
	// 	int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

	// 	if (fd == -1) {
	// 		perror("open");
	// 		_exit(EXIT_FAILURE);
	// 	}

	// 	if (dup2(fd, STDOUT_FILENO) == -1) {
	// 		perror("dup2");
	// 		close(fd);
	// 		_exit(EXIT_FAILURE);
	// 	}
	// 	close(fd);

	// 	// Specify the correct path to the CGI script
	// 	execve("public/file.php", FromVectorToArray(arguments), FromVectorToArray(env));
	// 	perror("execve");
	// 	_exit(EXIT_FAILURE);
	// } else {
	// 	int status;
	// 	waitpid(pid, &status, 0);
	// 	int fd = open(headerData->Path.c_str(), O_RDONLY, 0664);
	// 	headerData->response.fileFd = fd;
	// 	headerData->response.ResponseType = 1;

	// 	return true;
	// }

	return true;
}

bool MethodGet::GetFileHandler(){
	size_t pos = headerData->Path.find(".");
	if (pos != std::string::npos){
		std::string extention = headerData->Path.substr(pos);
		std::map<std::string, std::string> cgi = headerData->currentLocation.getCgiAccept();
		if(cgi.size() > 0){
			std::map<std::string, std::string>::iterator it = cgi.begin();
			for(it; it != cgi.end(); ++it){
				if (extention == it->first)
				{
					std::cout << "is found in cgi " << it->first << std::endl;
					headerData->CGI__STAGE = true;
					headerData->cgiData.cgiPath = it->second;
					break;
				}
			}
		}
	}
	// if (headerData->CGI__STAGE)
	// 	return CGIHandler();
	if(fileStatus(headerData->Path) == FORBIDDEN_READ)
		return requestParser.checkInHttp(403, 1);
	fd = open(headerData->Path.c_str(), O_RDONLY, 0664);
	headerData->response.fileFd = fd;
	headerData->response.ResponseType =  1;
	return true;
}

bool MethodGet::GetDirectoryHandler(){
	if (directoryStatus(headerData->Path) == DIRE && headerData->Path[headerData->Path.length() - 1] == '/'){
		DEBUGOUT(1 , COLORED("listen Dir : " , Blue));
		handleDirectoryPath();
		return true;
	}
	headerData->response.StatusCode = 301;
	headerData->response.ResponseType =  0;
	headerData->response.Location = headerData->url + "/" ;
	return true;	
}

bool MethodGet::GetMethodHandler(){
	DEBUGOUT(1 , COLORED("GET Method Handler : " , Blue));
	DEBUGOUT(1 , COLORED("path  : " << headerData->Path << " , IS DIR : " << directoryStatus(headerData->Path) , Blue));
	if (directoryStatus(headerData->Path) == VALID_PATH)
		return GetFileHandler();
	return GetDirectoryHandler();
}

bool MethodGet::processRequest()
{
	std::cout << "hi from Get\n";
	REQUEST_STATE = REQUEST_HANDLER_STAGE;
	return GetMethodHandler();
	
}