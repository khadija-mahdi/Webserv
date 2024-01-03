#include "HttpEventHandler.hpp"

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd), 
			headerData(new	DataPool), requestParser(headerData) , response(headerData)
{
	this->client.address = address;
	this->client.address_len = address_len;
	this->headerData->RESPONSE__STATE = false;
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
	this->client.address_len = sizeof(client.address);
}

int HttpEventHandler::Read()
{
	std::ostringstream request;
	char buffer[1024];
	memset(buffer, 0, 1024);
	int bytes = read(this->SocketFd, buffer, 1024);
	if (bytes <= 0)
		return 0;

	request.write(buffer, bytes);
	requestParser.HandlerRequest1(request.str());
	this->headerData->RESPONSE__STATE = true;
	return (-1);
}

int HttpEventHandler::Write()
{
	if (!headerData->RESPONSE__STATE)
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
	delete headerData;
}
