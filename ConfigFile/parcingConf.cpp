
#include "Configurations.hpp"


void parseEventsBlock(std::string & events){ //!  Events done, it dons't accept any other values  just "{worker_connections only}";
    Configurations::Events event;
    std::string value = "";
    std::string key = "";
    splitKeyValue(events, key, value, "worker_connections", 1);
    if (isDigit(key) || linesLength(events) == 2)
        event.setWorkerConnections(atoi(key.c_str()));
    else
        throw std::runtime_error("events Block error!!");
}

Location parseLocationBlock(std::string &locationBlock) {
    Location location;
    locationValues(location, locationBlock);
    // location.printErrorPages();
    return location;
}

void locationInServer(std::string &ServerBlock, ConfServer &ConfServerConfig){
    std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
    Values loc;
    int start, end;
    extractedBlocks = loc.BlocksS(ServerBlock,"location");
	std::vector<std::string> paths =  loc.getPaths();
    if (extractedBlocks.size() == 0)
        return;
    for (size_t i = 0; i < extractedBlocks.size(); ++i) {
        Location location = parseLocationBlock(extractedBlocks[i].first);

		location.setPath(paths[i]);
        ConfServerConfig.addLocation(location);
		if (location.getPath() == "/")
			ConfServerConfig.setDefaultLocation(i);
        start = extractedBlocks[0].second.first;
        end = extractedBlocks[i].second.second;
    }
    ServerBlock = ServerBlock.substr(0, start) + ServerBlock.substr(end+1);
}

ConfServer parseServerBlock(std::string &serverBlock) {
    ConfServer server;
    std::istringstream BlockStream(serverBlock);
    locationInServer(serverBlock, server);
    singleData(server,serverBlock);
    // server.printErrorPages();
    return server;
}


void ServerInHttp(std::string &httpBlock, Http &httpConfig){
	ConfServer server;
    std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
    Values serv;
    int start, end;
    
    extractedBlocks = serv.BlocksS(httpBlock,"server");
    if(extractedBlocks.size() < 1)
        throw std::runtime_error("block server not found ");
    for (size_t i = 0; i < extractedBlocks.size(); ++i) {
        server = parseServerBlock(extractedBlocks[i].first);
        httpConfig.addServer(server);
        start = extractedBlocks[0].second.first;
        end = extractedBlocks[i].second.second;
    }

    httpBlock = httpBlock.substr(0, start) + httpBlock.substr(end+1);
}

void processErrorPage(const std::string& line, std::string &path, int& status) {
    std::istringstream iss(line);
    std::string word;

    iss >> word;
    int i = -1;
    if (word == "error_page") {
        while (iss >> word) {
            i++;
            if (isDigitStr(word) && i == 0) {
                status = atoi(word.c_str());
                i++;
            } 
            else if (!isDigitStr(word) && i == 1){
                i++;
                path = word;
            }    
            else if ((i != 2) || status < 100 || status > 599)
                throw std::runtime_error("not a true error pages syntax: " + line);
        }
    }
}

void parseHttpBlock(std::string &httpBlock) {
    std::istringstream BlockStream(httpBlock);
    std::string line = "";
    int st;
    std::string path = "";

    std::map<std::string, std::string> values;
    ServerInHttp(httpBlock,Configurations::http); //! Parse "server" blocks
    values = extractKeyValues(httpBlock);
    std::map<std::string, std::string>::iterator it = values.begin();
    for (; it != values.end(); ++it) {
        if (it->first == "include")
            includeMimeTypes(it->second); //? open mime.types and set this values in map inside the http class
        else if(it->first == "default_type")
            Configurations::http.setDefault_type(it->second);// set the default path ;
        else if(it->first == "client_max_body_size")
        Configurations::http.setMax_body_size(it->second);
        else if (it->first == "error_page"){
            while (std::getline(BlockStream, line)) {
                processErrorPage(line, path, st);
            Configurations::http.setError_pages(path, st);
                path = "";
            }
        }
        else 
            throw std::runtime_error("http wrong key !");
        //Configurations::http.printErrorPages();
    }

}


void parsingValues(std::string &lines) {
    std::istringstream BlockStream(lines);
    std::string line;
    Values val;
    size_t pos = lines.find("events");
    if (pos != std::string::npos){
        std::vector<std::string> events = BlocksExtra(lines, "events");
        if (events.size() != 1)
            throw std::runtime_error("events Blocks duplicate");
        std::string event = events[0];
        parseEventsBlock(event);
        pos = 0;
    }
    pos = lines.find("http");
    if (pos != std::string::npos) {
        std::vector<std::string> https = BlocksExtra(lines, "http");
        if (https.size() != 1)
            throw std::runtime_error("http Block duplicate");
        std::string httpBlock = https[0];
        parseHttpBlock(httpBlock);
    }
    else
        throw std::runtime_error("http Block not exist");
}

void pacingConfigFile(){
    std::string lines = PreProcessingFile();
    parsingValues(lines);
    
}
