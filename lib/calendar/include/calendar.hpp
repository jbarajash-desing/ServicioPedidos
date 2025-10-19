 #ifndef CALENDARIO_H
#define CALENDARIO_H

#include <iostream>
#include <list>
#include <string>
using namespace std;

// Estructura que representa un paquete con un ID, cliente y estado
struct Package {
    int id;
    string client;
    string status; // "deposited" o "delivered"
};

// Alias para definir una lista de paquetes por cada hora
using Hour = list<Package>;

// Clase que representa el calendario de entregas y depósitos
class Calendar {
private:
    // Arreglo con la cantidad de días por mes (año no bisiesto)
    int daysPerMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    // Arreglo tridimensional [mes][día][hora] donde cada celda tiene una lista de paquetes
    Hour schedule[12][31][24];

public:
    Calendar(); // Constructor

    // Agrega un paquete al calendario en una hora específica
    void addPackage(int month, int day, int hour, const Package& pkg);

    // Guarda un día completo desde un arreglo externo de 24 horas
    void storeDay(int year, int month, int day, const Hour externalHours[24]);

    // Muestra todos los paquetes depositados y entregados en un día específico
    void showDay(int month, int day) const;
};

#endif
