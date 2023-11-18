#pragma once 

#include <iostream>
#include <string>
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
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <utility> // For std::pair



class Location;
class Server;
class Configurations;
class Http;
class Values;

std::string PreProcessingFile();
void pacingConfigFile();
void parsingValues(std::string &lines);