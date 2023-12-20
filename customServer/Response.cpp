#include "Response.hpp"

Response::Response(HeaderData *Data):headerData(Data)
{
	responseBuffer = "";
	StatusCodes[100] = "Continue";
	StatusCodes[101] = "Switching Protocols";
	StatusCodes[200] = "OK";
	StatusCodes[201] = "Created";
	StatusCodes[202] = "Accepted";
	StatusCodes[203] = "Non-Authoritative Information";
	StatusCodes[204] = "No Content"; 
	StatusCodes[205] = "Reset Content";
	StatusCodes[206] = "Partial Content";
	StatusCodes[300] = "Multiple Choices";
	StatusCodes[301] = "Moved Permanently";
	StatusCodes[302] = "Found";
	StatusCodes[303] = "See Other";
	StatusCodes[304] = "Not Modified";
	StatusCodes[305] = "Use Proxy";
	StatusCodes[307] = "Temporary Redirect";
	StatusCodes[400] = "Bad Request";
	StatusCodes[401] = "Unauthorized";
	StatusCodes[402] = "Payment Required";
	StatusCodes[403] = "Forbidden";
	StatusCodes[404] = "Not Found";
	StatusCodes[405] = "Method Not Allowed";
	StatusCodes[406] = "Not Acceptable";
	StatusCodes[407] = "Proxy Authentication Required";
	StatusCodes[408] = "Request Timeout";
	StatusCodes[409] = "Conflict";
	StatusCodes[410] = "Gone";
	StatusCodes[411] = "Length Required";
	StatusCodes[412] = "Precondition Failed";
	StatusCodes[413] = "Payload Too Large";
	StatusCodes[414] = "URI Too Long";
	StatusCodes[415] = "Unsupported Media Type";
	StatusCodes[416] = "Range Not Satisfiable";
	StatusCodes[417] = "Expectation Failed";
	StatusCodes[426] = "Upgrade Required";
	StatusCodes[500] = "Internal Server Error";
	StatusCodes[501] = "Not Implemented";
	StatusCodes[502] = "Bad Gateway";
	StatusCodes[503] = "Service Unavailable";
	StatusCodes[504] = "Gateway Timeout";
	StatusCodes[505] = "HTTP Version Not Supported";
	headerData->response.fileFd = -1;
	Headers_Stage = 1;
	headerData->response.ResponseType = -1;

	
}

std::string Response::DefaultErrorPage(int code)
{
    std::string ErrorPage = "<!DOCTYPE html>\n"
                            "<html lang=\"en\">\n"
                            "<head>\n"
                            "    <meta charset=\"UTF-8\">\n"
                            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                            "    <title> " + SSTR(headerData->response.StatusCode) + " " + StatusCodes[headerData->response.StatusCode] + "</title>\n"
                            "    <style>\n"
                            "        body {\n"
                            "            font-family: 'Arial', sans-serif;\n"
                            "            background-color: #f4f4f4;\n"
                            "            color: #333;\n"
                            "            text-align: center;\n"
                            "            padding: 50px;\n"
                            "            margin: 0;\n"
                            "        }\n"
                            "        h1 {\n"
                            "            font-size: 3em;\n"
                            "            color: #555;\n"
                            "        }\n"
                            "        p {\n"
                            "            font-size: 1.2em;\n"
                            "            color: #777;\n"
                            "        }\n"
                            "    </style>\n"
                            "</head>\n"
                            "<body>\n"
                            "    <h1>  " + SSTR(headerData->response.StatusCode) + " " + StatusCodes[headerData->response.StatusCode] + "  </h1>\n"
                            "    <p> Oops! Something went wrong.</p>\n"
                            "</body>\n"
                            "</html>\n";
	ErrorPage = "HTTP/1.1 " + SSTR(headerData->response.StatusCode) + " " + StatusCodes[headerData->response.StatusCode] + "\r\n"
					+ "Content-Type: " + ContentType + "\r\n"
					+ "Content-Length: " + SSTR(ErrorPage.length()) + "\r\n"
					+ "\r\n"
					+ ErrorPage;
	return ErrorPage;
}

void Response::setResponseBuffer(std::string const&res){responseBuffer = res;}

std::string Response::getResponseBuffer()const {return responseBuffer;}

void Response::setResponseHeader(std::string const&res){responseHeader = res;}

std::string Response::getResponseHeader()const {return responseHeader;}

void Response::clearResponseBuffer(){responseBuffer = "";}



