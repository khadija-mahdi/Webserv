#include "Request.hpp"

Request::Request(){
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
}

Request::Request(HeaderData	*Data) :headerData(Data), getMethod(headerData){
	REQUEST_STATE = HEADERS_STAGE;
	serverIndex = -1;
	headerData->locationIndex = -1;
	headerData->REDIRECTION_STAGE = false;
}

Request::~Request() {}


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

bool Request::parseHeaderErrors(){
	if (!GetHeadersValue(headerData->Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData->Headers, "Transfer-Encoding") != "chunked")
		return getMethod.checkInHttp(501, 1);
	if (headerData->Method == "POST" && GetHeadersValue(headerData->Headers, "Transfer-Encoding").empty() && GetHeadersValue(headerData->Headers, "Content-Length").empty())
		return getMethod.checkInHttp(400, 1);
	if (!IsValidSetOfCharacter(headerData->Path))
		return getMethod.checkInHttp(400, 1);
	if (headerData->Path.length() > 2048)
		return getMethod.checkInHttp(414, 1);
	// if (max body size with body length from post method) //-> add 4014 max body size
	// 	return getMethod.checkInHttp(414, 1);
	return false;
}


bool Request::processRedirectionAndAllowance(){

	int pos1 = 0; 
	std::string newPath;

	if (headerData->locationIndex != -1){
		std::vector<std::string> allowMethod = headerData->currentLocation.getAllow();
		int allow = std::find(allowMethod.begin(), allowMethod.end(), headerData->Method) != allowMethod.end();
		if(!allow)
			return getMethod.checkInHttp(405, 1);
		if (headerData->REDIRECTION_STAGE){
			headerData->response.Location = headerData->Path; 
			headerData->response.StatusCode = headerData->currentLocation.getRedirection().statusCode;
			headerData->response.ResponseType = 0;
			return true;
		}
	}
	return false;
}

bool Request::methodParser(){
	DEBUGOUT(1, COLORED(" check for Method : [" << headerData->Method << "]" <<  ", Path is : " << headerData->Path  << "\n", Red));
	if (headerData->Method == "GET")
		return getMethod.GetMethodHandler();
	if (headerData->Method == "POST")
		; //? post methos need implement 
	if (headerData->Method == "DELETE")
		; //? Delete method need implement 
	return false;
}

void Request::processRequest()
{
	headerData->response.StatusCode = 200;
	headerData->response.ResponseType =  1;
	if(parseHeaderErrors())
		return;
	if (processRedirectionAndAllowance())
		return;
	if (!headerData->REDIRECTION_STAGE){
		if (directoryStatus(headerData->Path) >  0){
			if (methodParser())
				return;
		}
		getMethod.checkInHttp(404, 1);
	}
	REQUEST_STATE = REQUEST_HANDLER_STAGE;
}


bool Request::RequestHandler(std::string Data)
{
	headerData->Buffer += Data;
	switch (REQUEST_STATE)
	{
	case HEADERS_STAGE:
		if (headerData->Buffer.find("\r\n\r\n") != std::string::npos){
			DEBUGOUT(1, headerData->Buffer);
			requestParser.ParseRequest(*headerData);
			DEBUGOUT(1, COLORED("\n the current Server is  : " << headerData->currentServer.getListen() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the current Location is  : " << headerData->currentLocation.getPath() << "\n", Cyan));
			DEBUGOUT(1, COLORED("\n the Path that working on it is in pros-->   : " << headerData->Path << "\n", Red));
		}
	case REQUEST_HANDLER_STAGE:
		processRequest();
		break;
	default:
		break;
	}
	return (false);
}
