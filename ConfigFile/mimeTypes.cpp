#include "Configurations.hpp"
std::map<std::string, std::string> extractKeyValuesIN(const std::string& Block) {
    std::map<std::string, std::string> keyValues;
    std::istringstream BlockStream(Block);
    std::string line;
    int i = 0;

    while (std::getline(BlockStream, line)) {
        std::vector<std::string> parts;
        std::istringstream lineStream(line);
        std::string part;

        while (lineStream >> part)
            parts.push_back(part);
        if (parts[0] == "{" || parts[0] == "}")
            continue;

        if (parts.size() >= 2) {
            std::string key;
            for (size_t i = 1; i < parts.size(); i++)
                key += parts[i] + " ";
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);

            if (!key.empty() && key[key.size() - 1] == ';')
                key.resize(key.length() - 1);

            std::string values = parts[0];
            std::istringstream keyStream(key);
            std::string individualKey;
            while (std::getline(keyStream, individualKey, ' '))
                keyValues[individualKey] = values;
        }
        else 
            throw std::runtime_error("syntax error values value. [" + line + "]");
    }

    return keyValues;
}

void includeMimeTypes(std::string &_file){
    std::map<std::string, std::string> keyValues;
    Configurations::Http http;
    std::string line = "";
    std::string lines = "";
    std::fstream fileName(_file.c_str());
    
    if (fileName.is_open()){
        while (std::getline(fileName, line, '\n')){
            line = skepComment(line);
            if (line.empty() || lineSpace(line))
                continue;
            while(line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
                line = line.erase(line.size() -1);
            syntaxForm(line, 0); 
            lines += line + "\n";
        }
        fileName.close();
        CurlyBrackets(lines);
        lines = Blocks(lines, "type");
        keyValues = extractKeyValuesIN(lines);
        std::map<std::string, std::string>::iterator it = keyValues.begin();
        for (; it != keyValues.end(); ++it)
            http.setIncludes(it->second, it->first);
    }
    else
        throw std::runtime_error("Error: could not open file.");

}