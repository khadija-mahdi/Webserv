
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
    location.errorPagesSter(locationBlock);
    return location;
}

void locationInServer(std::string &ServerBlock, Server &ServerConfig){
    std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
    Values loc;
    int start, end;
    
    extractedBlocks = loc.BlocksS(ServerBlock,"location");
    for (size_t i = 0; i < extractedBlocks.size(); ++i) {
        Location location = parseLocationBlock(extractedBlocks[i].first);
        ServerConfig.addLocation(location);
        start = extractedBlocks[0].second.first;
        end = extractedBlocks[i].second.second;
    }
    ServerBlock = ServerBlock.substr(0, start) + ServerBlock.substr(end+1);
}

Server parseServerBlock(std::string &serverBlock) {
    Server server;
    std::istringstream BlockStream(serverBlock);
    std::string line = "";
    std::string value = "";
    int errorCode = 0;
    locationInServer(serverBlock, server);
    singleData(server,serverBlock);
    server.errorPagesSter(serverBlock);
    return server;
}


void ServerInHttp(std::string &httpBlock,  Configurations::Http &httpConfig){
    std::vector<std::pair<std::string, std::pair<int, int> > > extractedBlocks;
    Values serv;
    int start, end;
    
    extractedBlocks = serv.BlocksS(httpBlock,"server");
    for (size_t i = 0; i < extractedBlocks.size(); ++i) {
        Server server = parseServerBlock(extractedBlocks[i].first);
        httpConfig.addServer(server);
        start = extractedBlocks[0].second.first;
        end = extractedBlocks[i].second.second;
    }
    httpBlock = httpBlock.substr(0, start) + httpBlock.substr(end+1);
}

void parseHttpBlock(std::string &httpBlock) {
    Configurations::Http httpConfig;
    int st;
    std::string path = "";

    std::map<std::string, std::string> values;
    ServerInHttp(httpBlock, httpConfig); //! Parse "server" blocks
    values = extractKeyValues(httpBlock);
    std::map<std::string, std::string>::iterator it = values.begin();
    for (; it != values.end(); ++it) {
        if (it->first == "include")
            includeMimeTypes(it->second); //? open mime.types and set this values in map inside the http class
        else if(it->first == "default_type")
                httpConfig.setDefault_type(it->second);// set the default path ;
        else if(it->first == "client_max_body_size")
            httpConfig.setMax_body_size(it->second);
        else if (it->first == "error_page" && processErrors(path, st, it->second))
            httpConfig.setError_pages(path, st);
        else 
            throw std::runtime_error("http wrong key !");
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
}
