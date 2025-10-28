#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <sstream>
#include <set>

#include "../lib/factory/include/factory.hpp"
#include "../lib/products/include/products.hpp"
#include "../lib/calendar/include/calendar.hpp"

extern void initProducts();

int promptInt(const std::string& prompt, int minV, int maxV) {
    int v;
    while (true) {
        std::cout << prompt;
        if (!(std::cin >> v)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Entrada invalida\n";
            continue;
        }
        if (v < minV || v > maxV) {
            std::cout << "Valor fuera de rango (" << minV << " - " << maxV << ")\n";
            continue;
        }
        std::cin.ignore(10000, '\n');
        return v;
    }
}

std::string promptLine(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

bool fechasContains(const std::vector<std::array<int,2>>& fechas, int m, int d) {
    for (auto &f : fechas) if (f[0]==m && f[1]==d) return true;
    return false;
}

int mainTerminal() {
    initProducts();
    std::vector<std::string> productList = storage.getProductNames();
    std::vector<int> productTimes = storage.getProductTimes();

    // copia local de dias por mes para normalizar
    const std::vector<int> daysPerMonth = {31,28,31,30,31,30,31,31,30,31,30,31};
    const std::string meses[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };
    const std::string Hours[] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"
    };

    Calendar calendar;
    Factory factory(5); // quantum de 5 horas para urgencia media
    std::vector<std::array<int,2>> fechas;           // almacena {month, day} 0-based
    std::vector<std::array<int,2>> registereddates;  // fechas con pedidos registrados
    std::set<int> processPackageIds;                 // ids de Package por procesar
    int nextId = 1;

    while (true) {
        std::cout << "\n--- Menu ---\n";
        std::cout << "1) Hacer pedido\n";
        std::cout << "2) Ver pedidos (y procesar)\n";
        std::cout << "0) Salir\n";
        int opt = promptInt("Elija una opcion: ", 0, 2);
        std::cout << "------------------------------------------\n";
        if (opt == 0) break;

        // ---------------------------------------------
        // OPCIÓN 1: Hacer pedido
        // ---------------------------------------------
        if (opt == 1) {
            if (productList.empty()) {
                std::cout << "No hay productos cargados.\n";
                continue;
            }

            std::cout << "Productos disponibles:\n";
            for (size_t i = 0; i < productList.size(); ++i) {
                std::cout << i << ": " << productList[i] << "\n";
            }
            std::cout<<"------------------------------------------\n";

            int pidx = promptInt("Selecciona producto (indice): ", 0, (int)productList.size() - 1);
            for (int i = 0; i < 12; ++i) {
                std::cout << "Mes " << (i + 1) << ": " << meses[i] << std::endl;
            }
            std::cout<<"------------------------------------------\n";
            int month = promptInt("Mes (1-12): ", 1, 12) - 1;
            int day = promptInt("Dia (1-" + std::to_string(daysPerMonth[month]) + "): ", 1, daysPerMonth[month]) - 1;
            int hour = promptInt("Hora (0-23): ", 0, 23);

            std::cout << "------------------------------------------\nPrioridades: \n1=Alta \n2=Media \n3=Baja\n";
            int priority = promptInt("Prioridad (1-3): ", 1, 3);
            std::string client = promptLine("Nombre del cliente: ");

            Package pkg;
            pkg.id = nextId++;
            pkg.urgency = priority;
            pkg.fabricationTime = productTimes[pidx];
            pkg.client = client;
            pkg.status = "deposited";

            processPackageIds.insert(pkg.id);
            calendar.addPackage(month, day, hour, pkg);

            if (!fechasContains(fechas, month, day)) fechas.push_back({month, day});
            if (!fechasContains(registereddates, month, day)) registereddates.push_back({month, day});

            std::cout << "Pedido agregado: id=" << pkg.id
                      << " fecha=" << (month + 1) << "/" << (day + 1)
                      << " - " << Hours[hour] << ":00\n";
        }

        // ---------------------------------------------
        // OPCIÓN 2: Ver pedidos (y procesar)
        // ---------------------------------------------
        if (opt == 2) {
            if (fechas.empty()) {
                std::cout << "No hay pedidos registrados.\n";
                continue;
            }
            std::cout << processPackageIds.size() << " pedidos pendientes de procesar.\n"<<registereddates.size()<<" fechas con pedidos registrados.\n";
            // Procesar paquetes pendientes
            for (auto &fd : registereddates) {
                int m = fd[0], d = fd[1];
                auto orders = calendar.getDay(m, d);

                // Filtrar solo los pedidos cuyos Package.id estén en processPackageIds
                std::array<Hour,24> toProcess;

                for (int h = 0; h < 24; ++h) {
                    for (const auto& itemPtr : orders[h]) {
                        if (!itemPtr) continue;
                        // Intentar convertir a Package
                        if (auto pkg = std::dynamic_pointer_cast<Package>(itemPtr)) {
                            if (pkg->status != "delivered" && processPackageIds.find(pkg->id) != processPackageIds.end()) {
                                // El paquete NO ha sido procesado todavía
                                toProcess[h].push_back(itemPtr);
                            }
                        }
                    }
                }

                // Procesar día a través de la fábrica
                auto processing = factory.processDayThroughFactory(toProcess);
                factory.receiveOrders(processing);
                factory.process();
                auto completed = factory.getAllBatchesAsVector();
                factory.clear();

                // Convertir los Orders completados a Packages y actualizar el calendario
                for (size_t b = 0; b < completed.size(); ++b) {
                    for (auto &o : completed[b]) {
                        Package pkg = Package::convertOrderToPackage(o);
                        calendar.addPackage(m, d, 23, pkg);
                        processPackageIds.erase(pkg.id); // marcar como procesado
                        if (!fechasContains(fechas, m, d)) fechas.push_back({m, d});
                    }

                    // Avanzar al siguiente día si hay más batches
                    d++;
                    if (d >= daysPerMonth[m]) {
                        d = 0;
                        m++;
                        if (m >= 12) break;
                    }
                }
            }
            registereddates.clear(); // ya se procesaron

            // Mostrar fechas disponibles
            std::cout << "Fechas con pedidos:\n";
            for (size_t i = 0; i < fechas.size(); ++i) {
                std::cout << i << ") " << (fechas[i][0] + 1)
                          << "/" << (fechas[i][1] + 1) << "\n";
            }

            int choice = promptInt("Selecciona indice de fecha para ver (o -1 para ver todas): ", -1, (int)fechas.size() - 1);
            std::vector<std::array<int,2>> toProcessDates;

            if (choice == -1) {
                toProcessDates = fechas;
            } else {
                toProcessDates.push_back(fechas[choice]);
            }

            // Mostrar pedidos de las fechas seleccionadas
            for (auto &fd : toProcessDates) {
                int m = fd[0], d = fd[1];
                std::ostringstream oss = calendar.showDay(m, d);
                std::cout << oss.str() << "\n";
            }
        }
    }

    std::cout << "Saliendo...\n";
    return 0;
}
