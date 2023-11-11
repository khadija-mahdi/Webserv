#pragma once
#include "../Webserv.hpp"

class Location {
private:
	std::string					root;
	std::vector<std::string>	index;
	std::map<int, std::string>	error_pages;
	std::map<int, std::string>	redirection; // path , status code
	std::string					autoindex; // on / of
	std::string					allow;
	std::string					upload;
	std::string					upload_stor;

public:

	std::vector<std::string>	getIndex() const;
	std::map<int, std::string>  getError_pages() const;
	std::map<int, std::string>	getRedirection() const;
	std::string					getRoot() const;
	std::string					getAutoindex() const;
	std::string					getAllow() const;
	std::string					getUpload() const;
	std::string					getUpload_stor() const;

	void	setIndex(std::string const & _name );
	void	setRedirection(std::string & _value, int _key);
	void	setError_pages(std::string & _value, int _key);
	void	setRoot(std::string  const &);
	void	setAutoindex(std::string  const &);
	void	setAllow(std::string  const &);
	void	setUpload(std::string  const &);
	void	setUpload_stor(std::string  const &);
	void	printErrorPages(){
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it) {
			std::cout<< "error loc : " << it->first << " => " << it->second << "\n";
		}
	}
};

class Server {
	std::string					host; //localhost / 127.0.0.1 /0.0.0.0
	int							listen; //(port)
	std::vector<std::string>	server_names;
	std::map<int, std::string>	error_pages;
	std::map<int, std::string>	redirection; // path , status code
	std::string					root;
	std::vector<Location>		Locations;

public:
	std::string					getHost() const;
	int							getListen() const;
	std::string					getRoot() const;
	std::vector<std::string>	getServer_names() const;
	std::map<int, std::string>  getError_pages() const;
	std::map<int, std::string>	getRedirection() const;

	void	setHost(std::string  const &);
	void	setListen(int);
	void	setRoot(std::string  const &);
	void	setServer_names(std::string const & _name);
	void	setRedirection(std::string & _value, int _key);
	void	setError_pages(std::string & _value, int _key);
	void	addLocation(Location const &);
	void	printErrorPages(){
		std::map<int, std::string>::iterator it = error_pages.begin();
		for (; it != error_pages.end(); ++it) {
			std::cout<< "server error : " << it->first << " => " << it->second << "\n";
		}
	}
	void	printRedirection(){
		std::map<int, std::string>::iterator it = redirection.begin();
		for (; it != redirection.end(); ++it) {
			std::cout<< "red : " << it->first << " => " << it->second << "\n";
		}
	}
	
};

class Configurations {
public:

	class Events {
		int	worker_connections;
	public:
		void	setWorkerConnections(int);
		int		getWorkerConnections() const;
		void	eventsBlock(std::string &lines);
	};

	class Http {
	private:
		std::string					max_body_size;
		std::map<int, std::string>	error_pages;
		std::vector<Server>			Servers;
	public:

		void						setMax_body_size(std::string &);
		void						setError_pages(std::string & _value, int _key);
		void 						addServer(Server const &);
		std::string					getMax_body_size() const;
		std::map<int, std::string>	getError_pages() const;
		void	printErrorPages(){
			std::map<int, std::string>::iterator it = error_pages.begin();
			for (; it != error_pages.end(); ++it) {
				std::cout<< "http error : " << it->first << " => " << it->second << "\n";
		}
	}
	};
};