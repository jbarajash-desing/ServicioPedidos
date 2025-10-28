#ifndef PRODUCTS_HPP
#define PRODUCTS_HPP
#include <vector>

#include <string>
#include <list>
#include <vector>

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
        std::string getName() const;  // <- NUEVO MÉTODO

};

class Storage {
    private:
        std::list<Products> productList;
    public:
        void addProduct(const Products& product);
        int getHMadebyId(int id) const;
        std::vector<std::string> getProductNames() const;
        std::vector<int> getProductTimes() const;


};

extern Storage storage;      // declaración del objeto global
void initProducts();         // función para poblar el storage


#endif

