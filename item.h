#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

using namespace std;

class Item {
private:
    int id;
    std::string name;
    int quantity;
    double price;
    bool idDisplay; 

public:
    Item() = default;
    Item(int id, const std::string& name, int quantity, double price)
        : id(id), name(name), quantity(quantity), price(price) {
            idDisplay = false; 
        }

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    bool isDisplayed() const { return idDisplay; } 
    Item *getItem() { return this; }

    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }
    void setPrice(double newPrice) { price = newPrice; }
    void setDisplay(bool display) { idDisplay = display; }

    void replenish(int amount) { quantity += amount; }
    bool discard(int amount) {
        if (quantity < amount) {
            return false;
        }
        quantity -= amount;
        return true;
    }
    bool checkAvailability(int requestedQty) const { return quantity >= requestedQty; }


    void alterItemById(int itemId, const std::string& newName, int newQuantity, double newPrice) {
        if (id == itemId) {
            name = newName;
            quantity = newQuantity;
            price = newPrice;
        }
    }

    void updatePriceQuantity(int itemId, double newPrice, int newQuantity) {
        if (id == itemId) {
            price = newPrice;
            quantity = newQuantity;
        }
    }
};

#endif // ITEM_H
