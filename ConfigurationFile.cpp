#include "Webserv.hpp"

bool isInFormat(std::string& word, std::string& line) {
    if (word + "{" != line)
        return false;
    return true;
}

void syntaxForm(std::string &line){
    int flag = 0;
    std::string word = "";
    for(int i = 0; line[i] ; i++){
        if (line[i] == '{'){
            word = line.substr(0,i);
            if (word == "events " || word == "http " 
                || word == "    server " || word == "        location / ")
                isInFormat(word, line);
            else
            {
                std::cout << line ;
                throw std::runtime_error("syntax error !!: {} ");
            }
        }
    }
}

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
            syntaxForm(line); 
            lines += line + "\n";
        }
        conf.close();
    }
    else
        throw std::runtime_error("Error: could not open file.");
}

void CurlyBrackets(std::string &lines){
    std::stack<int> Breakets;
    int flag = 0;
    for(int i = 0; lines[i] ; i++){
        if (lines[i] == '{'){
            flag++;
            Breakets.push(1);
        }
        else if (lines[i] == '}')
            Breakets.pop();
    }
    if (flag == 0 || Breakets.size() > 0);
        throw std::runtime_error("syntax error !!: ");
}
