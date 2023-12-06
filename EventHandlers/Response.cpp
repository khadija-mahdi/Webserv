#include "Response.hpp"

Response::Response(/* args */)
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
	StatusCodes[400] = "Bad RequestParser";
	StatusCodes[401] = "Unauthorized";
	StatusCodes[402] = "Payment Required";
	StatusCodes[403] = "Forbidden";
	StatusCodes[404] = "Not Found";
	StatusCodes[405] = "Method Not Allowed";
	StatusCodes[406] = "Not Acceptable";
	StatusCodes[407] = "Proxy Authentication Required";
	StatusCodes[408] = "RequestParser Timeout";
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
}

std::string Response::DefaultErrorPage(std::string Message, int code)
{
    std::string filePath = "/tmp/default_error_page.html";

    // Use std::ofstream constructor with std::ios::trunc to create or truncate the file
    std::ofstream defaultError(filePath.c_str(), std::ios::trunc);
	if (!defaultError.is_open())
	{
		std::cerr << "Error opening file: " << filePath << std::endl;
		return "";
	}
	defaultError << "<!DOCTYPE html>\n"
				 << "<html lang=\"en\">\n"
				 << "<head>\n"
				 << "    <meta charset=\"UTF-8\">\n"
				 << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
				 << "    <title>  " + SSRT(code) + " " + Message + "</title>\n"
				 << "    <style>\n"
				 << "        body {\n"
				 << "            font-family: 'Arial', sans-serif;\n"
				 << "            background-color: #f4f4f4;\n"
				 << "            color: #333;\n"
				 << "            text-align: center;\n"
				 << "            padding: 50px;\n"
				 << "            margin: 0;\n"
				 << "        }\n"
				 << "        h1 {\n"
				 << "            font-size: 3em;\n"
				 << "            color: #555;\n"
				 << "        }\n"
				 << "        p {\n"
				 << "            font-size: 1.2em;\n"
				 << "            color: #777;\n"
				 << "        }\n"
				 << "    </style>\n"
				 << "</head>\n"
				 << "<body>\n"
				 << "    <h1>  " + SSRT(code) + " " + Message + "  </h1>\n"
				 << "    <p> Oops! Something went wrong.</p>\n"
				 << "</body>\n"
				 << "</html>\n";
	defaultError.close();
	return filePath;
}

void Response::setResponseBuffer(std::string const&res){responseBuffer = res;}

std::string Response::getResponseBuffer()const {return responseBuffer;}

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
	DEBUGOUT(1, COLORED("not even exist : "  << path , Yellow));
	return -1; // is not exist
}


void Response::getContentType(Request &request){
	std::string Path = request.getPath();

	std::cout << "Path : " << Path << std::endl;
	int pos = Path.find(".");
	if (isDirectory(Path) == 1){

		ContentType = "dir/html";
	}
	else if (pos != std::string::npos)
	{
		std::string  extention  = Path.substr(pos + 1, Path.length());
		std::map<std::string, std::string> Types = Configurations::http.getIncludes();
		DEBUGOUT(1, COLORED("content Type  : "  << Types[extention] , Yellow));
		if (Types.find(extention) != Types.end()) {
			ContentType = Types[extention];
		} else if ( Configurations::http.getDefault_type() != "")
			ContentType = Configurations::http.getDefault_type(); 
	}
	DEBUGOUT(1, COLORED("content Type  : "  << ContentType , Yellow));
}

std::string Response::httpheader(int const& statusCode, Request &request){
	getContentType(request);
	std::string header = "HTTP/1.1 " +  SSRT(statusCode) + " " + StatusCodes[statusCode] +"\r\n"
	"Content-Type:" + ContentType + "\r\n"
	"Transfer-Encoding: chunked\r\n\r\n";
	return header;
}


int Response::PrepareNextChunk()
{
	std::stringstream chunk;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int bytesRead = read(fd, buffer, sizeof(buffer));
	if (bytesRead < 0)
		throw std::runtime_error("Error reading from file descriptor");
	else if (bytesRead == 0) {
		chunk << "/0/r/n";
		return 0;
	}

	chunk << std::hex << bytesRead << "\r\n";
	chunk.write(buffer, bytesRead);
	chunk << "\r\n";
	responseBuffer = chunk.str();
	return bytesRead;
}

int Response::sendChunkResponse(int const& clientSocket, Request &request){
	int bytes = PrepareNextChunk();
	if (bytes == 0) {
		if (write(clientSocket,"0\r\n", strlen("0\r\n")) < 0) 
			throw std::runtime_error("Error writing to socket");
		close(fd);
		return (bytes);
	}
	responseBuffer = httpheader(statusCode, request) +  responseBuffer;
	DEBUGOUT(1, COLORED(responseBuffer, Magenta));
	if (write(clientSocket, responseBuffer.c_str(), responseBuffer.length()) < 0) 
		throw std::runtime_error("Error writing to socket");
	return 0;
}

void Response::sendResponse(int const& clientSocket, Request &request){
	std::string testPage;
	statusCode = 200;
	if (isDirectory(request.getPath()) == -1)
	{
		this->statusCode = 404;
		testPage = DefaultErrorPage(StatusCodes[statusCode], statusCode);
		if (testPage != "")
			fd = std::fopen(testPage.c_str(), "r+")->_fileno;
		sendChunkResponse(clientSocket, request);
		return;

	}
	else if (isDirectory(request.getPath()) == 2)
	{
		this->statusCode = 200;
		fd = std::fopen(request.getPath().c_str(), "r+")->_fileno;
		sendChunkResponse(clientSocket, request);
	}
	// close(fd);
}