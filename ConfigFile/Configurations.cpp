#include "Configurations.hpp"

//---------------------------------Events class -------------------------------

int Configurations::Events::getWorkerConnections() const{return worker_connections;}

void    Configurations::Events::setWorkerConnections(int worCon){ worker_connections = worCon;}


//------------------------------------------Http class --------------------------------------------

std::string                 Configurations::Http::getMax_body_size() const{return max_body_size;}

std::map<int, std::string>  Configurations::Http::getError_pages() const { return error_pages; }


void        Configurations::Http::setMax_body_size(std::string &bodySize){ max_body_size = bodySize;}

void        Configurations::Http::setError_pages(std::map<int, std::string> &errors){error_pages = errors;}


 //--------------------------------------------location class --------------------------------------

std::vector<std::string>    Location::getIndex() const{return index;}

std::map<int, std::string>  Location::getRedirection() const{return redirection;}

std::map<int, std::string>  Location::getError_pages() const{return error_pages;}

std::string                 Location::getRoot() const{return root;}

std::string                 Location::getAutoindex() const{return autoindex;}

std::string                 Location::getAllow() const{return allow;}

std::string                 Location::getUpload() const{return upload;}

std::string                 Location::getUpload_stor() const{return upload_stor;}



void    Location::setIndex(std::vector<std::string> const & errors){index = errors;}

void    Location::setError_pages(std::map<int, std::string> const & errors){error_pages = errors;}

void    Location::setRedirection(std::map<int, std::string> const & errors){ redirection = errors;}

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


void    Server::setServer_names(std::vector<std::string> const & errors){ server_names = errors;}

void    Server::setError_pages(std::map<int, std::string> const & errors){ error_pages = errors;}

void    Server::setRedirection(std::map<int, std::string> const & errors){ redirection = errors;}

void    Server::setRoot(std::string const &_root){ root = _root;}

void    Server::setHost(std::string const& _Host){ host = _Host;}

void    Server::setListen(int lis){ listen = lis;}

