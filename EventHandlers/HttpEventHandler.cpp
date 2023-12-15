#include "HttpEventHandler.hpp"

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
	this->client.address = address;
	this->client.address_len = address_len;
	this->response_now = false;
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
	this->client.address_len = sizeof(client.address);
}

int HttpEventHandler::Read()
{
	std::ostringstream ss;
	char buffer[1024];
	memset(buffer, 0, 1024);
	int bytes = read(this->SocketFd, buffer, 1024);
	if (bytes <= 0)
		return 0;

	ss.write(buffer, bytes);

	requestParser.RequestHandler(ss.str(), response);
	response.setRequest(requestParser.headerData.Path);
	this->response_now = true;
	return (-1);
}

int HttpEventHandler::Write()
{
	if (!response_now)
		return (-1);
	response.clearResponseBuffer();
	return response.sendResponse(this->GetSocketFd());
}

EventHandler *HttpEventHandler::Accept(void)
{
	return (NULL);
}

const int &HttpEventHandler::GetSocketFd() const
{
	return this->SocketFd;
}

HttpEventHandler::~HttpEventHandler()
{
}
