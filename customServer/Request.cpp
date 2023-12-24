#include "Request.hpp"

Request::Request(){}

Request::Request(HeaderData *Data) : headerData(Data){}

Request::~Request() {}



RequestDefault::RequestDefault(){}

RequestDefault::RequestDefault(HeaderData *Data) : headerData(Data){}

RequestDefault::~RequestDefault() {}

bool RequestDefault::processRequest() {return false;}
