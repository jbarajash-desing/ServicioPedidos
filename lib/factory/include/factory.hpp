#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "../../calendar/include/calendar.hpp"

// -------------------------------------------------------------
// Clase que representa un pedido individual
// -------------------------------------------------------------
struct Order : public CalendarItem {
    std::string id;
    int urgency;
    int fabricationTime;
    int remainingTime;
    std::string client;

    Order(std::string id, int urgency, int fabricationTime, std::string client);
    Order(const Package& pkg); // constructor desde Package
    std::string info() const override { 
        return std::string("Order id=") + id + " client=\"" + client + "\" urgency=" + std::to_string(urgency);
    }
};

// conversor desde CalendarItem (devuelve nullptr si no es posible)
std::shared_ptr<Order> createOrderFromItem(const std::shared_ptr<CalendarItem>& item);

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
    std::vector<Order> lastBatch; // 
    std::vector<Order> currentBatch;  // Lote actual en construcción
    int currentBatchTime;          // Tiempo acumulado del lote actual
public:
    Factory(int quantum);
    std::vector<std::vector<Order>> processDayThroughFactory(const std::array<Hour, 24>& dayHours);
    void receiveOrders(std::vector<std::vector<Order>>& ordersPerHour); // Recibe los pedidos por hora (existing)
    void receiveOrders(const std::vector<std::shared_ptr<Order>>& orders); // nuevo: recibe lista plana de shared_ptr<Order>
    void process();          // Procesa todos los pedidos según prioridad
    std::vector<Order> getLastBatch(); // 
    
    std::vector<Order> getNext24hBatch();  // Obtener próximo lote de 24h
    void flushBatch();                     // Forzar creación de lote
    void printBatchStatus();
    std::vector<std::vector<Order>> getAllBatchesAsVector() const;
    void clear(); // Limpia todas las colas y lotes
};

#endif