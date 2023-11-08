#include "Webserv.hpp"

std::string locationSyntax(std::string word){
    std::string locWord = "";
    int i;
    for(i = 0; word[i] &&( word[i] == ' ' || word[i] == '\t'); i++);
    for(; word[i] && word[i] != ' '; i++)
        locWord += word[i];
    if (locWord == "location")
        return word;
    throw std::runtime_error("syntax error in :" + word);
    return NULL;
}


bool isInFormat(std::string& word, std::string& line) {
    if (word == "events " || word == "http " 
        || word == "	server " ||word ==  locationSyntax(word)){
        if (word + "{" != line)
            throw std::runtime_error("syntax error in :" + line);
    }
    return true;
}


void syntaxForm(std::string &line){
    int flag = 0;
    std::string word = "";
    for(int i = 0; line[i] ; i++){
        if (line[i] == '{'){
            flag++;
            word = line.substr(0,i);
            isInFormat(word, line);
        }
        if (line[i] == '}')
            flag++;
    }
    if(!flag) {
        if (line[line.size() - 1] != ';')
            throw std::runtime_error("syntax error in :" + line);
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
    std::fstream conf("config_file.conf");
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
    int Breakets = 0;
    int flag = 0;
    for(int i = 0; lines[i] ; i++){
        if (lines[i] == '{'){
            flag++;
            Breakets++;
        }
        else if (lines[i] == '}')
            Breakets--;
    }
    if (Breakets || !flag)
        throw std::runtime_error("syntax error in curly brackets.");
}


std::string PreProcessingFile(){
    std::string lines = "";
    try
    {    
        readConfigFile(lines);
        CurlyBrackets(lines);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return lines;
}
