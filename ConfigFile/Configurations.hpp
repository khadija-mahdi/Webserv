#include "../Webserv.hpp"

class Configurations {
public:
    class events {
        int worker_connections;
    public:
        void setWorkerConnections(int);
        int getWorkerConnections() const;
    };

    class Http {


    };
};