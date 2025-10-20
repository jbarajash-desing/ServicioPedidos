#include "c:\\archivos\\Nicolas\\U. nacional\\4 semestre\\estrucutras\\ServicioPedidos-main\\lib\\products\\include\\products.hpp"
#include <iostream>

Products::Products() : id(0), name(""), HMade(0) {}
Products::Products(int id_, const std::string& name_, int HMade_) : id(id_), name(name_), HMade(HMade_) {}
int Products::getID() const { return id; }
int Products::getHMade() const { return HMade; }

void Storage::addProduct(const Products& product) {
    productList.push_back(product);
}
int Storage::getHMadebyId(int id) const {
    for (const auto& product : productList) {
        if (product.getID() == id) return product.getHMade();
    }
    return -1;
}

Storage storage; // definición del objeto global

void initProducts() {
    Products television(1, "Television", 50);
    Products laptop(2, "Laptop", 35);
    Products microwave(3, "Microwave", 20);
    storage.addProduct(television);
    storage.addProduct(laptop);
    storage.addProduct(microwave);
}
