// int main() {
//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         std::cerr << "Error creating socket" << std::endl;
//         return 1;
//     }
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY;
//     addr.sin_port = htons(80); // Port 80 for HTTP

//     if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
//         std::cerr << "Error binding socket" << std::endl;
//         close(serverSocket);
//         return 1;
//     }
//     if (listen(serverSocket, 10) == -1) {
//         std::cerr << "Error listening on socket" << std::endl;
//         close(serverSocket);
//         return 1;
//     }
//     while (true) {
//     struct sockaddr_in clientAddress;
//     socklen_t clientAddressLength = sizeof(clientAddress);
//     int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
//     if (clientSocket == -1) {
//         std::cerr << "Error accepting connection" << std::endl;
//         close(serverSocket);
//         return 1;
//     }
    
//     // At this point, you have a connection with a client, and you can perform operations with 'clientSocket'.
    
//     // Close the client socket when you're done with the client.
//     close(clientSocket);
// }
// }