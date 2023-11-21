#include "Webserv.hpp"

int creatServer(){
	struct sockaddr_in host_addr;
	int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(8080);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd == -1)
		throw std::runtime_error("Error creating socket");
	std::cout << "socket created successfully" << std::endl;

	// Create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

	const int enable = 1;
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    	throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    //-> Bind the socket to the address
    if (bind(socketFd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
		throw std::runtime_error("Error bind socket");
	std::cout << "socket successfully bound to address" << std::endl;

	//-> Listen for incoming connections
    if (listen(socketFd, SOMAXCONN) != 0)
			throw std::runtime_error("Error listening");
	std::cout << "server listening for connections" << std::endl;
	return socketFd;
}

void creatSocket(){
	char buffer[1024];
	char resp[] =	        "HTTP/1.0 200 OK\r\n"
        "Server: webserver-c\r\n"
        "Content-type: text/html\r\n\r\n"
        "<html>"
        "<head><title>My Webpage</title></head>"
        "<body>"
        "<h1>Hello, Web!</h1>"
        "<p>This is a simple webpage.</p>"
        "<img src=\"image.jpg\" alt=\"An example image\">"
        "</body>"
        "</html>\r\n";

	int socketFd = creatServer();

    
	struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);


	struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

	//-> 
		fd_set currentSocket , readySocket;
		FD_ZERO(&currentSocket);
		FD_SET(socketFd, &currentSocket);
	//->

	while (true) {
        // Accept incoming connections
		readySocket = currentSocket;
		if(select(FD_SETSIZE, &readySocket, NULL,NULL,NULL) < 0)
			throw std::runtime_error("Error listening");
        std::cout << "Waiting in Accept" << std::endl;
		int newSockFd = accept(socketFd, (struct sockaddr *)&host_addr,
                               (socklen_t *)&host_addrlen);
        if (newSockFd < 0){
            perror("webServer (accept)");
            continue;
        }
		std::cout << "connection accepted" << std::endl;
		int sockName = getsockname(newSockFd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockName < 0) {
            perror("webServer (getsockname)");
            continue;
        }
		std::cout << "getsockname  ok" << std::endl;
		// Read from the socket
        int valRead = read(newSockFd, buffer, 1024);
        if (valRead < 0){
			perror("webServer (read)");
			continue;
		}
		std::cout << "reding done" << std::endl;


        // Read the request
        char method[1024], uri[1024], version[1024];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);
		
        // Write to the socket
        int valWrite = write(newSockFd, resp, strlen(resp));
        if (valWrite < 0) {
           perror("webServer (write)");
            continue;
        }
		std::cout << "writing done done" << std::endl;
        close(newSockFd);
    }

}

int main(){
    try
    {
        pacingConfigFile();
		creatSocket();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
} 