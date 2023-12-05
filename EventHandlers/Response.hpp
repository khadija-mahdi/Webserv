#pragma once
#include "../Webserv.hpp"
#include "Request.hpp"

class Request;
class Response
{
private:
	std::string Resp;
	std::map<int, std::string> StatusCodes;
	std::stringstream chunk;
	Request request;

public:
	void setRespose(std::string const&res){Resp = res;}
	std::string getRespond()const {return Resp;}
// 	int PrepareNextChunk(int fd) {

// 	char buffer[1024];
// 	memset(buffer, 0, sizeof(buffer));

// 	int bytesRead = read(fd, buffer, sizeof(buffer));
// 	if (bytesRead < 0)
// 		throw std::runtime_error("Error reading from file descriptor");
// 	else if (bytesRead == 0) {
// 		chunk << "/0/r/n";
// 		return 0;
// 	}

// 	chunk << std::hex << bytesRead << "\r\n";
// 	chunk.write(buffer, bytesRead);
// 	chunk << "\r\n";

// 	return bytesRead;
// }

// // Get the current chunk data
// std::string GetCurrentChunkData() const {
// 	return chunk.str();
// }
// void clearChunk(){
// 	chunk.str("");
// }


std::string httpheader(){
	std::string header = "HTTP/1.1" +  200 + StatusCodes[200] +"\r\n"
	"Content-Type: Content-Type: text/plain\r\n"
	"Transfer-Encoding: chunked\r\n"
		"\r\n";
	return header;
}
	Response(/* args */);
	~Response(){};
};