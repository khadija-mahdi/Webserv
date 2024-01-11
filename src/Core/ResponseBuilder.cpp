#include "Include/ResponseBuilder.hpp"

#define SHOWBUFFER 0

void ResponseBuilder::InitStatusCode()
{
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

ResponseBuilder::ResponseBuilder(DataPool dataPool)
{
	this->dataPool = dataPool;
	InitStatusCode();
	if (dataPool.response.fileFd != ERROR)
		FillHeaders(dataPool.response.StatusCode);
}

ResponseBuilder::ResponseBuilder(DataPool dataPool, HeadersType &ResponseHeaders)
{
	this->dataPool = dataPool;
	this->ResponseHeaders = ResponseHeaders;
	InitStatusCode();
	if (dataPool.response.fileFd != -2)
		FillHeaders(dataPool.response.StatusCode);
}

std::string ResponseBuilder::GetDefaultErrorPagePath()
{
	std::string FileName = "/tmp/" + Lstring::RandomStr(16);
	std::string title = SSTR(dataPool.response.StatusCode) + " " + StatusCodes[dataPool.response.StatusCode];
	std::ofstream ErrorPage(FileName.c_str());
	ErrorPage << "<!DOCTYPE html>"
			  << "<html>"
			  << "<head>"
			  << "<title>" << title << "</title>"
			  << "</head>"
			  << "<body>"
			  << "<center>"
			  << " <h1>" << title << "</h1>"
			  << "<hr>"
			  << "<p>Default WebServ Page</p>"
			  << " </center>"
			  << "</body>"
			  << "</html>";
	ErrorPage.close();
	return (FileName);
}

void ResponseBuilder::CreateStatusFile()
{
	std::string FileName;
	if (GetErrorPage())
		return;
	if (FileName.empty())
		FileName = GetDefaultErrorPagePath();
	this->dataPool.response.fileFd = IO::OpenFile(FileName.c_str(), "r+");
	this->dataPool.response.contentType = "text/html";
}

void ResponseBuilder::FillHeaders(int StatusCode)
{
	std::string Status;
	size_t index;
	Status = ResponseHeaders["Status"].empty()
				 ? (SSTR(StatusCode) + " " + StatusCodes[StatusCode])
				 : ResponseHeaders["Status"];
	Buffer += ("HTTP/1.1 " + Status + "\r\n");
	if (this->dataPool.response.StatusCode != 405 &&
		dataPool.response.fileFd == NOBODY &&
		dataPool.response.Location.empty())
		CreateStatusFile();
	for (HeadersIterator it = ResponseHeaders.begin(); it != ResponseHeaders.end(); it++)
	{
		if (it->first == "Status")
			continue;
		if (it->first == "Content-type" && (index = it->second.find(";")) != std::string::npos)
		{
			Buffer += (it->first + ": " + it->second.substr(0, index) + "\r\n");
			continue;
		}
		Buffer += (it->first + ": " + it->second + "\r\n");
	}
	if (ResponseHeaders.find("Content-type") == ResponseHeaders.end())
		Buffer += ("Content-Type: " + this->dataPool.response.contentType + "\r\n");
	Buffer += "Connection: closed\r\n";
	if (this->dataPool.response.StatusCode != 405)
		Buffer += "Transfer-Encoding: chunked\r\n";
	Buffer += this->dataPool.response.Location.empty()
				  ? "\r\n"
				  : ("Location: " + this->dataPool.response.Location + "\r\n\r\n");
}

bool ResponseBuilder::checkErrorPage(std::map<int, std::string> error_pages)
{

	std::map<int, std::string>::iterator it = error_pages.begin();
	for (; it != error_pages.end(); ++it)
	{
		if (it->first == dataPool.response.StatusCode)
		{
			dataPool.response.fileFd = open(it->second.c_str(), O_RDONLY, 0664);
			return true;
		}
	}
	return false;
}

bool ResponseBuilder::GetErrorPage()
{
	if (!checkErrorPage(dataPool.currentLocation.getError_pages()) &&
		!checkErrorPage(dataPool.currentServer.getError_pages()) &&
		!checkErrorPage(Configurations::http.getError_pages()))
		return false;
	return true;
}

std::map<int, std::string> &ResponseBuilder::GetStatusCodes()
{
	return this->StatusCodes;
}

int ResponseBuilder::FlushBuffer(int SocketFd)
{

	if (this->Buffer.empty())
		return (0);
	DEBUGMSGT(1, COLORED(this->Buffer.c_str(), Magenta));
	int i = 0;
	if ((i = write(SocketFd, this->Buffer.c_str(), this->Buffer.size())) < 0 || this->Buffer == "0\r\n\r\n")
		return (0);
	this->Buffer.clear();
	this->FillBuffer();
	return (1);
}

void ResponseBuilder::FillBuffer()
{
	if (this->dataPool.response.fileFd < 0)
	{
		// this->Buffer.append(SSTR(0) + "\r\n\r\n");
		this->Buffer.clear();
		return;
	}
	char buffer[1025];
	memset(buffer, 0, sizeof(buffer));
	std::stringstream ss;
	int BytesCount;
	BytesCount = read(dataPool.response.fileFd, buffer, KB);
	if (BytesCount < 0)
		throw std::runtime_error("Error Reading ResourceFile ");
	else if (BytesCount == 0)
		this->Buffer.append(SSTR(0) + "\r\n\r\n");
	else
	{
		ss << std::hex << BytesCount << "\r\n";
		ss.write(buffer, BytesCount);
		ss << "\r\n";
		this->Buffer = ss.str();
	}
	if (BytesCount < 1)
		close(dataPool.response.fileFd);
}

ResponseBuilder::~ResponseBuilder()
{
}