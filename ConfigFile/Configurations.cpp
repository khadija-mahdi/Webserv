#include "Configurations.hpp"

//---------------------------------Events class -------------------------------
int Configurations::Events::worker_connections = 0;
Http Configurations::http;

int Configurations::Events::getWorkerConnections(){return worker_connections;}

void    Configurations::Events::setWorkerConnections(int worCon){ worker_connections = worCon;}


//------------------------------------------Http class --------------------------------------------


std::string                 Http::getMax_body_size(){return max_body_size;}
std::string                 Http::getDefault_type() {return default_type;}

std::map<int, std::string>  Http::getError_pages() { return error_pages; }
std::map<std::string, std::string>  Http::getIncludes()const{ return Includes; }

void        Http::setMax_body_size(std::string &bodySize){ max_body_size = bodySize;}
void        Http::setDefault_type(std::string &defTyp){ default_type = defTyp;}

void        Http::setError_pages(std::string & _value, int _key){error_pages[_key] = _value;}
void        Http::setIncludes(std::string const& _value, std::string const& _key){Includes[_key] = _value;}
void	    Http::addServer(ConfServer const &loc){ConfServers.push_back(loc);}


