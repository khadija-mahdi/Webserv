#include "Include/MethodGet.hpp"

MethodGet::MethodGet(DataPool &Data) : Request(Data){}

MethodGet::~MethodGet() {}

int directoryStatus(const std::string &path)
{
	struct stat directoryInfo;
	if (stat(path.c_str(), &directoryInfo) == 0)
	{
		if (S_ISDIR(directoryInfo.st_mode))
		{
			return 1; // is a directory
		}
		else if (S_ISREG(directoryInfo.st_mode))
			return 2; // is path exist;
		return 0;	  // is not
	}
	return -1; // is not exist
}

int fileStatus(const std::string &path)
{
	struct stat fileInfo;

	// Use stat to get information about the file
	if (stat(path.c_str(), &fileInfo) == 0)
	{
		// Check if it's a regular file
		if (S_ISREG(fileInfo.st_mode))
		{
			// Check for read permission
			if (fileInfo.st_mode & S_IRUSR)
			{
				return 3; // Read permission is granted for a regular file
			}
			else
			{
				return 4; // No read permission for a regular file
			}
		}
		else
		{
			// Handle non-regular files (directories, symbolic links, etc.) here
			return 0; // Not a regular file
		}
	}
	else
	{
		// Handle error accessing file information
		std::cerr << "Error accessing file information for " << path << std::endl;
		return -1; // Error code (you may want to choose a more meaningful value)
	}
}

bool MethodGet::handleDirectoryPath()
{
	std::vector<std::string> indexes;
	if (dataPool.currentLocation.getAutoindex() == "off")
		throw HTTPError(403);
	if (dataPool.locationIndex == -1)
	{ // only server
		indexes = dataPool.currentServer.getIndex();
		if (indexes.size() > 0)
		{
			for (int i = 0; i < indexes.size(); ++i)
			{
				std::string newPath = dataPool.Path + indexes[i];
				if (directoryStatus(newPath) == 2)
				{
					dataPool.response.fileFd = open(newPath.c_str(), O_RDONLY, 0664);
					return true;
				}
			};
			throw HTTPError(404);
		}
	}
	indexes = dataPool.currentLocation.getIndex();
	for (int i = 0; i < indexes.size(); ++i)
	{
		std::string newPath = dataPool.Path + indexes[i];
		if (directoryStatus(newPath) == 2)
		{
			DEBUGMSGT(1, COLORED("path is  : " << newPath, Red));
			dataPool.response.fileFd = open(newPath.c_str(), O_RDONLY, 0664);
			return true;
		}
	}
	dataPool.response.StatusCode = dataPool.currentLocation.getRedirection().statusCode;
	dataPool.response.Location = dataPool.currentLocation.getRedirection().ReturnLocation;
	throw HTTPError(dataPool.currentLocation.getRedirection().statusCode);
	return true;
}

bool MethodGet::GetFileHandler()
{
	DEBUGMSGT(1, "GET OPENED FILE : ");
	if (fileStatus(dataPool.Path) == FORBIDDEN_READ)
		throw HTTPError(403);
	dataPool.response.fileFd = open(dataPool.Path.c_str(), O_RDONLY, 0664);
	return true;
}

bool MethodGet::GetDirectoryHandler()
{
	if (directoryStatus(dataPool.Path) == DIRE && dataPool.Path[dataPool.Path.length() - 1] == '/')
		return handleDirectoryPath();
	DEBUGMSGT(1, COLORED("dataPool.Path[dataPool.Path.length() - 1] : " << dataPool.Path[dataPool.Path.length() - 1], Blue));
	if (dataPool.Path[dataPool.Path.length() - 1] != '/')
	{
		dataPool.response.StatusCode = 301;
		dataPool.response.Location = dataPool.url + "/";
		return true;
	}
	throw HTTPError(404);
}

bool MethodGet::GetMethodHandler()
{
	DEBUGMSGT(1, COLORED("GET Method Handler : ", Blue));
	DEBUGMSGT(1, COLORED("path  : " << dataPool.Path << " , IS DIR : " << directoryStatus(dataPool.Path), Blue));
	if (directoryStatus(dataPool.Path) == VALID_PATH)
		return GetFileHandler();
	return GetDirectoryHandler();
}

bool MethodGet::HandleRequest(std::string &data)
{
	return GetMethodHandler();
}