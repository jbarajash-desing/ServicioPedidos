#include "../lib/factory/include/factory.hpp"

int main() {
    Factory factory(5); // quantum = 5 unidades de tiempo

    // Crear 24 horas con pedidos (ejemplo simple)
    std::vector<std::vector<Order>> ordersPerHour(24);

    // Agregamos algunos pedidos manualmente
    ordersPerHour[0].push_back(Order("Pedido_U1", 1, 8));   // Urgencia alta
    ordersPerHour[0].push_back(Order("Pedido_U2", 1, 5));   // Urgencia alta

    ordersPerHour[1].push_back(Order("Pedido_N1", 2, 7));   // Urgencia media
    ordersPerHour[1].push_back(Order("Pedido_N2", 2, 10));  // Urgencia media
    ordersPerHour[2].push_back(Order("Pedido_N3", 2, 4));   // Urgencia media
    ordersPerHour[3].push_back(Order("Pedido_N4", 2, 6));   // Urgencia media

    ordersPerHour[5].push_back(Order("Pedido_B1", 3, 9));   // Urgencia baja
    // Pasar pedidos al sistema
    factory.receiveOrders(ordersPerHour);

    // Procesar todo
    factory.process();

    // Mostrar resultados
    factory.showQueues();
    factory.showOutput();

    return 0;
}