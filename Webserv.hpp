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
// Configurations parsingValues(std::string &lines);
#define SSRT(x) static_cast<std::ostringstream &>(std::ostringstream() << x).str()
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

typedef enum Colors
{
	Black = 30,
	Red = 31,
	Green = 32,
	Yellow = 33,
	Blue = 34,
	Magenta = 35,
	Cyan = 36,
	White = 37,
	Reset = 0,
} t_colors;

#define COLORED(str, color) "\033[1;" << color << "m" << str << "\033[0m"

/**
 * 1-                   activate for all
 * 2-                   activate/deactivated by setting active
 * <any other number>-  deactivated
 */
#ifndef DEBUGE
#define DEBUGE 2
#endif

#if DEBUGE == 1
#define DEBUGOUT(active, x) \
	std::cout << x << std::endl
#elif DEBUGE == 2
#define DEBUGOUT(active, x) \
	if (active)             \
	std::cout << x << std::endl
#else
#define DEBUGOUT(active, x) \
	std::ostringstream() << x << active;
#endif

std::string read_file_content(const std::string& file_path);