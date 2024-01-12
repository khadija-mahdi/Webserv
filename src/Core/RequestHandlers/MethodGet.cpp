#include "Include/MethodGet.hpp"
#define vebros 0

MethodGet::MethodGet(DataPool &Data) : Request(Data) {}

MethodGet::~MethodGet() {}

bool MethodGet::processRedirection()
{
	if (dataPool.REDIRECTION_STAGE)
	{
		dataPool.response.Location = dataPool.Path;
		return true;
	}
	return false;
}

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

std::string autoIndexListenDir(DataPool &dataPool)
{
	dirent *entry;
	std::string buffer;
	std::string cur_path;
	DIR *dir;
	struct stat fileInfo;

	std::string FileName = "/tmp/" + Lstring::RandomStr(16);
	std::ofstream autoindex(FileName.c_str());
	dir = opendir(dataPool.Path.c_str());
	if (dir == NULL)
		return "";
	buffer = TemplateStart(dataPool.Path);
	while ((entry = readdir(dir)) != NULL)
	{
		if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
			buffer.append(CreateRow(1, entry->d_name, 0, ""));
		else
		{
			cur_path = dataPool.Path + entry->d_name;
			stat(cur_path.c_str(), &fileInfo);
			buffer.append(CreateRow(entry->d_type == DT_DIR ? 1 : 2,
									entry->d_name, (long long)fileInfo.st_size / (1024 * 1024),
									GetPermission(fileInfo)));
		}
	}
	buffer.append("</tbody></table></body></html>");
	autoindex << buffer;
	closedir(dir);
	autoindex.close();
	dataPool.response.contentType = "text/html";
	return FileName;
}
std::string getCorrectIndex(std::vector<std::string> &indexes, std::string &Path)
{

	for (size_t i = 0; i < indexes.size(); ++i)
	{
		std::string newPath = Path + indexes[i];
		if (directoryStatus(newPath) == 2)
			return newPath;
	}
	return "";
}

bool MethodGet::handleDirectoryPath()
{
	DEBUGMSGT(0, "GET Handler Directory : auto index : " << dataPool.currentLocation.getAutoindex());

	std::vector<std::string> indexes;
	indexes = dataPool.currentServer.getIndex();
	if (dataPool.currentLocation.getIndex().size() > 0)
		indexes = dataPool.currentLocation.getIndex();
	std::string Path = getCorrectIndex(indexes, dataPool.Path);
	if (indexes.size() > 0 && !Path.empty())
	{
		std::string extention = GetFileExtention(Path);
		if (dataPool.currentLocation.hasCgi(extention))
			return (Request::Execute(Path, "GET"), false);
		else if (!Path.empty())
		{
			dataPool.Path = Path;
			return GetFileHandler();
		}
	}
	if (access(dataPool.Path.c_str(), R_OK) != 0 || dataPool.currentLocation.getAutoindex() == "off")
		throw HTTPError(403);

	dataPool.response.fileFd = open(autoIndexListenDir(dataPool).c_str(), O_RDONLY, 0664);
	return true;
}

bool MethodGet::GetFileHandler()
{
	DEBUGMSGT(vebros, "GET OPENED FILE : ");
	std::string extention = GetFileExtention(dataPool.Path);
	if (dataPool.currentLocation.hasCgi(extention))
		return (Request::Execute(dataPool.Path, "GET"), false);
	else
	{
		int fd = open(dataPool.Path.c_str(), O_RDONLY, 0664);
		DEBUGMSGT(vebros, "Tried to Open File " << dataPool.Path);
		if (fd == -1)
			throw HTTPError(403);
		dataPool.response.fileFd = fd;
	}
	return true;
}

bool MethodGet::GetDirectoryHandler()
{
	DEBUGMSGT(vebros, COLORED("\n 		GET Method Directory Handler : \n ", Green));
	if (dataPool.Path[dataPool.Path.length() - 1] != '/')
	{
		dataPool.response.Location = dataPool.url + "/";
		throw HTTPError(301);
	}
	if (dataPool.Path[dataPool.Path.length() - 1] == '/')
		return handleDirectoryPath();
	throw HTTPError(404);
}

bool MethodGet::HandleRequest(std::string &data)
{
	(void)data;
	DEBUGMSGT(vebros, COLORED("\n 		GET Method Handler : \n ", Yellow));
	if (directoryStatus(dataPool.Path) == VALID_PATH)
		return GetFileHandler();
	return GetDirectoryHandler();
}
