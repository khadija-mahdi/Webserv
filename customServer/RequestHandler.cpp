#include "RequestHandler.hpp"

RequestHandler::RequestHandler(/* args */)
{
	REQUEST_STATE = HEADERS_STAGE;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType = 1;
}
RequestHandler::RequestHandler(DataPool *data) :headerData(data) , requestParser(headerData)
{
	REQUEST_STATE = HEADERS_STAGE;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType = 1;

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
	if (directoryStatus(headerData->Path) < 1 && !headerData->REDIRECTION_STAGE)
			return  requestParser.checkInHttp(404, 1);
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
		std::cout << "hi red : " << headerData->REDIRECTION_STAGE <<  "\n\n";
		if (headerData->REDIRECTION_STAGE)
		{
			std::cout << "hi \n\n";
			headerData->response.Location = headerData->Path;
			headerData->response.StatusCode = headerData->currentLocation.getRedirection().statusCode;
			headerData->response.ResponseType = 0;
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

bool RequestHandler::HandlerRequest1(std::string Data)
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
			DEBUGOUT(1, COLORED("\n REDIRECTION_STAGE " << headerData->REDIRECTION_STAGE  << "\n", Green));
		}
	case REQUEST_HANDLER_STAGE:
		if (parseHeaderErrors())
			return true;
		if (processRedirectionAndAllowance())
			return true;
		std::cout << "index : " << headerData->locationIndex; 
		if (request != NULL)
			return request->HandleRequest(Data);
		delete request;
		break;
	default:
		break;
	}
	return (false);
}
