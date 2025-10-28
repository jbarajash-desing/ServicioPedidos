#include <iostream>
#include "../include/products.hpp" // Esta linea es necesaria pra que reconozca la clase Products y Storage
#include <vector>

Products::Products() : id(0), name(""), HMade(0) {}
Products::Products(int id_, const std::string& name_, int HMade_) : id(id_), name(name_), HMade(HMade_) {}
int Products::getID() const { return id; }
int Products::getHMade() const { return HMade; }
std::string Products::getName() const {
    return name;
}


void Storage::addProduct(const Products& product) {
    productList.push_back(product);
}
int Storage::getHMadebyId(int id) const {
    for (const auto& product : productList) {
        if (product.getID() == id) return product.getHMade();
    }
    return -1;
}
std::vector<std::string> Storage::getProductNames() const {
    std::vector<std::string> names;
    for (const auto& p : productList) {
        names.push_back(p.getName());
    }
    return names;
}

std::vector<int> Storage::getProductTimes()const{
    std::vector<int> times;
    for (const auto& p : productList) {
        times.push_back(p.getHMade());
    }
    return times;
}

Storage storage; // definición del objeto global

void initProducts() {
    Products television(1, "Television", 50);
    Products laptop(2, "Laptop", 35);
    Products microwave(3, "Microondas", 20);
    storage.addProduct(television);
    storage.addProduct(laptop);
    storage.addProduct(microwave);
}



