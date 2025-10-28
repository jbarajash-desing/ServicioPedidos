#ifndef CALENDAR_HPP
#define CALENDAR_HPP

#include <iostream>
#include <list>
#include <string>
#include <memory> 
#include <vector>
#include <array>

// forward declare Order to allow conversion declarations (Order se define en factory.hpp)
struct Order;

// Elemento base almacenable en el calendario. Se pueden derivar tipos distintos (Package, etc.)
struct CalendarItem {
    virtual ~CalendarItem() = default;
    virtual std::string info() const = 0; // representación textual mínima
};

// Estructura que representa un paquete (deriva de CalendarItem)
struct Package : public CalendarItem {
    int id;
    int urgency;            // Nivel de urgencia (1 = alta, 2 = media, 3 = baja)
    int fabricationTime;    // Tiempo total necesario para fabricar
    std::string client;
    std::string status; // "deposited" o "delivered"

    Package() = default;
    Package(int idc, int urgencyc, int fabricationTimec, const std::string& clientc, const std::string& statusc)
        : id(idc), urgency(urgencyc), fabricationTime(fabricationTimec), client(clientc), status(statusc) {}

    std::string info() const override; // override para info()
    static Package convertOrderToPackage(const Order& order); // declarado aquí, implementado en factory.cpp
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

    // Devuelve una copia del arreglo de 24 horas con los items del día solicitado
    std::array<Hour,24> getDay(int month, int day) const;

    std::vector<std::shared_ptr<Order>> getOrdersFromDay(int month, int day) const;
};

#endif
