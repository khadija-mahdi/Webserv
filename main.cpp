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
            throw std::runtime_error("key is not integer");
    }
    return true;
}

void   splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag){
    std::string keyValue = "";
    size_t pos = block.find(word);
    if (pos == std::string::npos && flag == 1)
        throw std::runtime_error("block error : " + block);
    size_t end = block.find("}");
    for(int i = pos ; i < end ; i++)
        keyValue += block[i];
    int vstart = 0;
    int vend = keyValue.find("\t");
    value = keyValue.substr(vstart, vend);
    for(; keyValue[vend] && keyValue[vend] == '\t'; vend++);
    int kEnd = keyValue.find(";");
    key = keyValue.substr(vend, kEnd - vend);
    std::cout << "key " << key << ", value : " << value << std::endl;
}

// void   splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag){
//         std::string keyValue = "";
//         size_t pos = block.find(word);
//         if (pos == std::string::npos && flag == 1)
//             throw std::runtime_error("block error " + block);
//         size_t end = block.find("}");
//         for(int i = pos ; i < end ; i++)
//             keyValue += block[i];
//         int vstart = 0;
//         int vend = keyValue.find("\t");
//         value = keyValue.substr(vstart, vend);
//         for(; keyValue[vend] && keyValue[vend] == '\t'; vend++);
//         int kEnd = keyValue.find(";");
//         key = keyValue.substr(vend, kEnd - vend);
//         std::cout << "key " << key << ", value : " << value << std::endl;
// }

int linesLength(std::string& Block){
    int len = 0;
    for (size_t i = 0 ; Block[i]; i++)
    {
        if (Block[i] == '\n')  
            len++;
    }
    return len;
}

// void eventsBlock(std::string& events){
//     Configurations::Events event;
//     std::string line;
//     std::string value = "";
//     std::string key = "";
//     splitKeyValue(events, key, value, "worker_connections", 1);
//     event.setWorkerConnections(atoi(key.c_str()));
//     // std::cout  << linesLength(events) << " = " << event.getWorkerConnections() << std::endl;
//     if (isDigit(key) || linesLength(events) != 2);
//     else
//         throw std::runtime_error("events Block error!!");
// }

// void httpBlock(std::string& http){
//     Configurations::Http httpCl;
//     std::string value = "";
//     std::string key = "";
//     int serversCout = 0;
// int locationsCount = 0;
//     splitKeyValue(http, key, value, "client_max_body_size", 0);
//     httpCl.setMax_body_size(key);
//     if (linesLength(http) != 2)
//         std::cout  << linesLength(http) << " = " << httpCl.getMax_body_size() << std::endl;
//     else
//         throw std::runtime_error("http Block error!!");
// }

// void parsingValues(std::string& lines){
//     std::istringstream BlockStream(lines);
//     std::string line;

//     while (std::getline(BlockStream, line)) {
//         int httpCount = 0;
//         int eventsCount = 0;
//         if (line == "events {"){
//             eventsCount++;
//             std::string events = Blocks(lines, "events");
//             // eventsBlock(events);
//             // std::cout << events << std::endl;
//         }
//         else if (line == "http {"){
//             httpCount++;
//             std::string http = Blocks(lines, "http");;
//             httpBlock(http);
//             std::cout << http << std::endl;
//         }
//     }
// }

void pacingConfigFile(){
    try
    {
        Configurations::Events eventsConfig;
        Configurations::Http httpConfig;
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