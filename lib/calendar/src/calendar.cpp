#include "calendar.hpp"
#include <sstream>

// Constructor: construir schedule con 12 meses y cada mes con daysPerMonth[mes] días
Calendar::Calendar() {
    schedule.clear();
    schedule.resize(12);
    for (int m = 0; m < 12; ++m) {
        int days = daysPerMonth[m];
        schedule[m].resize(days);
        for (int d = 0; d < days; ++d) {
            // cada día tiene un array de 24 horas; se construye vacío por defecto
            schedule[m][d] = std::array<Hour,24>();
        }
    }
}

// Agrega un item genérico (por ejemplo Package) al calendario en una hora específica
void Calendar::addItem(int month, int day, int hour, std::shared_ptr<CalendarItem> item) {
    if (month < 0 || month >= 12) return;
    if (day < 0 || day >= (int)schedule[month].size()) return;
    if (hour < 0 || hour >= 24) return;
    schedule[month][day][hour].push_back(std::move(item));
}

// Conveniencia: agregar Package concreto
void Calendar::addPackage(int month, int day, int hour, const Package& pkg) {
    addItem(month, day, hour, std::make_shared<Package>(pkg));
}

// Recibe un arreglo de 24 horas con listas de items, y lo almacena en el calendario
void Calendar::storeDay(int year, int month, int day, const Hour externalHours[24]) {
    if (month < 0 || month >= 12) return;
    if (day < 0 || day >= (int)schedule[month].size()) return;
    for (int h = 0; h < 24; ++h) {
        for (const auto& itemPtr : externalHours[h]) {
            schedule[month][day][h].push_back(itemPtr);
        }
    }
}

// Muestra todos los items del día usando info() de cada item
std::ostringstream Calendar::showDay(int month, int day) const {
    std::ostringstream oss;
    if (month < 0 || month >= 12) return oss;
    if (day < 0 || day >= (int)schedule[month].size()) return oss;

    oss << "\nItems del día " << (day+1) << " del mes " << (month+1) << "\n";
    for (int h = 0; h < 24; ++h) {
        oss << "Hora " << h << ":\n";
        for (const auto& itemPtr : schedule[month][day][h]) {
            if (itemPtr) oss << "  - " << itemPtr->info() << "\n";
        }
    }

    return oss;
}
