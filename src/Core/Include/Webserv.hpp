#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <map>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <stack>
#include <vector>
#include <deque>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <sys/stat.h>
#include <utility>
#include <fcntl.h>
#include <algorithm>
#include <dirent.h>

typedef std::map<std::string, std::string>::iterator HeaderIterator;

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

std::string read_file_content(const std::string &file_path);