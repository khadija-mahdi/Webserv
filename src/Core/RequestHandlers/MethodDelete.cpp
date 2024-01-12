#include "Include/MethodDelete.hpp"
#define vebros 0

MethodDelete::MethodDelete(DataPool &Data) : Request(Data) {}

MethodDelete::~MethodDelete() {}

bool hasWritePermission(const std::string &path)
{
	struct stat directoryInfo;
	if (stat(path.c_str(), &directoryInfo) == 0)
	{
		if (S_ISDIR(directoryInfo.st_mode))
		{
			if (directoryInfo.st_mode & S_IRUSR)
				return true;
			return false;
		}
	}
	return true;
}
bool deleteFolderContents(const std::string &folderPath)
{
	DIR *dir = opendir(folderPath.c_str());
	if (dir != NULL)
	{
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				std::string filePath = folderPath + "/" + entry->d_name;

				if (access(filePath.c_str(), W_OK) < 0)
				{
					closedir(dir);
					return false;
				}

				int status = directoryStatus(filePath);
				if (status == 1)
				{
					if (!deleteFolderContents(filePath))
					{
						closedir(dir);
						return false;
					}
				}
				if (unlink(filePath.c_str()) != 0 && rmdir(filePath.c_str()) != 0)
				{
					closedir(dir);
					return false;
				}
			}
		}
		closedir(dir);
	}
	else
	{
		return false;
	}
	return true;
}

bool MethodDelete::DeleteDirectoryHandler()
{
	DEBUGMSGT(vebros, COLORED("Delete Method the Path Is a Directory : " << dataPool.Path, Green));
	if (dataPool.Path[dataPool.Path.length() - 1] != '/')
		throw HTTPError(409);
	if (!dataPool.currentLocation.getCgiAccept().empty())
	{
		std::vector<std::string> indexes;
		indexes = dataPool.currentServer.getIndex();
		if (dataPool.currentLocation.getIndex().size() > 0)
			indexes = dataPool.currentLocation.getIndex();
		std::string Path = getCorrectIndex(indexes, dataPool.Path);
		if (!Path.empty())
		{
			std::string extention = GetFileExtention(Path);
			if (dataPool.currentLocation.hasCgi(extention))
				return (Request::Execute(Path, "DELETE"), false);
		}
		throw HTTPError(403);
	}
	else
	{
		if (deleteFolderContents(dataPool.Path))
			throw HTTPError(204);
		else
		{
			if (access(dataPool.Path.c_str(), W_OK) >= 0)
				throw HTTPError(500);
			throw HTTPError(403);
		}
	}
	return false;
}

bool MethodDelete::HandleRequest(std::string &data)
{
	DEBUGMSGT(0, data);
	DEBUGMSGT(vebros, COLORED("\n 	Delete Method Handler : \n", Blue));
	if (directoryStatus(dataPool.Path) == DIRE)
		return DeleteDirectoryHandler();
	if (directoryStatus(dataPool.Path) == VALID_PATH)
	{
		std::string extention = GetFileExtention(dataPool.Path);
		if (dataPool.currentLocation.hasCgi(extention))
			return (Request::Execute(dataPool.Path, "DELETE"), false);
		else
		{
			if (access(dataPool.Path.c_str(), W_OK) >= 0 &&
				unlink(dataPool.Path.c_str()) == 0)
				throw HTTPError(204);
			throw HTTPError(403);
		}
	}
	throw HTTPError(404);
}