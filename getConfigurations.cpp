#include "Webserv.hpp"

#include <iostream>
#include <string>

std::string Blocks(const std::string& lines, const std::string& blockName) {
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
            extractedBlock = lines.substr(openBrace, closeBrace - openBrace);
            std::cout << extractedBlock << std::endl;
            pos = closeBrace;
        } else {
            break;
        }
    }
    return extractedBlock;
}


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
    {
        for(; lines[i] && lines[i] && lines[i] != '}'; i++)
            events += lines[i];
    }
    std::cout << "[" << events << "]";
}