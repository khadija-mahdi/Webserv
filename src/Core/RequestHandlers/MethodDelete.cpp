#include "Include/MethodDelete.hpp"

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

bool MethodDelete::DeleteDirectoryHandler()
{
	DEBUGMSGT(1, COLORED("Delete Method the Path Is a Directory : " << dataPool.Path, Green));
	if (dataPool.Path[dataPool.Path.length() - 1] != '/')
	{
		DEBUGMSGT(1, COLORED("Delete Method 409  dir without / ", Green));
		throw HTTPError(409);
	}
	else
	{
		if (deleteFolderContents(dataPool.Path))
		{
			DEBUGMSGT(1, COLORED("success remove all Directory content : ", Green));
			throw HTTPError(204);
		}
		else
		{
			if (hasWritePermission(dataPool.Path))
				throw HTTPError(500);
			throw HTTPError(403);
		}
	}
	return false;
}

bool MethodDelete::DeleteFileHandler()
{
	DEBUGMSGT(1, COLORED("Delete Method the Path Is a File : " << dataPool.Path, Green));
	if (std::remove(dataPool.Path.c_str()) == 0)
		throw HTTPError(204);
	else
		throw HTTPError(403);
	return false;
}

bool MethodDelete::DeleteMethodHandler()
{
	DEBUGMSGT(1, COLORED("Delete Method Handler : ", Blue));
	if (directoryStatus(dataPool.Path) == 1)
		return DeleteDirectoryHandler();
	if (directoryStatus(dataPool.Path) == 2)
		return DeleteFileHandler();
	else
		throw HTTPError(404);

	return false;
}

bool MethodDelete::HandleRequest(std::string &data)
{
	return DeleteMethodHandler();
}