#pragma once 

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <map>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <stack>
#include <vector>
#include <deque>
#include "ConfigFile/Configurations.hpp"

void readConfigFile(std::string &lines);
void CurlyBrackets(std::string &lines);

class configurations {
public:
    class events {
        int worker_connections;
    public:
        int getWorkerConnections() const{
           return worker_connections; 
        }
    };
};
