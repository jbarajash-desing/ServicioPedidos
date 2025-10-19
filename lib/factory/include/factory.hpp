#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <queue>
#include <vector>
#include <string>
#include <iostream>

// -------------------------------------------------------------
// Clase que representa un pedido individual
// -------------------------------------------------------------
class Order {
public:
    std::string name;       // Nombre o identificador del pedido
    int urgency;            // Nivel de urgencia (1 = alta, 2 = media, 3 = baja)
    int fabricationTime;    // Tiempo total necesario para fabricar
    int remainingTime;      // Tiempo que falta para completar

    Order(std::string name, int urgency, int fabricationTime);
};

// -------------------------------------------------------------
// Cola circular (Round Robin) para pedidos de urgencia media
// -------------------------------------------------------------
class CircularQueue {
private:
    std::queue<Order> queue;     // Cola interna
    int quantum;                 // Tiempo fijo asignado por turno
    std::vector<Order> completed; // Pedidos completados en este ciclo

public:
    CircularQueue(int quantum);

    void addOrder(const Order& order);     // Agrega un pedido
    void processOrders();                  // Procesa los pedidos parcialmente
    std::vector<Order> getCompleted();     // Retorna los pedidos terminados
    bool isEmpty() const;                  // Indica si la cola está vacía
};

// -------------------------------------------------------------
// Clase principal que coordina la fabricación
// -------------------------------------------------------------
class Factory {
private:
    std::queue<Order> highPriority;           // Urgencia 1
    CircularQueue mediumPriority;             // Urgencia 2
    std::queue<Order> lowPriority;            // Urgencia 3

    std::queue<std::vector<Order>> outputQueue; // Cola de colas de pedidos completados

public:
    Factory(int quantum);

    void receiveOrders(std::vector<std::vector<Order>>& ordersPerHour); // Recibe los pedidos por hora
    void process();          // Procesa todos los pedidos según prioridad
    void showQueues();       // Muestra el estado actual de las colas
    void showOutput();       // Muestra los pedidos completados
};

#endif