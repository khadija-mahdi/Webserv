#include "Include/Request.hpp"

Request::Request(DataPool &dataPool) : dataPool(dataPool)
{
	this->BodyReady = false;
	this->BodyReceiver = NULL;
	this->CGIProcessId = 0;
	this->CgiStart = -1;
	this->cgiController = NULL;
}

int Request::GetRequestedResource()
{
	struct stat ResourceState;

	ResourceFilePath = dataPool.Path;

	DEBUGMSGT(1, COLORED("ResourceFilePath : " << ResourceFilePath, Green));

	if (access(ResourceFilePath.c_str(), F_OK) != 0)
		throw HTTPError(404);

	if (stat(ResourceFilePath.c_str(), &ResourceState) < 0)
		throw std::runtime_error("state() failes at Request.cpp");

	if (S_ISREG(ResourceState.st_mode))
		this->dataPool.ResourceType = WB_FILE;
	else if (S_ISDIR(ResourceState.st_mode))
		this->dataPool.ResourceType = WB_DIRECTORY;
	else
		this->dataPool.ResourceType = WB_NEITHER;

	Redirection redirection = this->dataPool.currentServer.getRedirection();

	if (dataPool.REDIRECTION_STAGE)
	{
		DEBUGMSGT(1, "statusCode :" << redirection.statusCode << " ReturnLocation : " << redirection.ReturnLocation);
		this->dataPool.response.Location = redirection.ReturnLocation;
		throw HTTPError(redirection.statusCode);
	}
	return true;
}

std::string Request::GetIndex(std::string &path)
{
	dirent *entry;
	DIR *dir;
	std::string CurrentName;
	std::vector<std::string> indexList;

	dir = opendir(path.c_str());
	if (dir == NULL)
		return ("");
	indexList = this->dataPool.currentServer.getIndex();
	if (indexList.empty())
		return (closedir(dir), "");
	while ((entry = readdir(dir)) != NULL)
	{
		CurrentName = entry->d_name;
		for (size_t i = 0; i < indexList.size(); i++)
		{
			if (CurrentName == indexList.at(i))
				return (closedir(dir), CurrentName);
		}
	}
	return (closedir(dir), "");
}

void Request::PrintfFullRequest()
{
	bool Print = false;
	DEBUGMSGT(Print, COLORED("------------------------------", Green));
	DEBUGMSGT(Print, COLORED("Requested File  " << this->ResourceFilePath << " Method : " << this->dataPool.Method, Green));
	for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
		DEBUGMSGT(Print, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
	DEBUGMSGT(Print, COLORED("------------------------------", Green));
}

std::string Request::GetFileExtention(std::string &FilePath)
{
	size_t LastDotPos;
	if ((LastDotPos = FilePath.find_last_of(".")) != std::string::npos)
		return (FilePath.substr(LastDotPos + 1));
	return ("");
}

void Request::SetBodyController(int Type, u_int64_t Remaining)
{
	if (Type == Chunked)
		this->BodyReceiver = new ChunkController(this->dataPool);
	else if (Type == Lenght)
		this->BodyReceiver = new LenghtController(this->dataPool, Remaining);
}

void Request::Execute(std::string ResourceFilePath, std::string Method)
{
	std::string BodyFile = this->BodyReceiver ? this->BodyReceiver->GetFileName() : "";
	cgiController = new CGIController(dataPool, ResourceFilePath, BodyFile, Method);
}

CGIController *Request::GetCGIController()
{
	return this->cgiController;
}

pid_t Request::GetRunningProcessId()
{
	if (this->cgiController)
		return this->cgiController->GetRunningProcessId();
	return (0);
}

Request::~Request()
{
	delete this->BodyReceiver;
	delete this->cgiController;
}