#pragma once
#include "../Webserv.hpp"

struct Redirection {
    std::string	ReturnLocation;
    int			statusCode;
};
class Location {
private:

	std::string					root;
	std::vector<std::string>	index;
	std::map<int, std::string>	error_pages;
	Redirection					redirection; // path , status code
	std::string					autoindex; // on / of
	std::vector<std::string>	allow; // method get post delete
	std::string					upload; // on , of
	std::string					upload_stor; // path to stor upload


public:

	std::vector<std::string>	getIndex() const;
	std::map<int, std::string>  getError_pages() const;
	Redirection					getRedirection() const;
	std::string					getRoot() const;
	std::string					getAutoindex() const;
	std::vector<std::string>	getAllow() const;
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
	Redirection					redirection; // path , status code
	std::string					root;
	std::vector<Location>		Locations;

public:

	std::string					getHost() const;
	int							getListen() const;
	std::string					getRoot() const;
	std::vector<std::string>	getServer_names() const;
	std::map<int, std::string>  getError_pages() const;
	Redirection					getRedirection() const;

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
		std::string							max_body_size;
		std::string							default_type;
		std::map<int, std::string>			error_pages;
		std::vector<Server>					Servers;
		std::map<std::string, std::string>	Includes;
	public:

		void						setMax_body_size(std::string &);
		void						setError_pages(std::string & _value, int _key);
		void 						addServer(Server const &);
		void						setIncludes(std::string &, std::string );
		void						setDefault_type(std::string &);
	
		std::vector<Server>			getServe() const{
			return Servers;
		}
		std::string							getMax_body_size() const;
		std::string							getDefault_type() const;
		std::map<int, std::string>			getError_pages() const;
		std::map<std::string, std::string>	getIncludes() const;

		void	printErrorPages(){
			std::map<int, std::string>::iterator it = error_pages.begin();
			for (; it != error_pages.end(); ++it) {
				std::cout<< "http error : " << it->first << " => " << it->second << "\n";
			}
		}
	};
};


class Values {
    std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
public:
    std::vector<std::pair<std::string, std::pair<int, int> > > &BlocksS(const std::string &lines, const std::string &blockName) {
        int pos = 0;

        while (pos < lines.length()) {
            int start = lines.find(blockName, pos);
            if (start == std::string::npos)
                break;
            int openBrace = lines.find('{', start);
            if (openBrace == std::string::npos)
                break;
            int closeBrace = openBrace + 1;
            int bracesCount = 1;

            while (bracesCount > 0 && closeBrace < lines.length()) {
                if (lines[closeBrace] == '{') {
                    bracesCount++;
                } else if (lines[closeBrace] == '}') {
                    bracesCount--;
                }
                closeBrace++;
            }
            if (bracesCount == 0) {
                std::string block = lines.substr(openBrace, closeBrace - openBrace);
                extractedBlocks.push_back(std::make_pair(block, std::make_pair(start, closeBrace)));
                pos = closeBrace;
            } else {
                break;
            }
        }
        return extractedBlocks;
    }
    void printPairVect(std::string &lines){
        extractedBlocks = BlocksS(lines, "server");
        std::cout << extractedBlocks.size() << std::endl;
        for (size_t i = 0; i < extractedBlocks.size(); ++i) {
            std::cout << "Element " << i + 1 << ": ";
            std::cout << extractedBlocks[i].first << " (" << extractedBlocks[i].second.first << ", " << extractedBlocks[i].second.second << ")\n";
        }
    }
};




std::map<std::string, std::string>	extractKeyValues(const std::string& Block);
std::vector<std::string>			BlocksExtra(const std::string &lines, const std::string &blockName);
std::string							Blocks(const std::string& lines, const std::string& blockName);
bool								isDigit(std::string &value);
int									linesLength(std::string& Block);
void								splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag);
void								singleData(Server & server, std::string &serverBlock);
void								includeMimeTypes(std::string &fileName);
void								CurlyBrackets(std::string &lines);
void								readConfigFile(std::string &lines, int flag);
void 								syntaxForm(std::string &line, int check);
std::string 						&skepComment(std::string &line);
bool								lineSpace(std::string &line);
void								locationValues(Location &location, std::string &locationBlock);
bool								processErrors(std::string& path, int &status, std::string &key);
bool								isDigitStr(std::string &value);


