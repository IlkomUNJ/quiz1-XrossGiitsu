#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <iomanip>

using namespace std;

struct OrderItem {
    int itemId;
    string itemName;
    double price;
    int quantity;
    int sellerId; 
};

class Order {
public:
    enum Status { PENDING, PAID, CANCELED, COMPLETED };
private:
    int id;
    int buyerId;
    vector<OrderItem> items;
    double totalAmount;
    Status status;

public:
    Order(int id, int buyerId) : id(id), buyerId(buyerId), totalAmount(0.0), status(PENDING) {}

    void addItem(int itemId, const string& itemName, double price, int quantity, int sellerId) {
        items.push_back({itemId, itemName, price, quantity, sellerId});
        totalAmount += price * quantity;
    }
    
    int getId() const { return id; }
    int getBuyerId() const { return buyerId; }
    double getTotalAmount() const { return totalAmount; }
    Status getStatus() const { return status; }
    const vector<OrderItem>& getItems() const { return items; }
    string getStatusString() const {
        switch (status) {
            case PENDING: return "PENDING";
            case PAID: return "PAID";
            case CANCELED: return "CANCELED";
            case COMPLETED: return "COMPLETED";
        }
        return "UNKNOWN";
    }

    void markAsPaid() { status = PAID; }
    void markAsCanceled() { status = CANCELED; }
    void markAsCompleted() { status = COMPLETED; }

    void printDetails() const {
        cout << "--- INVOICE ID: " << id << " (Status: " << getStatusString() << ") ---" << endl;
        for (const auto& item : items) {
            cout << "- " << item.itemName << " x" << item.quantity 
                 << " @$" << fixed << setprecision(2) << item.price << " (Seller ID: " << item.sellerId << ")" << endl;
        }
        cout << "TOTAL: $" << fixed << setprecision(2) << totalAmount << endl;
        cout << "---------------------------------------" << endl;
    }
};

#endif // ORDER_H
