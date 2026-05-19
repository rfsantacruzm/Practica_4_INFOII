#include "network.h"
#include <algorithm>
#include <iostream>

using namespace std;

Network::~Network() {
    for(auto& router : routers) {
        delete router;
    }
}

void Network::addRouter(Router* router) {
    routers.push_back(router);
}

void Network::addConnection(Router* router1, Router* router2, int cost) {
    router1->updateRoutingTable(router2, cost);
    router2->updateRoutingTable(router1, cost);
}

void Network::removeRouter(Router* router) {
    routers.erase(remove(routers.begin(), routers.end(), router), routers.end());
    for(auto& r : routers) {
        r->routingTable.erase(router);
    }
    delete router;
}

void Network::printAllRoutingTables() {
    for(auto& router : routers) {
        router->printRoutingTable();
    }
}
