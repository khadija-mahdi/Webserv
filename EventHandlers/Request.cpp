#include "Request.hpp"

Request::Request(/* args */) { REQUEST_STATE = HEADERS_STAGE; }

Request::~Request() {}

std::string Request::getBuffer() const { return Buffer; }
std::string Request::getMethod() const { return Method; }
std::string Request::getPath() const { return Path; }
std::map<std::string, std::string> Request::getHeaders() const { return Headers; }

void Request::setHeaders(std::string &_value, std::string _key) { Headers[_key] = _value; }
void Request::setBuffer(std::string const &buff) { this->Buffer = buff; }
void Request::setMethod(std::string const &meth) { this->Method = meth; }
void Request::setPath(std::string const &path) { this->Path = path; }

bool Request::RequestParser(std::string Data)
{
	this->Buffer += Data;
	DEBUGOUT(1, COLORED("|\n"
							<< this->Buffer << "\n|",
						Magenta));
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		ParseHeaders();
	// fall through
	case REQUEST_HANDLER_STAGE:
		break;
	default:
		break;
	}
	return (false);
}


void WhiteSpaceFound(std::string &Path) {
    std::ostringstream decoded_ss;
	int Flag = 0;

    for (std::size_t i = 0; i < Path.length(); ++i) {
        char currentCh = Path[i];
        if (currentCh == '%') {
            if (i + 2 < Path.length()) {
                std::string hexSp = Path.substr(i + 1, 2);
				if (isDigitStr(hexSp)){
					Flag++;
					std::istringstream hexStream(hexSp);
					int hexValue;
					hexStream >> std::hex >> hexValue;
					if (!hexStream.fail()) {
						decoded_ss << static_cast<char>(hexValue);
						i += 2;
					} else {
						decoded_ss << currentCh;
					}
				}
            } else  if (Flag > 0 ){
                decoded_ss << currentCh; // Append '%' as is
            }
        } else {
            decoded_ss << currentCh;
        }
    }
    Path = decoded_ss.str();
}


void Request::StorHeaderData(){
	
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

void Request::ParseHeaders()
{
	if (this->Buffer.find("\r\n\r\n"))
	{
		StorHeaderData();
		std::cout << "Method : " << Method << std::endl;
		std::cout << "Path	 : " << Path << std::endl;
		std::map<std::string, std::string>::iterator it = Headers.begin();
		for (; it != Headers.end(); ++it)
			std::cout << "Headers : " << it->first << " => " << it->second << "\n";
		DEBUGOUT(1, COLORED("ParcHeader case is on ", Green));
		// TODO : add this later 
			// throw HttpError(404);
		REQUEST_STATE = REQUEST_HANDLER_STAGE;
	}
}