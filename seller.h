#pragma once
#include "buyer.h"
#include "item.h"
#include <string>
#include <vector>

class seller : public Buyer { 

public: 
    std::string sellerName;
    std::vector<Item> items; 

private:
    bool idDisplayed(int itemId) const {
        return itemId > 0; 
    }

public:
    seller() = default;

    seller(const Buyer& buyer, int sellerId, const std::string& sellerName)
        : Buyer(buyer.getId(), buyer.getName(), buyer.getAccount()), sellerName(sellerName) {
        }

    virtual ~seller() = default;

    void addNewItem(int newId, const std::string& newName, int newQuantity, double newPrice) {
        Item newItem(newId, newName, newQuantity, newPrice);
        items.push_back(newItem);
    }
    
    Item* findItemById(int itemId) {
        for (auto& item : items) {
            if (item.getId() == itemId) return &item;
        }
        return nullptr;
    }

    void setItemPrice(int itemId, double newPrice) {
        if (Item* item = findItemById(itemId)) {
            item->setPrice(newPrice);
        }
    }

    bool replenishItem(int itemId, int amount) {
        if (Item* item = findItemById(itemId)) {
            item->replenish(amount);
            return true;
        }
        return false;
    }

    bool discardItem(int itemId, int amount) {
        if (Item* item = findItemById(itemId)) {
            return item->discard(amount);
        }
        return false;
    }

    void updateItem(int itemId, const std::string& newName, int newQuantity, double newPrice) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.alterItemById(itemId, newName, newQuantity, newPrice); 
            }
        }
    }

    void makeItemVisibleToCustomer(int itemId) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.setDisplay(true); 
                break;
            }
        }
    }

    const std::vector<Item>& getItems() const { return items; }
};
