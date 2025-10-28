#include "../include/factory.hpp"
#include <memory>
#include <vector>

// -------------------------------------------------------------
// Implementación de la clase Order
// -------------------------------------------------------------
Order::Order(std::string id, int urgency, int fabricationTime, std::string client)
        : id(std::move(id)), urgency(urgency),
          fabricationTime(fabricationTime), remainingTime(fabricationTime),
          client(std::move(client)) {}

// constructor desde Package
Order::Order(const Package& pkg)
    : id(std::to_string(pkg.id)),
      urgency(pkg.urgency),
      fabricationTime(pkg.fabricationTime),
      remainingTime(pkg.fabricationTime),
      client(pkg.client) {}

// -------------------------------------------------------------
// Conversión desde CalendarItem -> shared_ptr<Order>
// -------------------------------------------------------------
std::shared_ptr<Order> createOrderFromItem(const std::shared_ptr<CalendarItem>& item) {
    if (!item) return nullptr;
    // Si ya es Order
    if (auto o = std::dynamic_pointer_cast<Order>(item)) return o;
    // Si es Package, construir Order desde Package
    if (auto p = std::dynamic_pointer_cast<Package>(item)) {
        return std::make_shared<Order>(*p);
    }
    // No se puede convertir
    return nullptr;
}

// -------------------------------------------------------------
// Procesa un día: recibe array de 24 horas (cada Hour = list<shared_ptr<CalendarItem>>)
// Convierte cada CalendarItem válido a Order y devuelve vector<24> de vector<Order>
// -------------------------------------------------------------
std::vector<std::vector<Order>>  Factory::processDayThroughFactory(const std::array<Hour,24>& dayHours) {
    std::vector<std::vector<Order>> out(24);
    for (int h = 0; h < 24; ++h) {
        for (const auto& itemPtr : dayHours[h]) {
            if (!itemPtr) continue;
            auto orderPtr = createOrderFromItem(itemPtr);
            if (orderPtr) out[h].push_back(*orderPtr);
        }
    }
    return out;
}

// -------------------------------------------------------------
// Conversión Order -> Package: implementación del método estático declarado en calendar.hpp
// -------------------------------------------------------------
Package Package::convertOrderToPackage(const Order& order) {
    Package pkg;
    // convertir id (string) a int de forma segura
    try {
        pkg.id = std::stoi(order.id);
    } catch (...) {
        pkg.id = 0; // valor por defecto si no se puede convertir
    }
    pkg.urgency = order.urgency;
    pkg.fabricationTime = order.fabricationTime;
    pkg.client = order.client;
    // estado por defecto al convertir desde Order
    pkg.status = "delivered";
    return pkg;
}

// -------------------------------------------------------------
// Implementación de la clase CircularQueue
// -------------------------------------------------------------
CircularQueue::CircularQueue(int quantum) : quantum(quantum) {}

void CircularQueue::addOrder(const Order& order) {
    queue.push(order);
}

// Procesa parcialmente los pedidos según el quantum
void CircularQueue::processOrders() {
    completed.clear(); // limpia los completados previos

    if (!queue.empty()) {
        Order current = queue.front();
        queue.pop();

        // Se trabaja una parte del pedido (quantum)
        current.remainingTime -= quantum;

        if (current.remainingTime > 0) {
            // Si aún no termina, se vuelve a colocar al final
            queue.push(current);
        } else {
            // Pedido terminado
            current.remainingTime = 0;
            completed.push_back(current);
        }
    }
}

std::vector<Order> CircularQueue::getCompleted() {
    return completed;
}

bool CircularQueue::isEmpty() const {
    return queue.empty();
}

// -------------------------------------------------------------
// Implementación de la clase Factory
// -------------------------------------------------------------
Factory::Factory(int quantum) : mediumPriority(quantum), currentBatchTime(0) {}

// Recibe los pedidos distribuidos por hora (24 horas)
void Factory::receiveOrders(std::vector<std::vector<Order>>& ordersPerHour) {
    for (int hour = 0; hour < 24; ++hour) {
        for (auto& order : ordersPerHour[hour]) {
            switch (order.urgency) {
                case 1:
                    highPriority.push(order);
                    break;
                case 2:
                    mediumPriority.addOrder(order);
                    break;
                case 3:
                    lowPriority.push(order);
                    break;
                default:
                    std::cout << "Invalid urgency for order: " << order.id << std::endl;
                    break;
            }
        }
    }
}

