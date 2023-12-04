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
	if (bytes < 0)
		std::cout << "No bytes are there to read" << std::endl;
	ss.write(buffer, bytes);
	requestParser.RequestParser(ss.str());
	this->response_now = true;
	return (-1);
}

int HttpEventHandler::Write()
{
	if (!response_now)
		return (-1);
	char resp[] = "HTTP/1.0 200 OK\r\n"
				  "Server: webserver-c\r\n"
				  "Content-Length: 316\r\n"
				  "Content-type: text/html\r\n\r\n"
				  "<html>"
				  "<head><title>My Webpage</title></head>"
				  "<body>"
				  "<h1>Hello, Web!</h1>"
				  "<p>This is a simple webpage.</p>"
				  "<img src=\"https://helpx.adobe.com/content/dam/help/en/photoshop/using/convert-color-image-black-white/jcr_content/main-pars/before_and_after/image-before/Landscape-Color.jpg\" alt=\"An example image\">"
				  "</body>"
				  "</html>\r\n";
	int valWrite = write(this->GetSocketFd(), resp, strlen(resp));
	if (valWrite < 0)
	{
		DEBUGOUT(1, COLORED("Write failed " << valWrite, Red));
		exit(1);
	}
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
