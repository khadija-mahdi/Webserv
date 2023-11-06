#include "Configurations.hpp"

int Configurations::events::getWorkerConnections() const{
    return worker_connections; 
}

void Configurations::events::setWorkerConnections(int worCon){
    worker_connections = worCon;
}