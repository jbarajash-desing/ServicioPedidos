#include "calendar.hpp"

// Constructor por defecto
Calendar::Calendar() {
    // No se requiere inicialización adicional porque usamos arreglos fijos
}

// Agrega un solo paquete al calendario en una hora específica
void Calendar::addPackage(int month, int day, int hour, const Package& p) {
    calendar[month][day][hour].push_back(p);
}

// Recibe un arreglo de 24 horas con listas de paquetes, y lo almacena en el calendario
void Calendar::storeDay(int year, int month, int day, const Hour externalHours[24]) {
    // Recorre las 24 horas del día recibido
    for (int h = 0; h < 24; ++h) {
        for (const auto& pkg : externalHours[h]) {
            calendar[month][day][h].push_back(pkg);
        }
    }
    // Nota: el parámetro 'year' no se usa porque solo trabajamos con un año fijo (puede ser extendido)
}

// Muestra todos los paquetes del día, separados por "deposited" y "delivered"
void Calendar::showDay(int month, int day) const {
    list<Package> deposited;
    list<Package> delivered;

    // Recorre cada hora del día seleccionado
    for (int h = 0; h < 24; ++h) {
        for (const auto& pkg : calendar[month][day][h]) {
            if (pkg.status == "deposited")
                deposited.push_back(pkg);
            else if (pkg.status == "delivered")
                delivered.push_back(pkg);
        }
    }

    // Mostrar paquetes depositados
    cout << "\nDeposited Packages:\n";
    for (const auto& p : deposited)
        cout << "ID: " << p.id << " | Client: " << p.client << endl;

    // Mostrar paquetes entregados
    cout << "\nDelivered Packages:\n";
    for (const auto& p : delivered)
        cout << "ID: " << p.id << " | Client: " << p.client << endl;
}
