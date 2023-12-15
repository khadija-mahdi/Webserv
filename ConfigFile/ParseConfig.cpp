#include "ParseConfig.hpp"

ParseConfig::ParseConfig(){
	BLOCK_FORMAT = false;
}


void ParseConfig::includeMimeTypes(std::string &_file){
    std::map<std::string, std::string> keyValues;
    std::string line = "";
	std::string _lines = "";
    std::fstream fileName(_file.c_str());
    BLOCK_FORMAT = false;
    if (fileName.is_open()){
        while (std::getline(fileName, line, '\n')){
            line = skepComment(line);
            if (line.empty() || lineSpace(line))
                continue;
            while(line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
                line = line.erase(line.size() -1);
            syntaxForm(line); 
            _lines += line + "\n";
        }
        fileName.close();
        CurlyBrackets();
        _lines = Blocks(_lines, "type");
        keyValues = extractKeyValuesIN(_lines);
        std::map<std::string, std::string>::iterator it = keyValues.begin();
        for (; it != keyValues.end(); ++it)
            Configurations::http.setIncludes(it->second , it->first);
    }
    else
        throw std::runtime_error("Error: could not open file.");

}

void ParseConfig::ParseConfig::syntaxForm(std::string &line){
    int flag = 0;
    std::string word = "";
    for(int i = 0; line[i] ; i++){
        if (line[i] == '{'){
            flag++;
            word = line.substr(0,i);
            if (BLOCK_FORMAT == 1)
                isInFormat(word, line);
        }
        if (line[i] == '}')
            flag++;
    }
    if(!flag) {
        if (line[line.size() - 1] != ';')
            throw std::runtime_error("syntax error in no ; in end of line :" + line);
    }
}

void ParseConfig::readConfigFile(std::string const&file){

    std::string line = "";
    std::ifstream conf(file.c_str());
    if (!conf.is_open()) {
        std::ifstream defaultConf("config/config_file.conf");
        if (!defaultConf.is_open()) {
			errorMessage << "\033[1;" << Red << "mError: " << "could not open file." << "\033[0m" << std::endl;
        	throw std::runtime_error(errorMessage.str());
		}
        conf.close();
        conf.clear(); 
        conf.open("config/config_file.conf", std::ios::in);
        
        if (!conf.is_open()) {
			errorMessage << "\033[1;" << Red << "mError: " << "could not open file." << "\033[0m" << std::endl;
        	throw std::runtime_error(errorMessage.str());
        }
    }
	while (std::getline(conf, line, '\n')){
		line = skepComment(line);
		if (line.empty() || lineSpace(line))
			continue;
		while(line[line.size() - 1] == '\t' || line[line.size() - 1] == ' ')
			line = line.erase(line.size() -1);
		syntaxForm(line); 
		lines += line + "\n";
	}
	conf.close();
}

void ParseConfig::CurlyBrackets(){
    int CloseBrackets = 0;
    int openBrackets = 0;
    for(int i = 0; lines[i] ; i++){
        if (lines[i] == '{'){
            openBrackets++;
            CloseBrackets++;
        }
        else if (lines[i] == '}')
            CloseBrackets--;
    }
    if (CloseBrackets || !openBrackets){
		errorMessage << "\033[1;" << Red << "syntax error in curly Brackets." << "\033[0m" << std::endl;
        throw std::runtime_error(errorMessage.str());
	}
}

void ParseConfig::PreProcessingFile(std::string &file){
    BLOCK_FORMAT = true;
    readConfigFile(file);
    CurlyBrackets();
}

void ParseConfig::parseEventsBlock(std::string & events){
    Configurations::Events event;
    std::string value = "";
    std::string key = "";
    splitKeyValue(events, key, value, "worker_connections", 1);
    if (isDigit(key) || linesLength(events) == 2)
        event.setWorkerConnections(atoi(key.c_str()));
    else
        throw std::runtime_error("events Block error!!");
}

Location ParseConfig::parseLocationBlock(std::string &locationBlock) {
    Location location;
    locationValues(location, locationBlock);
    // location.printErrorPages();
    return location;
}

void ParseConfig::locationInServer(std::string &ServerBlock, ConfServer &ConfServerConfig){
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
	// ConfServerConfig.printErrorPages();
}

ConfServer ParseConfig::parseServerBlock(std::string &serverBlock) {
    ConfServer server;
    std::istringstream BlockStream(serverBlock);
    locationInServer(serverBlock, server);
    singleData(server,serverBlock);
    // server.printErrorPages();
    return server;
}

void ParseConfig::ServerInHttp(std::string &httpBlock, Http &httpConfig){
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

void ParseConfig::parseHttpBlock(std::string &httpBlock) {
    std::string line = "";
    int st;
	int status = 0;
    std::string path = "";
	std::map<int , std::string> errs;
    std::map<std::string, std::string> values;
    ServerInHttp(httpBlock,Configurations::http); //! Parse "server" blocks
    std::istringstream BlockStream(httpBlock);
    values = extractKeyValues(httpBlock, errs);
    std::map<std::string, std::string>::iterator it = values.begin();
    for (; it != values.end(); ++it) {
        if (it->first == "include")
            includeMimeTypes(it->second); //? open mime.types and set this values in map inside the http class
        else if(it->first == "default_type")
            Configurations::http.setDefault_type(it->second);// set the default path ;
        else if(it->first == "client_max_body_size")
        Configurations::http.setMax_body_size(it->second);
        else if (it->first == "error_page"){
			if (errs.size())
				Configurations::http.setError_pages(errs);
        }
        else 
            throw std::runtime_error("http wrong key !");
    }
    // Configurations::http.printErrorPages();

}

void ParseConfig::parsingValues() {
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

void ParseConfig::pacingConfigFile(char *_file){
	std::string file = _file;
    PreProcessingFile(file);
    parsingValues();   
}