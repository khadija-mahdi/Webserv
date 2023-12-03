#include "ConfServer.hpp"

std::vector<std::string>    ConfServer::getConfServer_names() const{return ConfServer_names;}

Redirection                 ConfServer::getRedirection() const{return redirection;}

std::map<int, std::string>  ConfServer::getError_pages() const{return error_pages;}

std::string                 ConfServer::getRoot() const{return root;}

std::string                 ConfServer::getHost() const{return host;}

int                         ConfServer::getListen() const{return listen;}


void    ConfServer::setConfServer_names(std::string const & _name){ ConfServer_names.push_back(_name);}

void    ConfServer::setError_pages(std::string & _value, int _key){ error_pages[_key] = _value;}

void    ConfServer::setRedirection(std::string & _value, int _key){
    redirection.ReturnLocation = _value;
    redirection.statusCode = _key;
}

void    ConfServer::setRoot(std::string const &_root){ root = _root;}

void    ConfServer::setHost(std::string const& _Host){ host = _Host;}

void    ConfServer::setListen(int lis){ listen = lis;}


void	 ConfServer::addLocation(Location const &loc){Locations.push_back(loc);}

