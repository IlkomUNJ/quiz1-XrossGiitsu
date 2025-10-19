#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <chrono>

using namespace std;

class Transaction {
public:
    enum Status { PAID, CANCELED, COMPLETED };
private:
    int id;
    int buyerId;
    int sellerId;
    string itemName;
    double amount;
    chrono::system_clock::time_point timestamp;
    Status status;

public:
    Transaction(int id, int buyerId, int sellerId, const string& itemName, double amount) 
        : id(id), buyerId(buyerId), sellerId(sellerId), itemName(itemName), amount(amount) {
        timestamp = chrono::system_clock::now(); 
        status = PAID; 
    }

    int getId() const { return id; }
    int getBuyerId() const { return buyerId; }
    int getSellerId() const { return sellerId; }
    string getItemName() const { return itemName; }
    double getAmount() const { return amount; }
    chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    Status getStatus() const { return status; }
    string getStatusString() const {
        switch (status) {
            case PAID: return "PAID";
            case CANCELED: return "CANCELED";
            case COMPLETED: return "COMPLETED";
        }
        return "UNKNOWN";
    }

    void markAsCanceled() { status = CANCELED; }
    void markAsCompleted() { status = COMPLETED; }
    
    void printTimestamp() const;
};

#endif // TRANSACTION_H
