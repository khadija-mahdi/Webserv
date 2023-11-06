#include "Webserv.hpp"



int main(){
    
    try
    {
        std::string lines;
        readConfigFile(lines);
        std::cout << lines;
        // CurlyBrackets(lines);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}