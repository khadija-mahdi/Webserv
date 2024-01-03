#include "Include/RequestHandler.hpp"

RequestHandler::RequestHandler(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	headerData = new DataPool();
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
}
RequestHandler::RequestHandler(DataPool *data) : headerData(data), requestParser(headerData)
{
	printf("%p\n", headerData);
	fflush(stdout);
	// exit(0);
	REQUEST_STATE = HEADERS_STAGE;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
}
RequestHandler::~RequestHandler()
{
}

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
	if (!GetHeadersValue(headerData->Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData->Headers, "Transfer-Encoding") != "chunked")
		throw HTTPError(501);
	if (headerData->Method == "POST" && GetHeadersValue(headerData->Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData->Headers, "Content-Length").empty())
		throw HTTPError(400);
	if (!IsValidSetOfCharacter(headerData->Path))
		throw HTTPError(400);
	if (headerData->Path.length() > 2048)
		throw HTTPError(414);
	// if (max body size with body length from post method) //-> add 4014 max body size
	// 	throw HTTPError(414);
	if (directoryStatus(headerData->Path) < 1 && !headerData->REDIRECTION_STAGE)
		throw HTTPError(404);
	return false;
}

bool RequestHandler::processRedirectionAndAllowance()
{

	int pos1 = 0;
	std::string newPath;

	if (headerData->locationIndex != -1)
	{
		std::vector<std::string> allowMethod = headerData->currentLocation.getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), headerData->Method) != allowMethod.end();
		if (!allow)
			throw HTTPError(405);
		if (headerData->REDIRECTION_STAGE)
		{
			headerData->response.Location = headerData->Path;
			headerData->response.StatusCode = headerData->currentLocation.getRedirection().statusCode;
			return true;
		}
	}
	return false;
}

Request *RequestHandler::handlerRequestMethods()
{
	if (headerData->Method == "GET")
		return request = new MethodGet(headerData);
	if (headerData->Method == "DELETE")
		return request = new MethodDelete(headerData);
	if (headerData->Method == "POST")
		return request = new PostRequest(*headerData);
	return request;
}

bool RequestHandler::HandlerRequest1(std::string Data)
{
	headerData->Buffer += Data;
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (headerData->Buffer.find("\r\n\r\n") != std::string::npos)
		{
			DEBUGMSGT(1, headerData->Buffer);
			requestParser.ParseRequest();
			if (parseHeaderErrors())
				return true;
			if (processRedirectionAndAllowance())
				return true;
			handlerRequestMethods();

			// DEBUGMSGT(1, COLORED("\n the current Server is  : " << headerData->currentServer.getListen() << "\n", Cyan));
			// DEBUGMSGT(1, COLORED("\n the current Location is  : " << headerData->currentLocation.getPath() << "\n", Cyan));
			// DEBUGMSGT(1, COLORED("\n the Path : " << headerData->Path << ", dir status : " << directoryStatus(headerData->Path) << "\n", Green));
			// DEBUGMSGT(1, COLORED("\n REDIRECTION_STAGE " << headerData->REDIRECTION_STAGE << "\n", Green));
		}
		REQUEST_STATE = REQUEST_HANDLER_STAGE;
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
	return (*this->headerData);
}

Request *RequestHandler::GetRequestHandler()
{
	return this->request;
}