int isDirectory(const std::string& path) {
    struct stat directoryInfo;
    if (stat(path.c_str(), &directoryInfo) == 0) {
        // Check if it is a directory
        if (S_ISDIR(directoryInfo.st_mode))
			return 1; // is a directory
		else if (S_ISREG(directoryInfo.st_mode))
			return 2; //is path exist;
		return 0; //is not
	}
	return -1; // is not exist
}

void Response::getContentType(std::string const &contentPath) {
    int pos = contentPath.find(".");
    if (isDirectory(contentPath) == 1) {
        ContentType = "dir/html";
    } else if (pos != std::string::npos) {
        std::string extention = contentPath.substr(pos + 1, contentPath.length());
        std::map<std::string, std::string> Types = Configurations::http.getIncludes();
        
        
        if (Types.find(extention) != Types.end()) {
            ContentType = Types[extention];
        } else 
            ContentType = Configurations::http.getDefault_type();
    }
    
}

std::string& Response::httpheader(int const& StatusCode) {
	getContentType(Path);
	responseHeader = "HTTP/1.1 " + SSTR(StatusCode) + " " + StatusCodes[StatusCode] + "\r\n"
				+ "Content-Type: " + ContentType + "\r\n"
				+ "Transfer-Encoding: chunked\r\n\r\n";

    return responseHeader;
}

int Response::PrepareNextChunk() {
    std::stringstream chunk;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = read(headerData->response.fileFd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        throw std::runtime_error("Error reading from file descriptor");
    } else if (bytesRead == 0) {
        chunk << "0\r\n\r\n";
        responseBuffer = chunk.str();
		close(headerData->response.fileFd);
        return 0;
    }
    chunk << std::hex << bytesRead << "\r\n";
    chunk.write(buffer, bytesRead);
    chunk << "\r\n";
    responseBuffer = chunk.str();
    return bytesRead;
}

int Response::sendChunkResponse(int const& clientSocket){
	int bytes = -1;
	if (Headers_Stage)
	{
		responseBuffer = httpheader(headerData->response.StatusCode);
		if (write(clientSocket, responseBuffer.c_str(), responseBuffer.length()) < 0) 
			throw std::runtime_error("Error writing to socket in header stage");
		Headers_Stage = false;
	}
	else
	{
		int bytes = PrepareNextChunk();
		if (write(clientSocket, responseBuffer.c_str(), responseBuffer.length()) < 0) 
			throw std::runtime_error("Error writing to socket");
			return bytes;
	}
	return bytes;
}

std::string Response::listenDirectory(const std::string &path) {
    std::stringstream chunk;
    std::string dirList;
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path.c_str());
    if (dp == NULL) {
        perror("opendir");
        return dirList;
    }
    chunk << "HTTP/1.1 " + SSTR(headerData->response.StatusCode) + " "  + StatusCodes[headerData->response.StatusCode] + "\r\n"
          << "Content-Type: text/html\r\n\r\n"
          << "<html><body><h1>Directory Listing</h1><ul>";

    while ((entry = readdir(dp))) {
        chunk << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
    }

    chunk << "</ul></body></html>";
    dirList = chunk.str();
    closedir(dp);

    return dirList;
}


bool Response::Write(std::string const &response, int const &clientSocket){
	DEBUGOUT(1, COLORED("response is : \n" << response, Magenta));
	if (!response.empty()) {
		if (write(clientSocket, response.c_str(), response.length()) < 0)
			throw std::runtime_error("Error writing to socket");
	}
	if (headerData->response.fileFd != -1)
		close (headerData->response.fileFd);
	return 0;
}

int Response::sendResponse(int const &clientSocket)
{
	switch (headerData->response.ResponseType) {
		case REDIRECTION:
		{
			std::string headerRe = "HTTP/1.1 " + SSTR(headerData->response.StatusCode) + " " + StatusCodes[headerData->response.StatusCode] + "\r\n"
								"Location: " + headerData->response.Location + " \r\n\r\n";
			return Write(headerRe, clientSocket);
		}
		case LISTEN_DIR:
			return Write(listenDirectory(headerData->response.Location), clientSocket);
		case CHUNKED:
			if (headerData->response.fileFd > 0)
				return sendChunkResponse(clientSocket);
			break;
		case DEFAULT_ERRORS:
			return Write(DefaultErrorPage(headerData->response.StatusCode), clientSocket);
		default:
			return -1;
			break;
	}
	return -1;
}