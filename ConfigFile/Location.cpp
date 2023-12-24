#include "Location.hpp"


std::vector<std::string>    			Location::getIndex() const{return index;}

Redirection                 			Location::getRedirection() const{return redirection;}

std::map<int, std::string>  			Location::getError_pages() const{return error_pages;}

std::map<std::string, std::string> 		Location::getCgiAccept() const{return cgiAccept;}

std::string                 			Location::getRoot() const{return root;}
			
std::string                 			Location::getPath() const{return path;}
			
std::string                 			Location::getAutoindex() const{return autoindex;}
			
std::vector<std::string>    			Location::getAllow() const{return allow;}
			
std::string                 			Location::getUpload() const{return upload;}
			
std::string                 			Location::getUpload_stor() const{return upload_stor;}


void    Location::setIndex(std::string const & _index){index.push_back(_index);}

void    Location::setError_pages(std::map<int , std::string>& maperr){error_pages = maperr;}

void    Location::setRedirection(std::string & _value, int _key){
    redirection.ReturnLocation = _value;
    redirection.statusCode = _key;
}

// void	Location::setCgi(std::string & _cig_accept, std::string cgi_path)
// {
// 	this->cgiAccept = _cig_accept; 
// 	this->cgiPath = cgi_path;
	
// }
void	Location::setCgiAccept(std::map<std::string, std::string> &cgi){this->cgiAccept = cgi;}

void    Location::setRoot(std::string const &_root){ root = _root;}

void    Location::setPath(std::string const &_Path){ path = _Path;}

void    Location::setAutoindex(std::string const& _autoindex){ autoindex = _autoindex;}

void    Location::setUpload(std::string const& _upload){ upload = _upload;}

void    Location::setUpload_stor(std::string const& _upload_stor){ upload_stor = _upload_stor;}

void    Location::setAllow(std::string const& _allow){ allow.push_back(_allow);}

