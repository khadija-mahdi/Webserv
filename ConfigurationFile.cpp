#include "Webserv.hpp"

std::string &skepComment(std::string &line) {
    for (int i =0; line[i]; i++){
        if (line[i] == '#')
            return line = line.substr(0, i);
    }
    return line;
}

bool lineSpace(std::string &line) {
    for (int i =0; line[i]; i++){
        if (line[i] != ' ' && line[i] != '\t')
            return 0;
    }
    return 1;
}

void readConfigFile(std::string &lines){
    std::string line = "";
    std::fstream conf("config_file");
    if (conf.is_open())
    {   
        while (std::getline(conf, line, '\n')){
            line = skepComment(line);
            if (line.empty() || lineSpace(line))
                continue;
            lines+= line + "\n";
        }
        conf.close();
    }
    else
        throw std::runtime_error("Error: could not open file.");
}