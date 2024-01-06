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
					closedir(dir);
					return false;
				}
			}
		}
		closedir(dir);
	}
	else
		return false;
	return true;
}

bool MethodDelete::DeleteDirectoryHandler()
{
	DEBUGMSGT(1, COLORED("Delete Method the Path Is a Directory : " << dataPool.Path, Green));
	if (dataPool.Path[dataPool.Path.length() - 1] != '/'){
		DEBUGMSGT(1, COLORED("\n The directory path should end with a forward slash (/) when attempting to remove it \n ", Red));
		throw HTTPError(409);
	}
	else
	{
		if (deleteFolderContents(dataPool.Path))
		{
			DEBUGMSGT(1, COLORED("\n 	success remove all Directory content ! \n ", Green));
			throw HTTPError(204);
		}
		else
		{
			if (hasWritePermission(dataPool.Path)){
				DEBUGMSGT(1, COLORED("\n 	Even though the directory has write permissions, the removal operation is still encountering issues! \n ", Red));
				throw HTTPError(500);
			}
			DEBUGMSGT(1, COLORED("\n 	The removal operation failed due to insufficient write permissions on the directory. ! \n ", Red));
			throw HTTPError(403);
		}
	}
	return false;
}

bool MethodDelete::HandleRequest(std::string &data)
{
	DEBUGMSGT(1, COLORED("\n 	Delete Method Handler : \n", Blue));
	(void)data;
	if (directoryStatus(dataPool.Path) == DIRE)
		return DeleteDirectoryHandler();
	if (directoryStatus(dataPool.Path) == VALID_PATH ){
		if (std::remove(dataPool.Path.c_str()) == 0)
			throw HTTPError(204);
		throw HTTPError(403);
	}
	throw HTTPError(404);
}