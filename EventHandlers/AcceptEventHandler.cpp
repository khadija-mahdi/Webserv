#include "AcceptEventHandler.hpp"

AcceptEventHandler::AcceptEventHandler(int SocketFd) : EventHandler(SocketFd)
{
}


EventHandler *AcceptEventHandler::Accept(void)
{
    return (NULL);
}

int AcceptEventHandler::Read()
{
    return (-1);
}

int AcceptEventHandler::Write()
{
    return (-1);
}

