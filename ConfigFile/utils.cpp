#include "Configurations.hpp"

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
}


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


std::vector<std::string> BlocksExtra(const std::string &lines, const std::string &blockName) {
    int pos = 0;
    std::vector<std::string> extractedBlocks;

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
            extractedBlocks.push_back(lines.substr(openBrace, closeBrace - openBrace));
            pos = closeBrace;
        } else {
            break;
        }
    }
    return extractedBlocks;
}


void singleData(Server & server, std::string &serverBlock){
    std::map<std::string, std::string> values;
    int flag = 0;

    values = extractKeyValues(serverBlock);
    std::map<std::string, std::string>::iterator it = values.begin();
    for (; it != values.end(); ++it) {
        if(it->first == "listen" && isDigit(it->second)){
            flag++;
            server.setListen(atoi(it->second.c_str()));
        }
        else if(it->first == "host"){
            flag++;
            server.setHost(it->second.c_str());
        }
        else if(it->first == "root"){
            flag++;
            server.setRoot(it->second.c_str());
        }
        else if(it->first == "server_names")
        {
            std::string key;
            std::istringstream iss(key);
            std::string word;
            while (iss >> word) {
                server.setServer_names(word);
            }
            for (size_t i = 0; i < server.getServer_names().size(); ++i) {
                std::cout << "name " << i + 1 << ": " << server.getServer_names()[i] << std::endl;
            }
        }
        else if (it->first != "error_page" && it->first != "return")
            throw std::runtime_error("server wrong key : " + it->first);
    }
    if (flag != 3)
        throw std::runtime_error("the obligation keys not found");
}


void processErrorPage(const std::string& line, std::string &value, int& errorCode) {
    std::istringstream iss(line);
    std::string key;

    iss >> key;

    if (key == "error_page" && (iss >> key)) {
        std::istringstream keyStream(key);

        if (keyStream >> errorCode) {
            iss >> value;
        } else { 
            throw std::runtime_error("Invalid error page: " + line);
        }
    }
}


void processRedirection(const std::string& line, std::string& value, int& errorCode) {
    std::istringstream iss(line);
    std::string key;

    iss >> key;

    if (key == "return") {
        if (iss >> errorCode) {
            if (iss >> value) {
                // Check if there is more than 3 words
                std::string word;
                if (iss >> word) {
                    throw std::runtime_error("Invalid redirection: " + line);
                }
            } else {
                throw std::runtime_error("Invalid redirection: " + line);
            }
        } else {
            // Case: "return path"
            errorCode = 0; // Set a default error code if not provided
            iss.clear();   // Clear the error flag
            if (iss >> value) {
                // Check if there is exactly 2 words
                std::string word;
                if (iss >> word) {
                    throw std::runtime_error("Invalid redirection: " + line);
                }
                return;
            } else {
                throw std::runtime_error("Invalid redirection: " + line);
            }
        }
    }
}


void errorPagesSter(std::string &httpBlock, Configurations::Http &httpConfig){
    std::string value = "";
    int errorCode = 0;

    std::istringstream BlockStream(httpBlock);
    std::string line = "";
    while (std::getline(BlockStream, line)) {
        processErrorPage(line, value, errorCode);
        if (!value.empty()){
            httpConfig.setError_pages(value, errorCode);
            value = "";
        }
    }
    httpConfig.printErrorPages();
}
