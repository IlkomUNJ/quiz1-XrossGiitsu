#ifndef BANK_H
#define BANK_H

#include "bank_customer.h"
#include <iostream>
#include <vector>
#include <string>

class Bank {
private:
    std::string name;
    std::vector<BankCustomer> Accounts; 
    int customerCount;

public:
    Bank(const std::string& name);

    BankCustomer& createAccount(const std::string& customerName,
                                double initialDeposit,
                                const std::string& address = "",
                                const std::string& phone = "",
                                const std::string& email = "");

    BankCustomer& findCustomerById(int id);
    BankCustomer* findCustomerByIdOrNull(int id);

    bool deposit(int id, double amount);
    bool withdraw(int id, double amount);

    void printCustomerInfo(int id);
    
    void listAllCustomers() const;
    void listDormantAccounts() const;
    void listTopUsersToday(int n) const;

    int getNextCustomerId() const { return customerCount + 1; }
};

#endif // BANK_H
