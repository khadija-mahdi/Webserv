#ifndef READEVENTHANDLER_HPP
#define READEVENTHANDLER_HPP
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EventHandler.hpp"
#include <sstream>
#include <unistd.h>
#include "Request.hpp"

class Request;

class Client
{
public:
	struct sockaddr_storage address;
	socklen_t address_len;
};

class HttpEventHandler : public EventHandler
{
public:
	HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len);
	HttpEventHandler();
	virtual ~HttpEventHandler();

public:
	int Read();
	int Write();
	EventHandler *Accept();
	const int &GetSocketFd() const;

private:
	Request requestParser;
	Client client;
	bool response_now;
};

#endif