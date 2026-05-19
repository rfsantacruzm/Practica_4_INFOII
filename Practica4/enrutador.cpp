#include "enrutador.h"
#include <limits>
#include <iostream>
#include <string>

using namespace std;

void Router::updateRoutingTable(Router* destination, int cost) {
    routingTable[destination] = cost;
}

int Router::getCost(Router* destination) {
    auto it = routingTable.find(destination);
    if(it != routingTable.end()) {
        return it->second;
    } else {
        return numeric_limits<int>::max();
    }
}

void Router::printRoutingTable() {
    cout << "Tabla de enrutamiento para el enrutador " << name << ":\n";
    for(auto& entry : routingTable) {
        cout << "Destino: " << entry.first->name << ", Costo: " << entry.second << "\n";
    }
}
