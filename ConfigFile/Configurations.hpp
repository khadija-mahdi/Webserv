#include "../Webserv.hpp"

class Location {
private:
	std::vector<std::string>	index;
	std::map<int, std::string>	error_pages;
	std::map<int, std::string>	redirection; // path , status code
	std::string					root;
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

	void	setIndex(std::vector<std::string> const & );
	void	setRedirection(std::map<int, std::string> const & );
	void	setError_pages(std::map<int, std::string> const & );
	void	setRoot(std::string  const &);
	void	setAutoindex(std::string  const &);
	void	setAllow(std::string  const &);
	void	setUpload(std::string  const &);
	void	setUpload_stor(std::string  const &);
};

class Server {
	std::vector<std::string>	server_names;
	std::map<int, std::string>	error_pages;
	std::map<int, std::string>	redirection; // path , status code
	std::string					root;
	std::string					host; //localhost / 127.0.0.1 /0.0.0.0
	int							listen; //(port)
	Location					Locations[];

public:
	std::vector<std::string>	getServer_names() const;
	std::map<int, std::string>  getError_pages() const;
	std::map<int, std::string>	getRedirection() const;
	std::string					getRoot() const;
	std::string					getHost() const;
	int							getListen() const;

	void	setServer_names(std::vector<std::string> const & );
	void	setRedirection(std::map<int, std::string> const & );
	void	setError_pages(std::map<int, std::string> const & );
	void	setRoot(std::string  const &);
	void	setHost(std::string  const &);
	void	setListen(int);
};

class Configurations {
public:

	class Events {
		int	worker_connections;
	public:
		void	setWorkerConnections(int);
		int		getWorkerConnections() const;
	};

	class Http {
	private:
		std::string					max_body_size;
		std::map<int, std::string>	error_pages;
		Server						Servers[];
	public:

		void						setMax_body_size(std::string &);
		void						setError_pages(std::map<int, std::string>& );
		std::string					getMax_body_size() const;
		std::map<int, std::string>	getError_pages() const;
	};
};