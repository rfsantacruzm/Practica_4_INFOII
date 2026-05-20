#include "network.h"
#include <algorithm>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <random>
#include <chrono>
#include <set>
#include <queue>

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
    for (auto& r : routers) {
        r->routingTable.erase(router);
    }
    delete router;
}

void Network::updateNetwork() {
    for(auto& sourceRouter : routers) {
        map<Router*, int> shortestPaths;
        map<Router*, Router*> previousRouters;
        set<Router*> visitedRouters;
        for(auto& router : routers) {
            shortestPaths[router] = numeric_limits<int>::max();
        }
        shortestPaths[sourceRouter] = 0;
        for(int i = 0; i < routers.size(); i++) {
            Router* closestRouter = nullptr;
            int shortestDistance = numeric_limits<int>::max();
            for(auto& router : routers) {
                if(visitedRouters.find(router) == visitedRouters.end() && shortestPaths[router] < shortestDistance) {
                    closestRouter = router;
                    shortestDistance = shortestPaths[router];
                }
            }
            visitedRouters.insert(closestRouter);
            for(auto& neighbor : closestRouter->routingTable) {
                int alternativePathDistance = shortestPaths[closestRouter] + neighbor.second;
                if(alternativePathDistance < shortestPaths[neighbor.first]) {
                    shortestPaths[neighbor.first] = alternativePathDistance;
                    previousRouters[neighbor.first] = closestRouter;
                }
            }
        }
        sourceRouter->routingTable = shortestPaths;
        sourceRouter->previousRouter = previousRouters;
    }
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
        if (j != i) {
            int cost = distr(gen);
            routers[i]->updateRoutingTable(routers[j], cost);
            routers[j]->updateRoutingTable(routers[i], cost);
        }
        // Para el resto de enrutadores, decidimos aleatoriamente si conectarlos o no
        for(int j = i + 1; j < numRouters; j++) {
            if (distr_bool(gen)) {
                int cost = distr(gen);
                routers[i]->updateRoutingTable(routers[j], cost);
                routers[j]->updateRoutingTable(routers[i], cost);
            }
        }
    }
}
struct RouterConnection {
    Router* router;
    int cost;
    bool operator>(const RouterConnection& other) const {
        return cost > other.cost;
    }
};

void Network::getShortestPath(Router* source, Router* destination) {
    map<Router*, int> distances;
    map<Router*, Router*> previous;
    priority_queue<RouterConnection, vector<RouterConnection>, greater<RouterConnection>> queue;

    // Inicializar distancias
    for (auto& router : routers) {
        distances[router] = numeric_limits<int>::max();
        previous[router] = nullptr;
    }
    distances[source] = 0;
    queue.push({source, 0});

    // Algoritmo de Dijkstra
    while (!queue.empty()) {
        Router* current = queue.top().router;
        int currentDistance = queue.top().cost;
        queue.pop();

        if (current == destination) {
            break; // Hemos encontrado el destino
        }
        if (currentDistance > distances[current]) {
            continue; // Ya hemos encontrado un camino más corto
        }
        for (auto& connection : current->routingTable) {
            Router* neighbor = connection.first;
            int newDistance = distances[current] + connection.second;
            if (newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                previous[neighbor] = current;
                queue.push({neighbor, newDistance});
            }
        }
    }

    // Reconstruir el camino más corto
    list<Router*> path;
    for (Router* at = destination; at != nullptr; at = previous[at]) {
        path.push_front(at);
    }

    // Verificar si existe camino
    if (path.empty() || path.front() != source) {
        cout << "No existe camino entre " << source->name << " y " << destination->name << ".\n";
        return;
    }

    // Imprimir el camino más corto
    cout << "Camino más corto de " << source->name << " a " << destination->name << ": ";
    for (auto it = path.begin(); it != path.end(); ++it) {
        if (it != path.begin()) cout << " -> ";
        cout << (*it)->name;
    }
    cout << "\nCosto total: " << distances[destination] << "\n";
}
void Network::printAllRoutingTables() {
    for(auto& router : routers) {
        router->printRoutingTable();
    }
}
