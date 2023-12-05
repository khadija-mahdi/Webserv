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
	std::string rep = "";
	requestParser.RequestParser(ss.str(), fd);
	response.setRespose(rep);
	this->response_now = true;
	return (-1);
}
int HttpEventHandler::Write()
{
	std::stringstream chunk;
	if (!response_now)
		return (-1);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int bytesRead = read(fd, buffer, sizeof(buffer));
	if (bytesRead < 0) {
		throw std::runtime_error("Error reading from file descriptor");
	}

	if (bytesRead == 0) {
		chunk << "0\r\n";
		if (write(this->GetSocketFd(),"0\r\n", strlen("0\r\n")) < 0) 
			throw std::runtime_error("Error writing to socket");
		close(fd);
		return (bytesRead);
	}
	chunk << std::hex << bytesRead << "\r\n";

	chunk.write(buffer, bytesRead);
	chunk << "\r\n";
	std::string httpHeader = "HTTP/1.1 200 OK\r\n"
						"Content-Type: Content-Type: text/plain\r\n"
						"Transfer-Encoding: chunked\r\n"
						"\r\n";
	std::string chunkData = httpHeader +  chunk.str();
	if (write(this->GetSocketFd(), chunkData.c_str(), chunkData.length()) < 0) 
		throw std::runtime_error("Error writing to socket");
	chunk.str("");
	return (0);
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
