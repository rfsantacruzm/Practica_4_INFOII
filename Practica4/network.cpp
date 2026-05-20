#include "network.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>

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

void Network::loadTopologyFromFile(const string& filename) {
    ifstream file(filename);
    string line;
    while(getline(file, line)) {
        istringstream iss(line);
        string router1Name, router2Name;
        int cost;
        if(!(iss >> router1Name >> router2Name >> cost)) {
            break;
        }
        Router* router1 = findOrCreateRouter(router1Name);
        Router* router2 = findOrCreateRouter(router2Name);
        router1->updateRoutingTable(router2, cost);
        router2->updateRoutingTable(router1, cost);
    }
    file.close();
}

Router* Network::findOrCreateRouter(const string& name) {
    for(auto& router : routers) {
        if(router->name == name) {
            return router;
        }
    }
    Router* newRouter = new Router(name);
    routers.push_back(newRouter);
    return newRouter;
}

void Network::generateRandomNetwork(int numRouters, int maxCost) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(1, maxCost);
    uniform_int_distribution<> distr_bool(0, 1);
    for(int i = 0; i < numRouters; i++) {
        Router* newRouter = new Router("Router" + to_string(i));
        routers.push_back(newRouter);
    }
    for(int i = 0; i < numRouters; i++) {
        // Aseguramos que al menos un enrutador esté conectado
        int j = distr(gen) % numRouters;
        if(j != i) {
            int cost = distr(gen);
            routers[i]->updateRoutingTable(routers[j], cost);
            routers[j]->updateRoutingTable(routers[i], cost);
        }
        // Para el resto de enrutadores, decidimos aleatoriamente si conectarlos o no
        for(int j = i + 1; j < numRouters; j++) {
            if(distr_bool(gen)) {
                int cost = distr(gen);
                routers[i]->updateRoutingTable(routers[j], cost);
                routers[j]->updateRoutingTable(routers[i], cost);
            }
        }
    }
}

void Network::printAllRoutingTables() {
    for(auto& router : routers) {
        router->printRoutingTable();
    }
}
