#include "HttpEventHandler.hpp"

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
    this->client.address = address;
    this->client.address_len = address_len;

}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
    this->client.address_len = sizeof(client.address);
}

int HttpEventHandler::Read()
{
   
    return (-1);
}


int HttpEventHandler::Write()
{
   
    return (-1);
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
