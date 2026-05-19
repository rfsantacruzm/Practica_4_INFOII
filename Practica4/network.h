#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <string>
#include "enrutador.h"

class Network {
public:
    std::vector<Router*> routers;

    ~Network();
    void addRouter(Router* router);
    void removeRouter(Router* router);
    void addConnection(Router* router1, Router* router2, int cost);
    void printAllRoutingTables();
};

#endif // NETWORK_H
