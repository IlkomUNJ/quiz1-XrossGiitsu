#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <string>

using namespace std;

class BankCustomer {
private:
    int id;
    string name;
    double balance;

public:
    BankCustomer() : id(0), name(""), balance(0.0) {}
    
    BankCustomer(int id, const string& name, double balance) : id(id), name(name), balance(balance) {
    }

    int getId() const;
    string getName() const;
    double getBalance() const;

    void printInfo() const;
    void setName(const string& name);
    void setBalance(double balance);
    void addBalance(double amout);
    bool withdrawBalance(double amout);
};

#endif // BANK_CUSTOMER_H
