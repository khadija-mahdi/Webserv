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
#include "ConfigFile/ConfServer.hpp"
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <utility> // For std::pair

class Location;
class ConfServer;
class Configurations;
class Http;
class Values;

std::string PreProcessingFile();
Configurations pacingConfigFile();
// Configurations parsingValues(std::string &lines);

template <typename T>
bool Containes(std::vector<T> Vec, T item)
{
	for (typename std::vector<T>::iterator i = Vec.begin(); i != Vec.end(); i++)
	{
		if (*i == item)
			return (true);
	}
	return (false);
}