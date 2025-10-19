#ifndef BUYER_H
#define BUYER_H

#include <cstddef>
#include <string>
#include <vector>
#include "bank_customer.h" 
#include "order.h" 

using namespace std;

class Buyer {
private:
    int id;
    string name;
    BankCustomer &account; 
    vector<Order> orders; 

public:
    Buyer(int id, const string& name, BankCustomer &account0)
        : id(id), name(name), account(account0) {}

    int getId() const { return id; }
    string getName() const { return name; }
    BankCustomer& getAccount() { return account; } 
    vector<Order>& getOrders() { return orders; } 

    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }

    void printInfo() const {
        std::cout << "Buyer Name: " << name << std::endl;
        std::cout << "Buyer ID: " << id << std::endl;
    }
};

#endif // BUYER_H
