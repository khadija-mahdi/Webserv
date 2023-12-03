#include "Configurations.hpp"

//---------------------------------Events class -------------------------------

int Configurations::Events::getWorkerConnections() const{return worker_connections;}

void    Configurations::Events::setWorkerConnections(int worCon){ worker_connections = worCon;}


//------------------------------------------Http class --------------------------------------------

std::string                 Configurations::Http::getMax_body_size() const{return max_body_size;}
std::string                 Configurations::Http::getDefault_type() const{return default_type;}

std::map<int, std::string>  Configurations::Http::getError_pages() const { return error_pages; }
std::map<std::string, std::string>  Configurations::Http::getIncludes() const { return Includes; }

void        Configurations::Http::setMax_body_size(std::string &bodySize){ max_body_size = bodySize;}
void        Configurations::Http::setDefault_type(std::string &defTyp){ default_type = defTyp;}

void        Configurations::Http::setError_pages(std::string & _value, int _key){error_pages[_key] = _value;}
void        Configurations::Http::setIncludes(std::string & _value, std::string _key){Includes[_key] = _value;}
void	    Configurations::Http::addServer(ConfServer const &loc){ConfServers.push_back(loc);}

