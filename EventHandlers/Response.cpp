#include "Response.hpp"

Response::Response()
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
	responseHeader  = httpheader(statusCode);
	responseBuffer = responseHeader;
	fd = -1;
	
}

std::string Response::DefaultErrorPage(int code)
{
    std::string ErrorPage = "<!DOCTYPE html>\n"
                            "<html lang=\"en\">\n"
                            "<head>\n"
                            "    <meta charset=\"UTF-8\">\n"
                            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                            "    <title> " + SSRT(statusCode) + " " + StatusCodes[statusCode] + "</title>\n"
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
                            "    <h1>  " + SSRT(statusCode) + " " + StatusCodes[statusCode] + "  </h1>\n"
                            "    <p> Oops! Something went wrong.</p>\n"
                            "</body>\n"
                            "</html>\n";
	ErrorPage = "HTTP/1.1 " + SSRT(statusCode) + " " + StatusCodes[statusCode] + "\r\n"
					+ "Content-Type: " + ContentType + "\r\n"
					+ "Content-Length: " + SSRT(ErrorPage.length()) + "\r\n"
					+ "\r\n"
					+ ErrorPage;
	return ErrorPage;
}

void Response::setResponseBuffer(std::string const&res){responseBuffer = res;}

std::string Response::getResponseBuffer()const {return responseBuffer;}

void Response::setResponseHeader(std::string const&res){responseHeader = res;}

std::string Response::getResponseHeader()const {return responseHeader;}

void Response::clearResponseBuffer(){responseBuffer = "";}

void Response::setStatusCode(int const &st){ statusCode = st;}

int Response::getStatusCode() const{return statusCode;}

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

void Response::getContentType() {
    int pos = Path.find(".");
    
    if (isDirectory(Path) == 1) {
        ContentType = "dir/html";
    } else if (pos != std::string::npos) {
        std::string extention = Path.substr(pos + 1, Path.length());
        std::map<std::string, std::string> Types = Configurations::http.getIncludes();
        
        
        if (Types.find(extention) != Types.end()) {
            ContentType = Types[extention];
        } else {
            ContentType = Configurations::http.getDefault_type();
        }
    }
    
}

std::string& Response::httpheader(int const& statusCode) {
    responseHeader = "HTTP/1.1 " + SSRT(statusCode) + " " + StatusCodes[statusCode] + "\r\n"
                   + "Content-Type: text/html /\r\n"
                   + "Transfer-Encoding: chunked\r\n\r\n";
    return responseHeader;
}

int Response::PrepareNextChunk() {
    std::stringstream chunk;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        throw std::runtime_error("Error reading from file descriptor");
    } else if (bytesRead == 0) {
        chunk << "0\r\n\r\n";
        responseBuffer = chunk.str();
        return 0;
    }
    chunk << std::hex << bytesRead << "\r\n";
    chunk.write(buffer, bytesRead);
    chunk << "\r\n";
    responseBuffer = chunk.str();
    return bytesRead;
}

int Response::sendChunkResponse(int const& clientSocket){
	int bytes = PrepareNextChunk();
	DEBUGOUT(1, COLORED(responseBuffer, Magenta));
	if (write(clientSocket, responseBuffer.c_str(), responseBuffer.length()) < 0) 
		throw std::runtime_error("Error writing to socket");
	return bytes;
}

int Response::sendResponse(int const &clientSocket)
{
	DEBUGOUT(1, COLORED("response : " << DefaultErrorPage(statusCode), Magenta));
	if (statusCode != 200)
	{
		if (write(clientSocket, DefaultErrorPage(statusCode).c_str(), DefaultErrorPage(statusCode).length()) < 0)
			throw std::runtime_error("Error writing to socket");
		return (0);
	}
	else
	{
		if (fd != -1)
			return sendChunkResponse(clientSocket);
	}
	return (-1);
}

// 	statusCode = 200;
// 	if (isDirectory(request.getPath()) == -1)
// 	{
// 		this->statusCode = 404;
// 		testPage = DefaultErrorPage(StatusCodes[statusCode], statusCode);
// 		if (testPage != "")
// 			fd = std::fopen(testPage.c_str(), "r+")->_fileno;
// 		sendChunkResponse(clientSocket);
// 		return;

// 	}
// 	else if (isDirectory(request.getPath()) == 2)
// 	{
// 		this->statusCode = 200;
// 		fd = std::fopen(request.getPath().c_str(), "r+")->_fileno;
// 		sendChunkResponse(clientSocket);
// 	}
// }

// void Response::sendResponse(int const& clientSocket, std::string &FilePath){
// 	std::string testPage;

// 	if (FilePath != "")
// 		fd = std::fopen(testPage.c_str(), "r+")->_fileno;
// 		sendChunkResponse(clientSocket);
// 	// close(fd);
// }