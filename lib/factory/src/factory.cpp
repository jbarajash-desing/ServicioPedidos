#include "../include/factory.hpp"
// -------------------------------------------------------------
// Implementación de la clase Order
// -------------------------------------------------------------
Order::Order(std::string name, int urgency, int fabricationTime)
    : name(name), urgency(urgency),
      fabricationTime(fabricationTime), remainingTime(fabricationTime) {}

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
                    std::cout << "Invalid urgency for order: " << order.name << std::endl;
                    break;
            }
        }
    }
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
