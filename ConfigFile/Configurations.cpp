#include "Configurations.hpp"

//---------------------------------Events class -------------------------------

int Configurations::Events::getWorkerConnections() const{return worker_connections;}

void    Configurations::Events::setWorkerConnections(int worCon){ worker_connections = worCon;}

void    Configurations::Events::eventsBlock(std::string &lines) {

    std::string extractedBlock = Blocks(lines, "events");
    if (!extractedBlock.empty())
        return;
    std::map<std::string, std::string>  value = extractKeyValues(extractedBlock);
    if (!value.size())
        return;
    if (value.size() == 1){
        const char *integerV = value["worker_connections"].c_str();
        int intvalue = atoi(integerV);
        // std::cout << intvalue << std::endl;
        setWorkerConnections(intvalue);
    }
    else{
        throw std::runtime_error("events blocks error");
    }
}



//------------------------------------------Http class --------------------------------------------

std::string                 Configurations::Http::getMax_body_size() const{return max_body_size;}

std::map<int, std::string>  Configurations::Http::getError_pages() const { return error_pages; }

void        Configurations::Http::setMax_body_size(std::string &bodySize){ max_body_size = bodySize;}

void        Configurations::Http::setError_pages(std::string & _value, int _key){error_pages[_key] = _value;}
void	    Configurations::Http::addServer(Server const &loc){Servers.push_back(loc);}


//--------------------------------------------location class --------------------------------------

std::vector<std::string>    Location::getIndex() const{return index;}

std::map<int, std::string>  Location::getRedirection() const{return redirection;}

std::map<int, std::string>  Location::getError_pages() const{return error_pages;}

std::string                 Location::getRoot() const{return root;}

std::string                 Location::getAutoindex() const{return autoindex;}

std::string                 Location::getAllow() const{return allow;}

std::string                 Location::getUpload() const{return upload;}

std::string                 Location::getUpload_stor() const{return upload_stor;}



void    Location::setIndex(std::string const & _index){index.push_back(_index);}

void    Location::setError_pages(std::string & _value, int _key){error_pages[_key] = _value;;}

void    Location::setRedirection(std::string & _value, int _key){ redirection[_key] = _value;}

void    Location::setRoot(std::string const &_root){ root = _root;}

void    Location::setAutoindex(std::string const& _autoindex){ autoindex = _autoindex;}

void    Location::setUpload(std::string const& _upload){ upload = _upload;}

void    Location::setUpload_stor(std::string const& _upload_stor){ upload_stor = _upload_stor;}

void    Location::setAllow(std::string const& _allow){ allow = _allow;}


//-----------------------------------Server class ------------------------------------------


std::vector<std::string>    Server::getServer_names() const{return server_names;}

std::map<int, std::string>  Server::getRedirection() const{return redirection;}

std::map<int, std::string>  Server::getError_pages() const{return error_pages;}

std::string                 Server::getRoot() const{return root;}

std::string                 Server::getHost() const{return host;}

int                         Server::getListen() const{return listen;}


void    Server::setServer_names(std::string const & _name){ server_names.push_back(_name);}

void    Server::setError_pages(std::string & _value, int _key){ error_pages[_key] = _value;}

void    Server::setRedirection(std::string & _value, int _key){ redirection[_key] = _value;}

void    Server::setRoot(std::string const &_root){ root = _root;}

void    Server::setHost(std::string const& _Host){ host = _Host;}

void    Server::setListen(int lis){ listen = lis;}


void	 Server::addLocation(Location const &loc){Locations.push_back(loc);}
