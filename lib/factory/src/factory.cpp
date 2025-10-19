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
Factory::Factory(int quantum) : mediumPriority(quantum) {}

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

    // 4️⃣ Agregar los pedidos completados a la cola de salida
    outputQueue.push(completedOrders);
}

// Muestra el estado de las colas
void Factory::showQueues() {
    std::cout << "=== Current Queues ===" << std::endl;
    std::cout << "High Priority: " << highPriority.size() << " orders" << std::endl;
    std::cout << "Medium Priority: (cannot show count easily from circular queue)" << std::endl;
    std::cout << "Low Priority: " << lowPriority.size() << " orders" << std::endl;
}

// Muestra los pedidos completados
void Factory::showOutput() {
    std::cout << "=== Completed Orders ===" << std::endl;
    int batch = 1;
    std::queue<std::vector<Order>> temp = outputQueue;

    while (!temp.empty()) {
        auto list = temp.front();
        temp.pop();

        std::cout << "Batch " << batch++ << ":" << std::endl;
        for (auto& o : list) {
            std::cout << " - " << o.name << " (urgency " << o.urgency << ")" << std::endl;
        }
        std::cout << std::endl;
    }
}
