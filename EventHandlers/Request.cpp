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

// called URI DECODING
void urlDecoding(std::string &Path)
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
	if (int pos = Path.find("/") != std::string::npos)
		Path = Path.substr(pos);
	urlDecoding(Path);

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

void Request::ParseHeaders(Response &response)
{
	DEBUGOUT(1, "Request ParseHeaders been called again.");
	StorHeaderData();
	std::string expectedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	response.setStatusCode(200);
	if (!Headers["Transfer-Encoding"].empty() && Headers["Transfer-Encoding"] != "chunked\r")
	{
		std::cout << "/trans : /" << Headers["Transfer-Encoding"] << "/" << std::endl;
		response.setStatusCode(501);
	}
	else if (Headers["Transfer-Encoding"].empty() && Headers["Content-Length"].empty())
		response.setStatusCode(400);
	else if (Path.find_last_of(expectedChars) == std::string::npos)
		response.setStatusCode(400);
	else if (Path.length() > 2048)
		response.setStatusCode(414);
	else
	{
		response.setFD(std::fopen("/nfs/homes/kmahdi/Desktop/Server/public/index.html", "r+")->_fileno);
	}
	// if (response.getStatusCode() != 200) //-> add 4014 max body size
	// 	ThrowErrorCode (response.getStatusCode());

	DEBUGOUT(1, COLORED("Path [" << this->Path << "]\n", Yellow));
	DEBUGOUT(1, COLORED("Path [" << this->Method << "]\n", Yellow));

	std::map<std::string, std::string>::iterator it = Headers.begin();
	for (; it != Headers.end(); ++it)
		DEBUGOUT(0, COLORED("Path [" << it->first << "]"
									 << " = {" << it->second << "}",
							Yellow));

	REQUEST_STATE = REQUEST_HANDLER_STAGE;
}

bool Request::RequestParser(std::string Data, Response &response)
{
	this->Buffer += Data;
	DEBUGOUT(1, "Request Parser been called again.");
	DEBUGOUT(1, COLORED("\n" << this->Buffer << "\n", Yellow));
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
	{
		if (this->Buffer.find("\r\n\r\n") != std::string::npos)
			ParseHeaders(response);

		// fall through
	}
	case REQUEST_HANDLER_STAGE:

		break;
	default:
		break;
	}
	return (false);
}
