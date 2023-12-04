#include "Reactor.hpp"

Reactor::Reactor()
{
	if ((this->epoll_fd = epoll_create1(0)) < 0)
		throw std::runtime_error("epoll_create1() failed");
	this->MAX_EVENTS = 1024;
	this->events = new struct epoll_event[this->MAX_EVENTS];
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{
	struct epoll_event event;
	if (eventHandler == NULL)
		return;
	DEBUGOUT(1, COLORED("Regester New "
							<< (dynamic_cast<AcceptEventHandler *>(eventHandler) != NULL ? "Server " : "Client ")
							<< "Socket " << SSRT(socketFd) << "\n",
						Blue));
	event.events = EPOLLRDNORM | EPOLLWRNORM;
	event.data.fd = eventHandler->GetSocketFd();
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, eventHandler->GetSocketFd(), &event) < 0)
		throw std::runtime_error("epoll_ctl() `EPOLL_CTL_ADD` failed");

	this->clients[socketFd] = eventHandler;
}

void Reactor::UnRegisterSocket(int SocketFd)
{
	std::string Type = dynamic_cast<AcceptEventHandler *>(this->clients[SocketFd]) != NULL ? "Server " : "Client ";
	DEBUGOUT(1, COLORED("UnRegister " << Type << "Socket "
									  << SSRT(SocketFd) << "\n",
						Red));
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, SocketFd, NULL) < 0)
		throw std::runtime_error("epoll_ctl() `EPOLL_CTL_DEL` failed");

	if (this->clients[SocketFd] != NULL)
	{
		delete clients[SocketFd];
		this->clients.erase(SocketFd);
	}
	close(SocketFd);
}

void Reactor::HandleEvents()
{
	if ((event_count = epoll_wait(this->epoll_fd, this->events, this->MAX_EVENTS, -1)) < 0)
		throw std::runtime_error("epoll_wait() failed");
	Dispatch();
}

void Reactor::Dispatch()
{
	AcceptEventHandler *server;
	HttpEventHandler *client;
	int current_fd;
	for (int i = 0; i < this->event_count; i++)
	{
		current_fd = this->events[i].data.fd;
		if (this->events[i].events & EPOLLRDNORM)
		{
			if ((server = dynamic_cast<AcceptEventHandler *>(this->clients[current_fd])) != NULL)
			{
				client = dynamic_cast<HttpEventHandler *>(server->Accept());
				if (client != NULL)
					return RegisterSocket(client->GetSocketFd(), client);
			}
			else if ((client = dynamic_cast<HttpEventHandler *>(this->clients[current_fd])) != NULL)
			{

				if (client->Read() == 0)
					return UnRegisterSocket(current_fd);
			}
		}
		else if (this->events[i].events & EPOLLWRNORM)
		{
			if ((client = dynamic_cast<HttpEventHandler *>(this->clients[current_fd])) != NULL)
			{
				if (client->Write() == 0)
					return UnRegisterSocket(current_fd);
			}
		}
	}
}

void Reactor::EventLoop()
{
	while (true)
	{
		HandleEvents();
	}
}

Reactor::~Reactor()
{
	delete[] this->events;
	if (close(this->epoll_fd) < 0)
		throw std::runtime_error("close(epoll_fd) failed");
}