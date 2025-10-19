#include "bank_customer.h"
#include <iostream>
#include <iomanip>

using namespace std;

string BankCustomer::getName() const {
    return this->name;
}

int BankCustomer::getId() const {
    return this->id ;
}

double BankCustomer::getBalance() const {
    return this->balance;
}

void BankCustomer::setName(const string& name) {
    this->name = name;
}

void BankCustomer::setBalance(double amount) {
    this->balance = amount;
}

void BankCustomer::addBalance(double amount) {
    this->balance += amount;
}

bool BankCustomer::withdrawBalance(double amount){
    if (amount > this->balance) {
        return false;
    }
    this->balance -= amount;
    return true;
}

void BankCustomer::printInfo() const {
    std::cout << "Customer Name: " << this->name << std::endl;
    std::cout << "Customer ID: " << this->id << std::endl;
    std::cout << "Balance: $" << fixed << setprecision(2) << this->balance << std::endl;
}
