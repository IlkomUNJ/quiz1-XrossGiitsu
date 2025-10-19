#include "bank.h"
#include "cash_flow_log.h" 
#include "transaction.h"
#include "buyer.h" 
#include <algorithm>
#include <stdexcept>
#include <map>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;

extern vector<Transaction> allTransactions; 
extern vector<Buyer> buyers; 
extern vector<CashFlowLog> cashFlows; 

Bank::Bank(const std::string& name) : name(name), customerCount(0) { }

BankCustomer& Bank::createAccount(const std::string& customerName,
                                  double initialDeposit,
                                  const std::string& address,
                                  const std::string& phone,
                                  const std::string& email) {
    
    customerCount++;
    int newId = customerCount;
    Accounts.emplace_back(newId, customerName, initialDeposit);
    
    CashFlowLog initialDepositLog(newId, "Credit", initialDeposit, "Initial Deposit");
    cashFlows.push_back(initialDepositLog);

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
    
    CashFlowLog depositLog(id, "Credit", amount, "User Topup/Deposit");
    cashFlows.push_back(depositLog);

    std::cout << "Bank: Deposit successful. New balance: $" << fixed << setprecision(2) << acct->getBalance() << "\n";
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
        CashFlowLog withdrawLog(id, "Debit", amount, "User Withdrawal");
        cashFlows.push_back(withdrawLog);
        
        std::cout << "Bank: Withdraw successful. New balance: $" << fixed << setprecision(2) << acct->getBalance() << "\n";
    } else {
        std::cout << "Bank: Withdraw failed due to insufficient funds.\n";
    }
    return ok;
}

void Bank::printCustomerInfo(int id) {
    BankCustomer* acct = findCustomerByIdOrNull(id);
    if (!acct) {
        std::cout << "Bank: Account not found.\n";
        return;
    }
    acct->printInfo();
}

void Bank::listAllCustomers() const {
    std::cout << "\n--- LIST OF ALL BANK CUSTOMERS (" << Accounts.size() << ") ---" << std::endl;
    for (const auto &a : Accounts) {
        std::cout << "ID " << a.getId() << ": " << a.getName()
                  << " (Balance: $" << fixed << setprecision(2) << a.getBalance() << ")\n";
    }
    std::cout << "---------------------------------------------------" << std::endl;
}

void Bank::listDormantAccounts() const {
    auto thirty_days_ago = chrono::system_clock::now() - chrono::hours(24 * 30); 
    std::vector<const BankCustomer*> dormantList;

    for (const auto& customer : Accounts) {
        bool hasRecentActivity = false;
        
        for (const auto& transaction : allTransactions) {
            if ((transaction.getBuyerId() == customer.getId() || transaction.getSellerId() == customer.getId()) &&
                transaction.getTimestamp() >= thirty_days_ago) {
                hasRecentActivity = true;
                break;
            }
        }
        
        if (!hasRecentActivity) {
            for (const auto& flow : cashFlows) {
                if (flow.getAccountId() == customer.getId() && flow.getTimestamp() >= thirty_days_ago) {
                    hasRecentActivity = true;
                    break;
                }
            }
        }

        if (!hasRecentActivity) {
            dormantList.push_back(&customer);
        }
    }

    std::cout << "\n--- LIST OF DORMANT ACCOUNTS (" << dormantList.size() << ") ---" << std::endl;
    if (dormantList.empty()) {
        std::cout << "No dormant accounts found." << std::endl;
    } else {
        for (const auto* customer : dormantList) {
            std::cout << "ID " << customer->getId() << ": " << customer->getName()
                      << " (Balance: $" << fixed << setprecision(2) << customer->getBalance() << ")\n";
        }
    }
    std::cout << "------------------------------------------" << std::endl;
}

void Bank::listTopUsersToday(int n) const {
    time_t now_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
    struct tm *ltm = localtime(&now_t);
    ltm->tm_hour = 0;
    ltm->tm_min = 0;
    ltm->tm_sec = 0;
    auto today_start = chrono::system_clock::from_time_t(mktime(ltm));

    map<int, int> userActivityCount; 

    for (const auto& t : allTransactions) {
        if (t.getTimestamp() >= today_start) {
            userActivityCount[t.getBuyerId()]++;
            userActivityCount[t.getSellerId()]++;
        }
    }
    for (const auto& flow : cashFlows) {
         if (flow.getTimestamp() >= today_start) {
            userActivityCount[flow.getAccountId()]++;
        }
    }

    vector<pair<int, int>> sortedUsers; 
    for (const auto& pair : userActivityCount) {
        sortedUsers.push_back({pair.second, pair.first}); 
    }
    sort(sortedUsers.rbegin(), sortedUsers.rend());

    std::cout << "\n--- TOP " << n << " ACTIVE USERS TODAY (By Activity Count) ---" << std::endl;
    if (sortedUsers.empty()) {
        std::cout << "No transaction activity found today." << std::endl;
        return;
    }
    for (int i = 0; i < min((int)sortedUsers.size(), n); ++i) {
        int userId = sortedUsers[i].second;
        int count = sortedUsers[i].first;
        
        string name = "Unknown";
        for (const auto& b : buyers) {
            if (b.getId() == userId) {
                name = b.getName();
                break;
            }
        }
        
        std::cout << i + 1 << ". ID " << userId << " (" << name << ") - " 
                  << count << " activities\n";
    }
    std::cout << "---------------------------------------------------" << std::endl;
}
