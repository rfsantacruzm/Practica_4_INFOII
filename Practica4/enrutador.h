#ifndef ENRUTADOR_H
#define ENRUTADOR_H

#include <map>
#include <string>

class Router {
public:
    std::string name;
    std::map<Router*, int> routingTable;

    Router(const std::string& name): name(name) {}

    void updateRoutingTable(Router* destination, int cost);
    int getCost(Router* destination);
    void printRoutingTable();
};

#endif // ENRUTADOR_H
