#include "ConfServer.hpp"

ConfServer::ConfServer():isDefaultLocation(-1) {}

std::map<int, std::string>  ConfServer::getError_pages() const{return error_pages;}
std::vector<std::string>    ConfServer::getConfServer_names() const{return ConfServer_names;}
std::vector<Location>		ConfServer::getLocations() const{return Locations;}
Redirection                 ConfServer::getRedirection() const{return redirection;}
std::string                 ConfServer::getRoot() const{return root;}
std::string                 ConfServer::getHost() const{return host;}
std::string                 ConfServer::getListen() const{return listen;}
int 						ConfServer::getDefaultLocation() const{return isDefaultLocation;}
std::vector<std::string>    ConfServer::getIndex() const{return index;}


void    ConfServer::setConfServer_names(std::string const & _name){ ConfServer_names.push_back(_name);}

void    ConfServer::setError_pages(std::map<int , std::string>& maperr){error_pages = maperr;}

void    ConfServer::setRedirection(std::string & _value, int _key){
    redirection.ReturnLocation = _value;
    redirection.statusCode = _key;
}

void    ConfServer::setIndex(std::string const & _index){index.push_back(_index);}

void    ConfServer::setRoot(std::string const &_root){ root = _root;}

void    ConfServer::setHost(std::string const& _Host){ host = _Host;}

void    ConfServer::setListen(std::string const &lis){ listen = lis;}

void	ConfServer::setDefaultLocation(int const &a){isDefaultLocation = a;}

void	 ConfServer::addLocation(Location const &loc){Locations.push_back(loc);}