// Nueva sobrecarga: recibir lista plana de shared_ptr<Order>
void Factory::receiveOrders(const std::vector<std::shared_ptr<Order>>& orders) {
    for (const auto& optr : orders) {
        if (!optr) continue;
        const Order& order = *optr;
        switch (order.urgency) {
            case 1:
                highPriority.push(order);
                break;
            case 2:
                mediumPriority.addOrder(order);
                break;
            case 3:
                lowPriority.push(order);
                break;
            default:
                std::cout << "Invalid urgency for order: " << order.id << std::endl;
                break;
        }
    }
}

// Conversión: vector<shared_ptr<Order>> -> vector<Package>
std::vector<Package> convertOrdersToPackages(const std::vector<std::shared_ptr<Order>>& orders) {
    std::vector<Package> out;
    out.reserve(orders.size());
    for (const auto& optr : orders) {
        if (!optr) continue;
        out.push_back(Package::convertOrderToPackage(*optr));
    }
    return out;
}

// Procesa las colas según prioridad
void Factory::process() {
    std::vector<Order> completedOrders;

    // 1️⃣ Procesar pedidos de urgencia 1 (FIFO)
    while (!highPriority.empty()) {
        Order o = highPriority.front();
        highPriority.pop();
        o.remainingTime = 0;
        completedOrders.push_back(o);
    }

    // 2️⃣ Procesar pedidos de urgencia 2 (Round-Robin)
    while (!mediumPriority.isEmpty()) {
        mediumPriority.processOrders();
        auto finished = mediumPriority.getCompleted();
        for (auto& o : finished) {
            completedOrders.push_back(o);
        }
    }

    // 3️⃣ Procesar pedidos de urgencia 3 (FIFO)
    while (!lowPriority.empty()) {
        Order o = lowPriority.front();
        lowPriority.pop();
        o.remainingTime = 0;
        completedOrders.push_back(o);
    }

    for (auto& order : completedOrders) {
        // Si agregar este pedido excede 24 horas, crear nuevo lote
        if (currentBatchTime + order.fabricationTime > 24) {
            if (!currentBatch.empty()) {
                outputQueue.push(currentBatch);  // Guardar lote completo
                currentBatch.clear();
                currentBatchTime = 0;
               }
        }
        // Agregar pedido al lote actual
        currentBatch.push_back(order);
        currentBatchTime += order.fabricationTime;
    }
    lastBatch = completedOrders;
    flushBatch();
}

std::vector<Order> Factory::getNext24hBatch() {
    if (!outputQueue.empty()) {
        auto batch = outputQueue.front();
        outputQueue.pop();
        return batch;
    }
    return std::vector<Order>{};
}

void Factory::flushBatch() {
    if (!currentBatch.empty()) {
        outputQueue.push(currentBatch);
        currentBatch.clear();
        currentBatchTime = 0;
    }
}

void Factory::printBatchStatus() {
    std::cout << "Lote actual: " << currentBatch.size() << " pedidos, " 
              << currentBatchTime << "/24 horas" << std::endl;
    std::cout << "Lotes completos en cola: " << outputQueue.size() << std::endl;
}

std::vector<Order> Factory::getLastBatch() {
    return lastBatch; // o como guardes el último lote
}
std::vector<std::vector<Order>> Factory::getAllBatchesAsVector() const {
    std::vector<std::vector<Order>> result;
    auto tempQueue = outputQueue;  // Hacemos una copia para no modificar la original
    
    // Transferimos todos los lotes de la cola al vector
    while (!tempQueue.empty()) {
        result.push_back(tempQueue.front());
        tempQueue.pop();
    }
    
    return result;
}
void Factory::clear() {
    // Limpia colas de prioridad
    while (!highPriority.empty()) highPriority.pop();
    while (!lowPriority.empty()) lowPriority.pop();

    // Limpia Round-Robin (urgencia 2)
    while (!mediumPriority.isEmpty()) {
        mediumPriority.processOrders(); // vacía por pasos
        mediumPriority.getCompleted();  // limpia completados
    }

    // Limpia los lotes de salida y buffers
    std::queue<std::vector<Order>> empty;
    std::swap(outputQueue, empty);

    currentBatch.clear();
    lastBatch.clear();
    currentBatchTime = 0;
}
