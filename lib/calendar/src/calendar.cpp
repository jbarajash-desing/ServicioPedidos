#include "Calendario.h"

// Constructor por defecto (no hace nada especial porque los arreglos ya están definidos)
Calendar::Calendar() {}

// Agrega un paquete a la lista correspondiente según mes, día y hora
void Calendar::addPackage(int month, int day, int hour, const Package& pkg) {
    schedule[month][day][hour].push_back(pkg);
}

// Muestra los paquetes del día especificado, clasificando entre depositados y entregados
void Calendar::showPackagesByDay(int month, int day) const {
    list<Package> deposited;
    list<Package> delivered;

    // Recorre las 24 horas del día
    for (int h = 0; h < 24; ++h) {
        for (const auto& pkg : schedule[month][day][h]) {
            if (pkg.status == "deposited")
                deposited.push_back(pkg);
            else if (pkg.status == "delivered")
                delivered.push_back(pkg);
        }
    }

    // Imprime los depositados
    cout << "\nDeposited:\n";
    for (const auto& p : deposited)
        cout << "ID: " << p.id << "  Client: " << p.client << endl;

    // Imprime los entregados
    cout << "\nDelivered:\n";
    for (const auto& p : delivered)
        cout << "ID: " << p.id << "  Client: " << p.client << endl;
}
