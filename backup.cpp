#include "Webserv.hpp"

#include <iostream>
#include <string>

std::string Blocks(const std::string& lines, const std::string& blockName){
    int pos = 0; 
    std::string extractedBlock = "";

    while (pos < lines.length()) {
        int start = lines.find(blockName, pos);
        if (start == std::string::npos)
            break;
        int openBrace = lines.find('{', start);
        if (openBrace == std::string::npos)
            break;
        int closeBrace = openBrace + 1;
        int bracesCount = 1;

        while (bracesCount > 0 && closeBrace < lines.length()) {
            if (lines[closeBrace] == '{') {
                bracesCount++;
            } else if (lines[closeBrace] == '}') {
                bracesCount--;
            }
            closeBrace++;
        }
        if (bracesCount == 0) {
            extractedBlock += lines.substr(openBrace, closeBrace - openBrace);
            pos = closeBrace;
        } else {
            break;
        }
    }
    return extractedBlock;
}

std::map<std::string, std::string> extractKeyValues(const std::string& Block) {
    std::map<std::string, std::string> keyValues;
    std::istringstream BlockStream(Block);
    std::string line;

    while (std::getline(BlockStream, line)) {
        std::vector<std::string> parts;
        std::istringstream lineStream(line);
        std::string part;
        while (lineStream >> part) {
            parts.push_back(part);
        }
        if (parts[0] == "{" || parts[0] == "}")
            continue;

        if (parts.size() >= 2) {
            std::string key = parts[0];
            std::string value;
            for (size_t i = 1; i < parts.size(); i++) {
                value += parts[i] + " ";
            }
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (!value.empty() && value[value.size() -1]== ';') {
                value.resize(value.length() - 1);
            }
            keyValues[key] = value;
        } else {
            throw std::runtime_error("syntax error key value. [" + line + "]");
        }
    }
    return keyValues;
}


std::map<std::string, std::string> extractValueForKey(std::string& block,std::string key) {
    std::map<std::string, std::string> keyValueMap;
    std::istringstream iss(block);
    std::string line;

    while (std::getline(iss, line)) {
        size_t pos = line.find(key);
        if (pos != std::string::npos) {
            size_t valuePos = pos + key.length();
            std::string value = line.substr(valuePos);
            value = value.substr(value.find_first_not_of(" \t"));
            value = value.substr(0, value.find_last_not_of(" \t") + 1);
            keyValueMap[key] = value;
        }
    }

    return keyValueMap;
}
