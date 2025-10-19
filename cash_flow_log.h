#ifndef CASH_FLOW_LOG_H
#define CASH_FLOW_LOG_H

#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;

class CashFlowLog {
private:
    int accountId;
    string type; 
    double amount;
    string description;
    chrono::system_clock::time_point timestamp;

public:
    CashFlowLog(int id, const string& type, double amount, const string& description) 
        : accountId(id), type(type), amount(amount), description(description) {
        timestamp = chrono::system_clock::now();
    }

    int getAccountId() const { return accountId; }
    const string& getType() const { return type; }
    double getAmount() const { return amount; }
    const string& getDescription() const { return description; }
    chrono::system_clock::time_point getTimestamp() const { return timestamp; }

    void printInfo() const {
        time_t tt = chrono::system_clock::to_time_t(timestamp);
        struct tm * ptm = localtime(&tt);
        cout << "[" << put_time(ptm, "%Y-%m-%d %H:%M:%S") << "] "
             << getType() << " $" << fixed << setprecision(2) << getAmount() << " - " << getDescription();
    }
};

#endif // CASH_FLOW_LOG_H
