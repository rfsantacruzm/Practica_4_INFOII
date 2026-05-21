#include "network.h"
#include <iostream>
#include <limits>
#include <fstream>

using namespace std;

// Limpia el estado de cin y descarta la línea actual
void limpiarCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Lee un entero validado. Repite hasta obtener un valor dentro del rango indicado.
int leerEntero(const string& mensaje, int minVal, int maxVal) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            limpiarCin();
            if (valor >= minVal && valor <= maxVal) {
                return valor;
            }
            cout << "Error: ingresa un numero entre " << minVal << " y " << maxVal << ".\n";
        } else {
            cout << "Error: entrada invalida. Ingresa un numero entero.\n";
            limpiarCin();
        }
    }
}

// Lee un string no vacío y sin espacios
string leerNombre(const string& mensaje) {
    string nombre;
    while (true) {
        cout << mensaje;
        if (cin >> nombre && !nombre.empty()) {
            limpiarCin();
            return nombre;
        }
        cout << "Error: el nombre no puede estar vacio.\n";
        limpiarCin();
    }
}

int main() {
    Network network;
    int option = 0;

    while (option != 9) {
        cout << "\n1. Agregar enrutador\n";
        cout << "2. Remover enrutador\n";
        cout << "3. Actualizar red\n";
        cout << "4. Cargar topologia desde archivo\n";
        cout << "5. Encontrar o crear enrutador\n";
        cout << "6. Generar red aleatoria\n";
        cout << "7. Obtener el camino mas corto\n";
        cout << "8. Imprimir costos a cada enrutador\n";
        cout << "9. Salir\n";

        option = leerEntero("Elige una opcion: ", 1, 9);

        switch (option) {
        case 1: {
            string name = leerNombre("Nombre del enrutador: ");

            // Verificar que no exista ya en la red
            bool existe = false;
            for (auto& r : network.routers) {
                if (r->name == name) {
                    existe = true;
                    break;
                }
            }
            if (existe) {
                cout << "Error: ya existe un enrutador con el nombre \"" << name << "\".\n";
                break;
            }

            Router* router = new Router(name);

            int numConnections = leerEntero("Numero de conexiones: ", 0, 1000);

            int i = 0;
            while (i < numConnections) {
                cout << "-- Conexion " << i + 1 << " --\n";
                string connectedRouterName = leerNombre("Nombre del enrutador destino: ");

                // Evitar conectarse a si mismo
                if (connectedRouterName == name) {
                    cout << "Error: un enrutador no puede conectarse a si mismo. Ingresa otro nombre.\n";
                    continue;
                }

                // Verificar si el destino existe en la red
                Router* connectedRouter = nullptr;
                for (auto& r : network.routers) {
                    if (r->name == connectedRouterName) {
                        connectedRouter = r;
                        break;
                    }
                }

                if (connectedRouter == nullptr) {
                    cout << "El enrutador \"" << connectedRouterName << "\" no existe en la red.\n";
                    int crear = leerEntero("¿Deseas crearlo? (1 = Si, 2 = No): ", 1, 2);
                    if (crear == 2) {
                        cout << "Conexion omitida. Verifica el nombre e intentalo de nuevo.\n";
                        continue;
                    }
                    connectedRouter = network.findOrCreateRouter(connectedRouterName);
                    cout << "Enrutador \"" << connectedRouterName << "\" creado.\n";
                }

                // Verificar si ya existe una conexion previa entre ambos routers
                bool conexionPrevia = router->routingTable.find(connectedRouter) != router->routingTable.end();
                if (conexionPrevia) {
                    int costoActual = router->routingTable[connectedRouter];
                    cout << "Advertencia: ya existe una conexion con \"" << connectedRouterName
                         << "\" (costo actual: " << costoActual << ").\n";
                    int reescribir = leerEntero("¿Deseas reescribir el costo? (1 = Si, 2 = No): ", 1, 2);
                    if (reescribir == 2) {
                        cout << "Costo mantenido.\n";
                        i++;
                        continue;
                    }
                }

                int cost = leerEntero("Costo de la conexion: ", 1, numeric_limits<int>::max() - 1);

                router->updateRoutingTable(connectedRouter, cost);
                connectedRouter->updateRoutingTable(router, cost);
                i++;
            }

            network.addRouter(router);
            cout << "Enrutador \"" << name << "\" agregado correctamente.\n";
            break;
        }

        case 2: {
            if (network.routers.empty()) {
                cout << "Error: la red esta vacia, no hay enrutadores para remover.\n";
                break;
            }

            string name = leerNombre("Nombre del enrutador a remover: ");

            Router* router = nullptr;
            for (auto& r : network.routers) {
                if (r->name == name) {
                    router = r;
                    break;
                }
            }

            if (router == nullptr) {
                cout << "Error: no existe un enrutador con el nombre \"" << name << "\".\n";
                break;
            }

            network.removeRouter(router);
            cout << "Enrutador \"" << name << "\" removido correctamente.\n";
            break;
        }

        case 3: {
            if (network.routers.empty()) {
                cout << "Error: la red esta vacia, no hay nada que actualizar.\n";
                break;
            }

            network.updateNetwork();
            cout << "Red actualizada correctamente.\n";
            break;
        }

        case 4: {
            string filename = leerNombre("Nombre del archivo: ");

            ifstream testFile(filename);
            if (!testFile.is_open()) {
                cout << "Error: no se pudo abrir el archivo \"" << filename << "\". Verifica que exista y sea accesible.\n";
                break;
            }
            if (testFile.peek() == ifstream::traits_type::eof()) {
                cout << "Error: el archivo \"" << filename << "\" esta vacio.\n";
                testFile.close();
                break;
            }
            testFile.close();

            network.loadTopologyFromFile(filename);
            cout << "Topologia cargada correctamente desde \"" << filename << "\".\n";
            break;
        }

        case 5: {
            string name = leerNombre("Nombre del enrutador: ");
            Router* r = network.findOrCreateRouter(name);

            // Verificar si ya existia o fue creado
            bool eraExistente = false;
            for (auto& router : network.routers) {
                if (router == r && router->name == name) {
                    eraExistente = true;
                    break;
                }
            }
            if (eraExistente) {
                cout << "Enrutador \"" << name << "\" encontrado.\n";
            } else {
                cout << "Enrutador \"" << name << "\" creado.\n";
            }
            break;
        }

        case 6: {
            if (!network.routers.empty()) {
                cout << "Advertencia: la red ya tiene " << network.routers.size()
                << " enrutador(es). La red aleatoria se agregara sobre los existentes.\n";
                cout << "¿Deseas continuar? (1 = Si, 2 = No): ";
                int confirm = leerEntero("", 1, 2);
                if (confirm == 2) {
                    cout << "Operacion cancelada.\n";
                    break;
                }
            }

            int numRouters = leerEntero("Numero de enrutadores: ", 2, 1000);
            int maxCost    = leerEntero("Costo maximo: ", 1, numeric_limits<int>::max() - 1);

            network.generateRandomNetwork(numRouters, maxCost);
            cout << "Red aleatoria de " << numRouters << " enrutadores generada correctamente.\n";
            break;
        }

        case 7: {
            if (network.routers.empty()) {
                cout << "Error: la red esta vacia.\n";
                break;
            }
            if (network.routers.size() < 2) {
                cout << "Error: se necesitan al menos 2 enrutadores para buscar un camino.\n";
                break;
            }

            string sourceName = leerNombre("Nombre del enrutador origen: ");
            string destinationName = leerNombre("Nombre del enrutador destino: ");

            if (sourceName == destinationName) {
                cout << "Error: el origen y el destino no pueden ser el mismo enrutador.\n";
                break;
            }

            // Verificar que ambos existan en la red (sin crear nuevos)
            Router* source = nullptr;
            Router* destination = nullptr;
            for (auto& r : network.routers) {
                if (r->name == sourceName)      source      = r;
                if (r->name == destinationName) destination = r;
            }

            if (source == nullptr) {
                cout << "Error: no existe un enrutador con el nombre \"" << sourceName << "\".\n";
                break;
            }
            if (destination == nullptr) {
                cout << "Error: no existe un enrutador con el nombre \"" << destinationName << "\".\n";
                break;
            }

            network.getShortestPath(source, destination);
            break;
        }

        case 8: {
            if (network.routers.empty()) {
                cout << "Error: la red esta vacia, no hay tablas que imprimir.\n";
                break;
            }

            network.printAllRoutingTables();
            break;
        }

        case 9: {
            cout << "Saliendo...\n";
            break;
        }
        }
    }

    return 0;
}
