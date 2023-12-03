#ifndef ACCEPTEVENTHANDLER_HPP
#define ACCEPTEVENTHANDLER_HPP
#include <vector>
#include <map>
#include "EventHandler.hpp"
#include "../Reactor.hpp"
#include <fcntl.h>

class AcceptEventHandler : public EventHandler
{

public:
    AcceptEventHandler(int SocketFd);

public:
    EventHandler *Accept();
	int Read();
    int Write();
};

#endif