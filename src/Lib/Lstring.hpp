#ifndef LSTRING_HPP
#define LSTRING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "IO.hpp"

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
#define DEBUGMSGT(active, x) \
	std::cout << x << std::endl
#elif DEBUGE == 2
#define DEBUGMSGT(active, x) \
	if (active)              \
	std::cout << x << std::endl
#else
#define DEBUGMSGT(active, x) \
	std::ostringstream() << x << active;
#endif

class Lstring
{
private:
	Lstring();
	Lstring &operator=(Lstring &copy);

public:
	static std::vector<std::string> Split(std::string line, std::string delimiter);
	static std::vector<std::string> SplitByOneOf(std::string line, std::string delimiter);
	static std::string ExtractFromString(std::string data, std::string start, std::string end, std::string defaultReturn = "");
	static std::string &tolower(std::string &str);
	static std::string RandomStr(size_t len);
	static std::string encode_binary_to_text(const std::string &input);
	static bool IsAlNum(const std::string &input, size_t pos, size_t n = std::string::npos);
	static bool Contains(const std::string &input, std::string &checklist, size_t pos);
	static std::string &Trim(std::string &input, std::string delim = " ");
	static std::string &LTrim(std::string &input, std::string delim = " ");
	static std::string &RTrim(std::string &input, std::string delim = " ");
	static std::string &Replace(std::string &input, std::string target, std::string _new);
};

#endif
