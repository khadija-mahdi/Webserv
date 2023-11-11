
#include "Webserv.hpp"

// std::string &splitBlocks(std::string &mainBlock, std::string &nameChild){
//     std::string childBlock = "";
//      int pos = 0;
//     std::istringstream BlockStream(mainBlock);
//     std::string line;

//     while (std::getline(BlockStream, line)) {
//         int start = mainBlock.find(, pos);
//         childBlock += line + "\n";
//         else
//             continue;

//     }
// }
void parseEventsBlock(std::string& events){ //!  Events done, it dons't accept any other values  just "{worker_connections only}";
    Configurations::Events event;
    std::string value = "";
    std::string key = "";
    splitKeyValue(events, key, value, "worker_connections", 1);
    if (isDigit(key) || linesLength(events) == 2)
        event.setWorkerConnections(atoi(key.c_str()));
    else
        throw std::runtime_error("events Block error!!");
    std::cout  << linesLength(events) << " = " << event.getWorkerConnections() << std::endl;
}

Location parseLocationBlock(std::string &locationBlock) {
    Location location;

    // Find and set the value of "root"
    std::size_t rootPos = locationBlock.find("root");
    if (rootPos != std::string::npos) {
        std::size_t semiColonPos = locationBlock.find(";", rootPos);
        location.setRoot(locationBlock.substr(rootPos + 4, semiColonPos - rootPos - 4));
    }

    // Find and set the value of "index"
    std::size_t indexPos = locationBlock.find("index");
    if (indexPos != std::string::npos) {
        std::size_t semiColonPos = locationBlock.find(";", indexPos);
        std::string indexValue = locationBlock.substr(indexPos + 5, semiColonPos - indexPos - 5);
        
        // Split the index values separated by spaces
        std::istringstream indexStream(indexValue);
        std::string indexItem;
        while (indexStream >> indexItem) {
            location.getIndex().push_back(indexItem);
        }
    }

    // Find and set the value of "autoindex"
    std::size_t autoindexPos = locationBlock.find("autoindex");
    if (autoindexPos != std::string::npos) {
        std::size_t semiColonPos = locationBlock.find(";", autoindexPos);
        location.setAutoindex(locationBlock.substr(autoindexPos + 9, semiColonPos - autoindexPos - 9));
    }

    // Add similar logic for other fields in the Location class...

    return location;
}


void singleData(Server & server, std::string &serverBlock){
    std::string value = "";
    std::string key = "";
    splitKeyValue(serverBlock, key, value, "listen", 1);
    if (isDigit(key))
        server.setListen(atoi(key.c_str()));
    splitKeyValue(serverBlock, key, value, "host", 1);
    if (!key.empty())
        server.setHost(key);
    splitKeyValue(serverBlock, key, value, "root", 1);
    if (!key.empty())
        server.setRoot(key);
    splitKeyValue(serverBlock, key, value, "server_names", 0);
    if (!key.empty())
    {
        std::istringstream iss(key);
        std::string word;
        while (iss >> word) {
            server.setServer_names(word);
        }
        for (size_t i = 0; i < server.getServer_names().size(); ++i) {
            std::cout << "name " << i + 1 << ": " << server.getServer_names()[i] << std::endl;
        }
    }
}

void processErrorPage(const std::string& line, std::string &value, int& errorCode) {
    std::istringstream iss(line);
    std::string key;

    iss >> key;

    if (key == "error_page" && (iss >> key)) {
        std::istringstream keyStream(key);

        if (keyStream >> errorCode) {
            iss >> value;
        } else {
            throw std::runtime_error("Invalid error page: " + line);
        }
    }
}


void processRedirection(const std::string& line, std::string& value, int& errorCode) {
    std::istringstream iss(line);
    std::string key;

    iss >> key;

    if (key == "return") {
        if (iss >> errorCode) {
            if (iss >> value) {
                // Check if there is more than 3 words
                std::string word;
                if (iss >> word) {
                    throw std::runtime_error("Invalid redirection: " + line);
                }
            } else {
                throw std::runtime_error("Invalid redirection: " + line);
            }
        } else {
            // Case: "return path"
            errorCode = 0; // Set a default error code if not provided
            iss.clear();   // Clear the error flag
            if (iss >> value) {
                // Check if there is exactly 2 words
                std::string word;
                if (iss >> word) {
                    throw std::runtime_error("Invalid redirection: " + line);
                }
                return;
            } else {
                throw std::runtime_error("Invalid redirection: " + line);
            }
        }
    }
}

Server parseServerBlock(std::string &serverBlock) {
    Server server;
    std::istringstream BlockStream(serverBlock);
    std::string line = "";
    std::string value = "";
    int errorCode = 0;

    singleData(server, serverBlock);

    while (std::getline(BlockStream, line)) {
     
        processErrorPage(line, value, errorCode);
        if (!value.empty()){
            server.setError_pages(value, errorCode);
            value = "";
        }
        processRedirection(line, value, errorCode);
        if (!value.empty()){
            server.setRedirection(value, errorCode);
            value = "";
        }
    }
    server.printErrorPages();
    server.printRedirection();

    size_t pos = serverBlock.find("location");
    while (pos != std::string::npos) {
        size_t end = serverBlock.find("{", pos);
        std::string locationBlock = serverBlock.substr(pos, end - pos + 1);
        Location location = parseLocationBlock(locationBlock);
        server.addLocation(location);
        pos = serverBlock.find("location", end);
    }
    return server;
}


void parseHttpBlock(std::string &httpBlock) {
    std::istringstream BlockStream(httpBlock);
    std::string line = "";
    std::string valuep = "";
    int errorCode = 0;
    Configurations::Http httpConfig;
    std::string value = "";

    //! Parse "server" blocks
    size_t pos = httpBlock.find("server");
    int i = 0;
    while (pos != std::string::npos) {
        size_t end = httpBlock.find("}", pos);
        std::string serverBlock = httpBlock.substr(pos, end - pos + 1);
        Server server = parseServerBlock(serverBlock);
        httpConfig.addServer(server);

        pos = httpBlock.find("server", end);
    }
    std::string key = "";
    splitKeyValue(httpBlock, key, value, "client_max_body_size", 0);
    if (!key.empty())
        httpConfig.setMax_body_size(key);
    std::cout << "MAX body size value : " << httpConfig.getMax_body_size() << std::endl;

    //-> Add logic to parse values the "http" block : includes !!
    
        while (std::getline(BlockStream, line)) {
     
        processErrorPage(line, valuep, errorCode);
        if (!valuep.empty()){
            httpConfig.setError_pages(valuep, errorCode);
            valuep = "";
        }
    }
    httpConfig.printErrorPages();

}


void parsingValues(std::string &lines) {
    std::istringstream BlockStream(lines);
    std::string line;

    while (std::getline(BlockStream, line)) {
        if (line == "events {") {
            std::string events = Blocks(lines, "events");
            parseEventsBlock(events);
        } else if (line == "http {") {
            std::string httpBlock = Blocks(lines, "http");
            parseHttpBlock(httpBlock);
        }
    }
}
