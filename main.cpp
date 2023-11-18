#include "Webserv.hpp"
#include <string>

void pacingConfigFile(){
    Configurations::Events eventsConfig;
    Configurations::Http httpConfig;
    std::string lines = PreProcessingFile();
    parsingValues(lines);
    
}

int main(){
    try
    {
        pacingConfigFile();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}