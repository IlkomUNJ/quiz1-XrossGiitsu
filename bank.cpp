#include "bank.h"
#include <algorithm>
#include <stdexcept>

Bank::Bank(const std::string& name) : name(name), customerCount(0) { }

BankCustomer& Bank::createAccount(const std::string& customerName,
                                  double initialDeposit,
                                  const std::string& address,
                                  const std::string& phone,
                                  const std::string& email) {
    
    customerCount++;
    int newId = customerCount;

    Accounts.emplace_back(newId, customerName, initialDeposit);
    
    std::cout << "Bank: Account created for " << customerName 
              << " with ID: " << newId << std::endl;

    return Accounts.back(); 
}

BankCustomer& Bank::findCustomerById(int id) {
    for (auto &a : Accounts) {
        if (a.getId() == id) return a;
    }
    throw std::runtime_error("Error: Bank Customer ID not found.");
}

BankCustomer* Bank::findCustomerByIdOrNull(int id) {
    for (auto &a : Accounts) {
        if (a.getId() == id) return &a;
    }
    return nullptr;
}

bool Bank::deposit(int id, double amount) {
    BankCustomer* acct = findCustomerByIdOrNull(id);
    if (!acct) {
        std::cout << "Bank: Deposit failed: Account not found.\n";
        return false;
    }
    if (amount <= 0) {
        std::cout << "Bank: Deposit failed: amount must be positive.\n";
        return false;
    }
    acct->addBalance(amount);
    std::cout << "Bank: Deposit successful. New balance: $" << acct->getBalance() << "\n";
    return true;
}

bool Bank::withdraw(int id, double amount) {
    BankCustomer* acct = findCustomerByIdOrNull(id);
    if (!acct) {
        std::cout << "Bank: Withdraw failed: Account not found.\n";
        return false;
    }
    if (amount <= 0) {
        std::cout << "Bank: Withdraw failed: amount must be positive.\n";
        return false;
    }
    bool ok = acct->withdrawBalance(amount); 
    if (ok) {
        std::cout << "Bank: Withdraw successful. New balance: $" << acct->getBalance() << "\n";
    }
    return ok;
}

void Bank::printCustomerInfo(int id) {
    BankCustomer* acct = findCustomerByIdOrNull(id);
    if (!acct) {
        std::cout << "Bank: Account not found.\n";
        return;
    }
    std::cout << "--- Bank Account Info ---" << std::endl;
    acct->printInfo();
    std::cout << "-------------------------" << std::endl;
}

void Bank::listAllAccounts() const {
    std::cout << "--- Accounts in " << name << " ---" << std::endl;
    for (const auto &a : Accounts) {
        std::cout << "- ID " << a.getId() << ": " << a.getName()
                  << " (Balance: $" << a.getBalance() << ")\n";
    }
    std::cout << "--------------------------" << std::endl;
}
