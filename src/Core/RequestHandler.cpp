#include "Include/RequestHandler.hpp"

RequestHandler::RequestHandler(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	this->dataPool.locationIndex = -1;
	this->dataPool.REDIRECTION_STAGE = false;
	this->dataPool.response.StatusCode = 200;
	this->dataPool.response.fileFd = -1;
}

RequestHandler::~RequestHandler(){}

bool IsValidSetOfCharacter(std::string str)
{
	std::string expectedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	return (str.find_last_of(expectedChars) != std::string::npos);
}

std::string GetHeadersValue(std::map<std::string, std::string> &headers, std::string Key)
{
	HeaderIterator it;
	it = headers.find(Key);
	if (it == headers.end())
		return ("");
	return (it->second);
}

bool RequestHandler::parseHeaderErrors()
{
	if (dataPool.locationIndex != -1)
	{
		std::vector<std::string> allowMethod = dataPool.currentLocation.getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), dataPool.Method) != allowMethod.end();
		if (!allow)
			throw HTTPError(405);
	}
	if (!GetHeadersValue(dataPool.Headers, "Transfer-Encoding").empty() && GetHeadersValue(dataPool.Headers, "Transfer-Encoding") != "chunked")
		throw HTTPError(501);
	if (dataPool.Method == "POST" && GetHeadersValue(dataPool.Headers, "Transfer-Encoding").empty() && GetHeadersValue(dataPool.Headers, "Content-Length").empty())
		throw HTTPError(400);
	if (!IsValidSetOfCharacter(dataPool.Path))
		throw HTTPError(400);
	if (dataPool.Path.length() > 2048)
		throw HTTPError(414);
	// if (max body size with body length from post method) //-> add 4014 max body size
	// 	throw HTTPError(414);
	if (directoryStatus(dataPool.Path) < 1 && !dataPool.REDIRECTION_STAGE)
		throw HTTPError(404);
	if (GetHeadersValue(dataPool.Headers, "Transfer-Encoding") == "chunked"){
        this->request->SetBodyController(Chunked, 0);
	}
    else if (!GetHeadersValue(dataPool.Headers, "Content-Length").empty()){
        this->request->SetBodyController(Lenght, atoll(GetHeadersValue(dataPool.Headers, "Content-Length").c_str()));
	}
	return false;
}


Request *RequestHandler::handlerRequestMethods()
{
	if (dataPool.Method == "GET")
		return request = new MethodGet(dataPool);
	if (dataPool.Method == "DELETE")
		return request = new MethodDelete(dataPool);
	if (dataPool.Method == "POST")
		return request = new PostRequest(dataPool);
	return request;
}

bool RequestHandler::HandlerRequest1(std::string Data)
{
	dataPool.Buffer += Data;
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (dataPool.Buffer.find("\r\n\r\n") != std::string::npos)
		{
			DEBUGMSGT(1, dataPool.Buffer);
			requestParser.ParseRequest(dataPool);
			handlerRequestMethods();
			if (parseHeaderErrors())
				return true;
			DEBUGMSGT(1, COLORED("\n the current Server is  : " << dataPool.currentServer.getListen() << "\n", Cyan));
			DEBUGMSGT(1, COLORED("\n the current Location is  : " << dataPool.currentLocation.getPath() << "\n", Cyan));
			DEBUGMSGT(1, COLORED("\n the Path : " << dataPool.Path << ", dir status : " << directoryStatus(dataPool.Path) << "\n", Green));
			DEBUGMSGT(1, COLORED("\n REDIRECTION_STAGE " << dataPool.REDIRECTION_STAGE << "\n", Green));
			REQUEST_STATE = REQUEST_HANDLER_STAGE;
		}
		// intentionally fall through
	case REQUEST_HANDLER_STAGE:
		if (request != NULL)
			return request->HandleRequest(Data);
		break;
	}
	return (false);
}
DataPool &RequestHandler::GetDataPool(void)
{
	return (this->dataPool);
}

Request *RequestHandler::GetRequestHandler()
{
	return this->request;
}