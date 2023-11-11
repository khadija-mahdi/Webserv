// #include "Webserv.hpp"

// #include <iostream>
// #include <sstream>
// #include <stdexcept>

// namespace Configurations {
//     // Define your data structures for holding parsed values
//     struct Events {
//         int worker_connections;
//     };

//     struct Server {
//         int listen;
//         std::string host;
//         std::string server_names;
//         // Add more fields as needed for other server configurations
//     };

//     struct Http {
//         std::string max_body_size;
//         std::vector<Server> servers; // To store multiple server configurations
//         // Add more fields as needed for other HTTP configurations
//     };
// }

// // Function to split a key-value pair from a block
// void splitKeyValue(std::string &block, std::string &key, std::string &value, std::string word, int flag) {
//     std::string keyValue = "";
//     size_t pos = block.find(word);
//     if (pos == std::string::npos && flag == 1)
//         throw std::runtime_error("events block error");

//     size_t end = block.find("}");
//     for (int i = pos; i < end; i++)
//         keyValue += block[i];

//     int vstart = 0;
//     int vend = keyValue.find("\t");
//     value = keyValue.substr(vstart, vend);

//     for (; keyValue[vend] && keyValue[vend] == '\t'; vend++)
//         ;

//     int kEnd = keyValue.find(";");
//     key = keyValue.substr(vend, kEnd);
// }

// // Function to parse the "server" block
// Configurations::Server parseServerBlock(std::string &serverBlock) {
//     Configurations::Server server;
//     std::string value = "";
//     std::string key = "";

//     // Add parsing logic for server block configurations
//     splitKeyValue(serverBlock, key, value, "listen", 0);
//     server.listen = std::stoi(key);

//     splitKeyValue(serverBlock, key, value, "host", 0);
//     server.host = key;

//     splitKeyValue(serverBlock, key, value, "server_names", 0);
//     server.server_names = key;

//     // Print or store the parsed values
//     std::cout << "Server Listen: " << server.listen << std::endl;
//     std::cout << "Server Host: " << server.host << std::endl;
//     std::cout << "Server Names: " << server.server_names << std::endl;

//     return server;
// }

// // Function to parse the "http" block
// void httpBlock(std::string &http) {
//     Configurations::Http httpCl;
//     std::string value = "";
//     std::string key = "";
//     splitKeyValue(http, key, value, "client_max_body_size", 0);
//     httpCl.max_body_size = key;

//     // Print or store the parsed values
//     std::cout << "Max Body Size: " << httpCl.max_body_size << std::endl;

//     // Parse server blocks within the http block
//     size_t pos = http.find("server");
//     while (pos != std::string::npos) {
//         size_t end = http.find("}", pos);
//         std::string serverBlock = http.substr(pos, end - pos + 1);
//         Configurations::Server server = parseServerBlock(serverBlock);
//         httpCl.servers.push_back(server);
//         pos = http.find("server", end);
//     }

//     // Print or store the parsed server configurations
//     for (const auto &server : httpCl.servers) {
//         std::cout << "Server Listen: " << server.listen << std::endl;
//         std::cout << "Server Host: " << server.host << std::endl;
//         std::cout << "Server Names: " << server.server_names << std::endl;
//     }

//     if (linesLength(http) != 2)
//         std::cout << linesLength(http) << " = " << httpCl.max_body_size << std::endl;
//     else
//         throw std::runtime_error("http Block error!!");
// }

// // Function to parse the entire configuration
// void parsingValues(std::string &lines) {
//     std::istringstream BlockStream(lines);
//     std::string line;

//     while (std::getline(BlockStream, line)) {
//         if (line == "events {") {
//             std::string events = Blocks(lines, "events");
//             eventsBlock(events);
//         } else if (line == "http {") {
//             std::string http = Blocks(lines, "http");
//             httpBlock(http);
//         }
//     }
// }

// int main() {
//     // Your configuration block
//     std::string configurationBlock = R"(
// events {
//     worker_connections        1024;
// }

// http {

//     #includ=e                 mime.types;
//     default_type            application/octet-stream;
//     client_max_body_size    10M;

//     server {
//         listen          80;
//         host            localhost;
//         server_names    localhost;
//         location / {
//             root    html;
//             index   index.html index.htm;
//         } 
//         error_page      404         /404.html;
//         error_page      500         /500.html;
       
//     }
//     server {
//         listen          8000;
//         host            127.0.0.1;
//         root            html;
//         server_names    somename alias  another.alias;
//         error_page      500    /500.html;
        
//         location kk {
//         root   html;
//         index  index.html index.htm;
//         return     200;
//         autoindex on;
//         allow      all;
//         upload      off;
//         upload_stor /some/path;
//         error_page      404   /404.html;
//        }
//     }
// })";

//     // Parse the configuration
//     parsingValues(configurationBlock);

//     return 0;
// }




// int main() {
//     // Your configuration block
//     std::string configurationBlock = R"(
// events {
//     worker_connections        1024;
// }

// http {

//     #includ=e                 mime.types;
//     default_type            application/octet-stream;
//     client_max_body_size    10M;

//     server {
//         listen          80;
//         host            localhost;
//         server_names    localhost;
//         location / {
//             root    html;
//             index   index.html index.htm;
//         } 
//         error_page      404         /404.html;
//         error_page      500         /500.html;
       
//     }
//     server {
//         listen          8000;
//         host            127.0.0.1;
//         root            html;
//         server_names    somename alias  another.alias;
//         error_page      500    /500.html;
        
//         location kk {
//         root   html;
//         index  index.html index.htm;
//         return     200;
//         autoindex on;
//         allow      all;
//         upload      off;
//         upload_stor /some/path;
//         error_page      404   /404.html;
//        }
//     }
// })";

//     // Parse the configuration
//     Configurations::Events eventsConfig;
//     Configurations::Http httpConfig;
//     parsingValues(configurationBlock, eventsConfig, httpConfig);

//     return 0;
// }
