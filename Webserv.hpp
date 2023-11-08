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


class Location;
class Server;
class Configurations;

std::string PreProcessingFile();
std::string Blocks(const std::string& lines, const std::string& blockName);
std::map<std::string, std::string> extractKeyValues(const std::string& config);