#include "calendar.hpp"
#include "../../factory/include/factory.hpp"
#include <sstream>

// Implementación de Package::info (simple)
std::string Package::info() const {
    std::ostringstream oss;
    oss << "Id=" << id << " Cliente=\"" << client << "\" Prioridad de envio=" << urgency
        << " Tiempo fabricacion=" << fabricationTime << " Estado=" << status;
    return oss.str();
}

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

    oss << "\nItems del dia " << (day+1) << " del mes " << (month+1) << "\n";
    for (int h = 0; h < 24; ++h) {
        for (const auto& itemPtr : schedule[month][day][h]) {
            if (itemPtr) oss << "  - " << itemPtr->info() << "| hora:" << h <<"\n";
        }
    }

    return oss;
}

// Devuelve una copia del arreglo de 24 horas con los items del día solicitado
std::array<Hour,24> Calendar::getDay(int month, int day) const {
    std::array<Hour,24> empty{}; // devuelvo vacío en caso de error
    if (month < 0 || month >= 12) return empty;
    if (day < 0 || day >= (int)schedule[month].size()) return empty;
    return schedule[month][day]; // copia segura del día solicitado
}

std::vector<std::shared_ptr<Order>> Calendar::getOrdersFromDay(int month, int day) const {
    std::vector<std::shared_ptr<Order>> orders;
    auto dayHours = getDay(month, day);  // obtengo las 24 horas

    for (const auto& hourList : dayHours) {         // recorro las 24 horas
        for (const auto& itemPtr : hourList) {      // recorro los items en cada hora
            if (!itemPtr) continue;
            // usar factory.createOrderFromItem (función libre) para convertir cualquier CalendarItem a Order
            auto orderPtr = createOrderFromItem(itemPtr);
            if (orderPtr) orders.push_back(orderPtr);
        }
    }

    return orders;
}
