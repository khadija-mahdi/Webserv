#include "Request.hpp"

Request::Request(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
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
}

Request::~Request() {}

std::string Request::getBuffer() const { return Buffer; }
std::string Request::getMethod() const { return Method; }
std::string Request::getPath() const { return Path; }
std::map<std::string, std::string> Request::getHeaders() const { return Headers; }

void Request::setHeaders(std::string &_value, std::string _key) { Headers[_key] = _value; }
void Request::setBuffer(std::string const &buff) { this->Buffer = buff; }
void Request::setMethod(std::string const &meth) { this->Method = meth; }
void Request::setPath(std::string const &path) { this->Path = path; }

void WhiteSpaceFound(std::string &Path)
{
	std::ostringstream decoded_ss;
	int Flag = 0;

	for (std::size_t i = 0; i < Path.length(); ++i)
	{
		char currentCh = Path[i];
		if (currentCh == '%')
		{
			if (i + 2 < Path.length())
			{
				std::string hexSp = Path.substr(i + 1, 2);
				if (isDigitStr(hexSp))
				{
					Flag++;
					std::istringstream hexStream(hexSp);
					int hexValue;
					hexStream >> std::hex >> hexValue;
					if (!hexStream.fail())
					{
						decoded_ss << static_cast<char>(hexValue);
						i += 2;
					}
					else
					{
						decoded_ss << currentCh;
					}
				}
			}
			else if (Flag > 0)
			{
				decoded_ss << currentCh; // Append '%' as is
			}
		}
		else
		{
			decoded_ss << currentCh;
		}
	}
	Path = decoded_ss.str();
}

void Request::StorHeaderData()
{

	std::istringstream BufferStream(Buffer);
	std::string requestLine;
	std::getline(BufferStream, requestLine);

	std::istringstream requestLineStream(requestLine);
	requestLineStream >> Method >> Path;
	WhiteSpaceFound(Path);

	std::string header;
	while (std::getline(BufferStream, header) && !header.empty())
	{
		size_t colonPos = header.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = header.substr(0, colonPos);
			std::string value = header.substr(colonPos + 2);
			Headers[key] = value;
		}
	}
}

std::string DefaultErrorPage(std::string Message, int code)
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

void Request::sendResponseError(int &resp)
{
	std::string testPage = DefaultErrorPage(StatusCodes[501], 501);
	if (!testPage.empty())
		resp = std::fopen(testPage.c_str(), "r+")->_fileno;
	// if (!Headers["Transfer-Encoding header"].empty() && Headers["Transfer-Encoding header"] != "chunked")
	// 	resp = send_response(501, DefaultErrorPage(StatusCodes[501], 501));
	// else if (Headers["Transfer-Encoding"].empty() && Headers["Content-Length"].empty() && Method == "Post")
	// 	resp = send_response(400, DefaultErrorPage(StatusCodes[400], 400));
	// else
	// 	resp = send_response(200, DefaultErrorPage(StatusCodes[200], 200));
}

void Request::ParseHeaders(int &resp)
{
	if (this->Buffer.find("\r\n\r\n") != std::string::npos)
	{
		StorHeaderData();
		DEBUGOUT(0, COLORED("ParcHeader case is on ", Green));
		sendResponseError(resp); // headerValid
		// TODO : add this later
		// throw HttpError(404);
		REQUEST_STATE = REQUEST_HANDLER_STAGE;
	}
}

bool Request::RequestParser(std::string Data,int &resp)
{
	this->Buffer += Data;
	DEBUGOUT(0, COLORED("\n" << this->Buffer << "\n", Magenta));
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		ParseHeaders(resp);
	// fall through
	case REQUEST_HANDLER_STAGE:
		break;
	default:
		break;
	}
	return (false);
}
