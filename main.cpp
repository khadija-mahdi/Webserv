#include "Webserv.hpp"
#include <string>

#include <sstream>

#include <iostream>
#include <fstream>
#include <string>
#include <map>

bool isDigit(std::string &value){
    for (int i = 0; i < value.length() -2  ; i++)
    {
        if (!std::isdigit(value[i]))
        {
            std::cout << value[i];
            return false;
        }
    }
    return true;
}

void   spratKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag){
    std::string keyValue = "";
    size_t pos = block.find(word);
    if (pos == std::string::npos && flag == 1)
        throw std::runtime_error("events block error");
    size_t end = block.find("}");
    for(int i = pos ; i < end ; i++)
        keyValue += block[i];
    int vstart = 0;
    int vend = keyValue.find("\t");
    value = keyValue.substr(vstart, vend);
    for(; keyValue[vend] && keyValue[vend] == '\t'; vend++);
    int kEnd = keyValue.find(";");
    key = keyValue.substr(vend, kEnd);
}

int linesLength(std::string& Block){
    int len = 0;
    for (size_t i = 0 ; Block[i]; i++)
    {
        if (Block[i] == '\n')  
            len++;
    }
    return len;
}

void eventsBlock(std::string& events){
    Configurations::Events event;
    std::string line;
    std::string value = "";
    std::string key = "";
    spratKeyValue(events, key, value, "worker_connections", 1);
    event.setWorkerConnections(atoi(key.c_str()));
    if (isDigit(key) || linesLength(events) != 2)
        std::cout  << linesLength(events) << " = " << event.getWorkerConnections() << std::endl;
    else
        throw std::runtime_error("events Block error!!");
}

void httpBlock(std::string& http){
    Configurations::Http httpCl;
    std::string line;
    std::string value = "";
    std::string key = "";
    spratKeyValue(http, key, value, "client_max_body_size", 0);
    httpCl.setMax_body_size(key);
    if (linesLength(http) != 2)
        std::cout  << linesLength(http) << " = " << httpCl.getMax_body_size() << std::endl;
    else
        throw std::runtime_error("http Block error!!");
}

void parsingValues(std::string& lines){
    std::istringstream BlockStream(lines);
    std::string line;

    while (std::getline(BlockStream, line)) {
        int serversCout = 0;
        int locationsCount = 0;
        int httpCount = 0;
        int eventsCount = 0;
        int evPos = lines.find("events {");
        int hPos = lines.find("http {");
        int evEnd;
        if (line == "events {"){
            eventsCount++;
            std::string events = Blocks(lines, "events");
            eventsBlock(events);
            std::cout << events << std::endl;
        }
        else if (line == "http {"){
            httpCount++;
            int hEnd = lines.find("}", hPos);
            std::string http = lines.substr(hPos, hEnd);
            // httpBlock(http);
            std::cout << http << std::endl;
        }
    }
}

void pacingConfigFile(){
    try
    {
        std::string lines = PreProcessingFile();
        parsingValues(lines);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

int main(){
    pacingConfigFile();
}