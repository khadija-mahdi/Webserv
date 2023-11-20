#include "Configurations.hpp"

int linesLength(std::string& Block){
    int len = 0;
    for (size_t i = 0 ; Block[i]; i++)
    {
        if (Block[i] == '\n')  
            len++;
    }
    return len;
}

bool isDigit(std::string &value){
    for (int i = 0; i < value.length() -2  ; i++)
    {
        if (!std::isdigit(value[i]))
            throw std::runtime_error("key is not integer : " + value);
    }
    return true;
}


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


void syntaxForm(std::string &line, int check){
    int flag = 0;
    std::string word = "";
    for(int i = 0; line[i] ; i++){
        if (line[i] == '{'){
            flag++;
            word = line.substr(0,i);
            if (check == 1)
                isInFormat(word, line);
        }
        if (line[i] == '}')
            flag++;
    }
    if(!flag) {
        if (line[line.size() - 1] != ';')
            throw std::runtime_error("syntax error in no ; in end of line :" + line);
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


void readConfigFile(std::string &lines, int flag){
    std::string line = "";
    std::fstream conf("config/config_file.conf");
    if (conf.is_open())
    {   
        while (std::getline(conf, line, '\n')){
            line = skepComment(line);
            if (line.empty() || lineSpace(line))
                continue;
            while(line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
                line = line.erase(line.size() -1);
            syntaxForm(line, flag); 
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
    
    readConfigFile(lines, 1);
    CurlyBrackets(lines);
    return lines;
}