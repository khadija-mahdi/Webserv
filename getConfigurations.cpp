#include "Webserv.hpp"

void Blocks(std::string &lines){
    std::string events = "";
    std::string http = "";
    std::string server = "";
    std::string location = "";
    std::string word = "";
    int i;
    for (i = 0; lines[i] && lines[i] != '{'; i++);
    word = lines.substr(0,i);
    if (word == "events ")
    for(; lines[i] && lines[i]; i++)
        events += lines[i];
    std::cout << lines;
}