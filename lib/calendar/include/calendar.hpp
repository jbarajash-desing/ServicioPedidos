#ifndef CALENDAR_HPP
#define CALENDAR_HPP

#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <vector>
#include <array>

// Elemento base almacenable en el calendario. Se pueden derivar tipos distintos (Package, etc.)
struct CalendarItem {
    virtual ~CalendarItem() = default;
    virtual std::string info() const = 0; // representación textual mínima
};

// Estructura que representa un paquete (deriva de CalendarItem)
struct Package : public CalendarItem {
    int id;
    std::string client;
    std::string status; // "deposited" o "delivered"
    Package(int idc, const std::string& clientc, const std::string& statusc)
        : id(idc), client(clientc), status(statusc) {}
    std::string info() const override {
        return "ID: " + std::to_string(id) + " | Client: " + client + " | Status: " + status;
    }
};

// Alias para definir una lista de items por cada hora
using Hour = std::list<std::shared_ptr<CalendarItem>>;

// Clase que representa el calendario de entregas y depósitos
class Calendar {
private:
    // Arreglo con la cantidad de días por mes (año no bisiesto)
    int daysPerMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    // Estructura dinámica: schedule[mes][día][hora]
    std::vector<std::vector<std::array<Hour,24>>> schedule;

public:
    Calendar(); // Constructor

    // Agrega un paquete al calendario en una hora específica
    void addPackage(int month, int day, int hour, const Package& pkg);

    // Agrega un item genérico (permite distintos tipos)
    void addItem(int month, int day, int hour, std::shared_ptr<CalendarItem> item);

    // Guarda un día completo desde un arreglo externo de 24 horas
    void storeDay(int year, int month, int day, const Hour externalHours[24]);

    // Muestra todos los paquetes del día (usa info() de los items)
    std::ostringstream showDay(int month, int day) const;
};

#endif
