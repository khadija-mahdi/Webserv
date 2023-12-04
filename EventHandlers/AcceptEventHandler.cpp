#include "AcceptEventHandler.hpp"

AcceptEventHandler::AcceptEventHandler(int SocketFd) : EventHandler(SocketFd)
{
}

EventHandler *AcceptEventHandler::Accept(void)
{
	struct sockaddr_storage addr;
	socklen_t addrlen;
	int socket;
	int flags;

	addrlen = sizeof(addrlen);
	socket = accept(this->SocketFd, (struct sockaddr *)&addr, &addrlen);
	if (socket < 0)
		throw std::runtime_error("accept() failed");
	flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl() failed");
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	return (new HttpEventHandler(socket, addr, addrlen));
}

int AcceptEventHandler::Read() // that for complier dyal zmer errors noting useful here 
{
	return (-1);
}

int AcceptEventHandler::Write() // that shit too complier dyal zmer errors noting useful here 
{
	return (-1);
}
