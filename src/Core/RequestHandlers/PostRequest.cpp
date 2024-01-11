#include "Include/PostRequest.hpp"

#define DEBUG_GETREQUEST 1

PostRequest::PostRequest(DataPool &dataPool) : Request(dataPool)
{
	this->UploadBodyState = ZERO;
}

bool PostRequest::HandleRequest(std::string &data)
{

	if (this->UploadBodyState == ZERO && GetRequestedResource())
		return (PrintfFullRequest(), dataPool.response.StatusCode = OK, true);
	if (this->UploadBodyState == UP_INPROGRESS || this->UploadBodyState == CGI_INPROGRESS || this->UploadBodyState == EMPTY_INPROGRESS)
	{
		if (!this->BodyReady)
			this->BodyReady = BodyReceiver ? BodyReceiver->Receiver(data) : false;
		if (this->BodyReady)
		{
			if (this->UploadBodyState == UP_INPROGRESS)
				return (this->UploadBodyState = DONE,
						dataPool.response.StatusCode = CREATED, true);
			else if (this->UploadBodyState == CGI_INPROGRESS)
				return (Execute(ResourceFilePath, "POST"),
						this->UploadBodyState = DONE, false);
			else if (this->UploadBodyState == EMPTY_INPROGRESS)
			{
				unlink(this->BodyReceiver->GetFileName().c_str());
				return (this->UploadBodyState = DONE,
						dataPool.response.StatusCode = 301, true);
			}
		}
	}
	return false;
}

int PostRequest::GetRequestedResource()
{

	Request::GetRequestedResource();
	std::string IndexFileName;

	if (this->dataPool.ResourceType == WB_DIRECTORY)
	{
		if (*(this->dataPool.url.end() - 1) != '/')
		{
			this->dataPool.response.Location = this->dataPool.url + "/";
			throw HTTPError(301);
		}

		if (((IndexFileName = GetIndex(ResourceFilePath)).empty() &&
			 !this->dataPool.currentLocation.getUpload()) &&
			!this->dataPool.REDIRECTION_STAGE)
			throw HTTPError(403);

		ResourceFilePath.append(IndexFileName);
	}

	FileExtention = GetFileExtention(ResourceFilePath);
	if (this->dataPool.REDIRECTION_STAGE)
	{
		this->BodyReceiver->CreateFile("", true);
		return (this->UploadBodyState = EMPTY_INPROGRESS, false);
	}
	if (this->dataPool.currentLocation.hasCgi(FileExtention))
	{
		this->BodyReceiver->CreateFile("", true);
		this->BodyReceiver->SetIsCGI(true);
		return (this->UploadBodyState = CGI_INPROGRESS, false);
	}
	if (this->dataPool.currentLocation.getUpload())
	{
		if (!this->BodyReceiver)
			throw HTTPError(403);
		this->BodyReceiver->SetIsCGI(false);
		if (GetHeaderAttr(dataPool.Headers, "Content-Type").find("boundary=") == std::string::npos)
			this->BodyReceiver->CreateFile();
		return (this->UploadBodyState = UP_INPROGRESS, false);
	}
	throw HTTPError(403);
}

PostRequest::~PostRequest() {}