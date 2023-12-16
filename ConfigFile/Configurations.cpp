#include "Configurations.hpp"
#include "../Webserv.hpp"

//---------------------------------Events class -------------------------------
int		Configurations::Events::worker_connections = 0;
Http	Configurations::http;

int		Configurations::Events::getWorkerConnections(){return worker_connections;}

void    Configurations::Events::setWorkerConnections(int worCon){ worker_connections = worCon;}


//------------------------------------------Http class --------------------------------------------

std::string                 		Http::getMax_body_size(){return max_body_size;}
std::string                 		Http::getDefault_type() {return default_type;}
std::map<int, std::string>			Http::getError_pages() { return error_pages; }
std::map<std::string, std::string> 	Http::getIncludes()const{ return Includes; }
std::vector<ConfServer>				Http::getConfServes(){return ConfServers;}
void        						Http::setMax_body_size(std::string &bodySize){ max_body_size = bodySize;}
void        						Http::setDefault_type(std::string &defTyp){ default_type = defTyp;}

void        						Http::setError_pages(std::map<int , std::string>& maperr){error_pages = maperr;}
void        						Http::setIncludes(std::string const& _value, std::string const& _key){Includes[_key] = _value;}
void	    						Http::addServer(ConfServer const &loc){ConfServers.push_back(loc);}

//------------------------------------------Values class --------------------------------------------

void checkValues(const std::string &lines) {
    std::stringstream errorMessage;
    std::istringstream lineStream(lines);
    std::string line;
    size_t lineNumber = 0;
    int nextLine = 0;

    while (std::getline(lineStream, line)) {

        if (nextLine == 1) {
            size_t openBrace = line.find('{');
            size_t closeBrace = line.find('}');

            if (openBrace == std::string::npos && closeBrace == std::string::npos) {
                errorMessage << "\033[1;31mError: Syntax error. Please add The Value inside Braces " << "\033[0m" << std::endl;
                throw std::runtime_error(errorMessage.str());
            }

            nextLine = 0;
        }

        size_t openBrace = line.find('}');
        if (openBrace != std::string::npos)
            nextLine = 1;
    }
}

std::vector<std::pair<std::string, std::pair<int, int> > >& Values::BlocksS(const std::string &lines, const std::string &blockName)
{
	int pos = 0;
	checkValues(lines);
	while (pos < lines.length())
	{
		int start = lines.find(blockName, pos);
		if (start == std::string::npos)
			break;
		int openBrace = lines.find('{', start);
		if (openBrace == std::string::npos)
			break;
		if (blockName == "location"){
			ConfServer server;
			std::string path = lines.substr(start + 9 , openBrace - (start + 10));
			paths.push_back(path);
		}
		int closeBrace = openBrace + 1;
		int bracesCount = 1;
		while (bracesCount > 0 && closeBrace < lines.length())
		{
			if (lines[closeBrace] == '{'){
				bracesCount++;
			}
			else if (lines[closeBrace] == '}')
				bracesCount--;
			closeBrace++;
		}
		if (bracesCount == 0)
		{
			std::string block = lines.substr(openBrace, closeBrace - openBrace);
			extractedBlocks.push_back(std::make_pair(block, std::make_pair(start, closeBrace)));
			pos = closeBrace;
		}
		else
			break;
	}
	return extractedBlocks;
}

std::vector<std::string>	Values::getPaths() const  {return paths;}
void						Values::setPaths(std::string const &nPAThs){ paths.push_back(nPAThs);}

