#include "RequestHandler.hpp"

RequestHandler::RequestHandler(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType = 1;
	request = new RequestDefault(headerData);
}
RequestHandler::RequestHandler(HeaderData *data) :headerData(data) , requestParser(headerData)
{
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType = 1;
	request = new RequestDefault(headerData);

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
		return requestParser.checkInHttp(501, 1);
	if (headerData->Method == "POST" && GetHeadersValue(headerData->Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData->Headers, "Content-Length").empty())
		return requestParser.checkInHttp(400, 1);
	if (!IsValidSetOfCharacter(headerData->Path))
		return requestParser.checkInHttp(400, 1);
	if (headerData->Path.length() > 2048)
		return requestParser.checkInHttp(414, 1);
	// if (max body size with body length from post method) //-> add 4014 max body size
	// 	return requestParser.checkInHttp(414, 1);
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
			return requestParser.checkInHttp(405, 1);
		std::cout << "hi \n\n";
		if (headerData->REDIRECTION_STAGE)
		{
			headerData->response.Location = headerData->Path;
			headerData->response.StatusCode = headerData->currentLocation.getRedirection().statusCode;
			headerData->response.ResponseType = 1;
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
		return request = new MethodGet(headerData);
	return request;
}

bool RequestHandler::HandlerRequest(std::string Data)
{
	headerData->Buffer += Data;
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (headerData->Buffer.find("\r\n\r\n") != std::string::npos)
		{
			DEBUGOUT(1, headerData->Buffer);
			requestParser.ParseRequest();
			handlerRequestMethods();
			DEBUGOUT(1, COLORED("\n the current Server is  : " << headerData->currentServer.getListen() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the current Location is  : " << headerData->currentLocation.getPath() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the Path : " << headerData->Path << ", dir status : " << directoryStatus(headerData->Path) << "\n", Green));
		}
	case REQUEST_HANDLER_STAGE:
		if (parseHeaderErrors())
			return true;
		if (processRedirectionAndAllowance())
			return true;
		if (request != NULL)
			return request->processRequest();
		delete request;
		break;
	default:
		break;
	}
	return (false);
}
