#ifndef PRODUCTS_HPP
#define PRODUCTS_HPP

#include <string>
#include <list>

class Products {
    private:
        int id;
        std::string name;
        int HMade;
    public:
        Products(); // Constructor
        Products(int id, const std::string& name, int HMade);
        int getID() const;
        int getHMade() const;
};

class Storage {
    private:
        std::list<Products> productList;
    public:
        void addProduct(const Products& product);
        int getHMadebyId(int id) const;
};

extern Storage storage;      // declaración del objeto global
void initProducts();         // función para poblar el storage

#endif