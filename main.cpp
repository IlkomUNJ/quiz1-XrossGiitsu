#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include "bank_customer.h"
#include "buyer.h"
#include "seller.h"
#include "bank.h"

using namespace std;

enum PrimaryPrompt { LOGIN, REGISTER, EXIT };

Bank myBank("Main Bank");
vector<Buyer> buyers;
vector<seller> sellers;
int buyerIdCounter = 0;
bool isLoggedIn = false;

Buyer* currentBuyer = nullptr;
seller* currentSeller = nullptr;

void handleRegister();
void handleLogin();
void buyerMenu();
void sellerMenu();
void bankingFunctions(BankCustomer& account);
void upgradeToSeller();
seller* findSeller(int buyerId);
Buyer* findBuyer(int buyerId);

int main() {
    BankCustomer& aliceAcc = myBank.createAccount("Alice", 2000.0);
    buyers.emplace_back(++buyerIdCounter, "Alice", aliceAcc); 
    sellers.emplace_back(buyers.back(), buyers.back().getId(), "Alice's Store");


    PrimaryPrompt prompt = LOGIN;
    while (prompt != EXIT) {
        cout << "\n===================================" << endl;
        cout << "Select an option: " << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "===================================" << endl;

        int choice;
        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        if (choice < 1 || choice > 3) {
            cout << "Invalid option." << endl;
            continue;
        }
        
        prompt = static_cast<PrimaryPrompt>(choice - 1);
        switch (prompt) {
            case LOGIN:
                handleLogin();
                break;
            case REGISTER:
                handleRegister();
                break;
            case EXIT:
                cout << "Exiting." << std::endl;
                break;
        }
    }

    return 0;
}

Buyer* findBuyer(int id) {
    for (auto& b : buyers) {
        if (b.getId() == id) return &b;
    }
    return nullptr;
}

seller* findSeller(int buyerId) {
    for (auto& s : sellers) {
        if (s.getId() == buyerId) return &s;
    }
    return nullptr;
}

void handleRegister() {
    cout << "\n--- REGISTRATION ---" << endl;
    string name, address, phone, email, storeName;
    double initialDeposit;
    char isSeller;

    cout << "Enter Name: "; cin.ignore(); getline(cin, name);
    cout << "Enter Home Address: "; getline(cin, address);
    cout << "Enter Phone number: "; getline(cin, phone);
    cout << "Enter Email: "; getline(cin, email);
    cout << "Enter Initial Deposit Amount: $"; 
    while (!(cin >> initialDeposit) || initialDeposit < 0) {
        cout << "Invalid amount. Enter Initial Deposit Amount: $";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    try {
        BankCustomer& newAcc = myBank.createAccount(name, initialDeposit, address, phone, email);

        buyers.emplace_back(++buyerIdCounter, name, newAcc);
        Buyer& newBuyer = buyers.back();
        
        cout << "Buyer account created (ID: " << newBuyer.getId() << ").\n";

        cout << "Do you want to create a Seller Account now? (y/n): "; 
        cin >> isSeller;
        
        if (tolower(isSeller) == 'y') {
            cout << "Enter Store Name: "; cin.ignore(); getline(cin, storeName);

            sellers.emplace_back(newBuyer, newBuyer.getId(), storeName);
            cout << "Seller account created (Store Name: " << storeName << ").\n";
        }

        currentBuyer = &newBuyer;
        currentSeller = findSeller(newBuyer.getId());
        cout << "\nRegistration complete. Logged in as " << currentBuyer->getName() << ".\n";

        buyerMenu(); 
        
    } catch (const exception& e) {
        cout << "Registration Failed: " << e.what() << endl;
    }
}

void handleLogin() {
    cout << "\n--- LOGIN ---" << endl;
    int id;
    cout << "Enter Buyer ID: "; 
    if (!(cin >> id)) {
        cout << "Invalid ID format." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    currentBuyer = findBuyer(id);

    if (currentBuyer) {
        currentSeller = findSeller(id);
        cout << "Login successful. Welcome, " << currentBuyer->getName() << "!\n";
        isLoggedIn = true;
        buyerMenu();
    } else {
        cout << "Login failed. Buyer ID not found.\n";
    }
}

void buyerMenu() {
    int choice;
    do {
        cout << "\n--- BUYER MENU (Logged in as: " << currentBuyer->getName() << ") ---" << endl;
        bool isSeller = (currentSeller != nullptr);

        cout << "1. Check Account Status" << endl;
        if (!isSeller) {
            cout << "2. Upgrade Account to Seller" << endl;
        } else {
            cout << "2. Go to Seller Menu" << endl; 
        }
        cout << (isSeller ? "3" : "2") << ". Banking Functions (Deposit/Withdraw/Balance)" << endl;
        cout << (isSeller ? "4" : "3") << ". Browse Store Functionality (DUMMY)" << endl;
        cout << (isSeller ? "5" : "4") << ". Order Functionality (DUMMY)" << endl;
        cout << (isSeller ? "6" : "5") << ". Payment Functionality (DUMMY)" << endl;
        cout << (isSeller ? "7" : "6") << ". Logout" << endl;
        cout << (isSeller ? "8" : "7") << ". Delete Account (DUMMY)" << endl;

        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        int baseOption = isSeller ? 0 : 1; 
        
        switch (choice) {
            case 1:
                cout << "\n--- ACCOUNT STATUS ---" << endl;
                currentBuyer->printInfo();
                cout << "Role: " << (isSeller ? "Buyer & Seller" : "Buyer Only") << endl;
                cout << "----------------------" << endl;
                break;
            case 2:
                if (isSeller) {
                    sellerMenu();
                } else {
                    upgradeToSeller();
                    isSeller = (currentSeller != nullptr);
                }
                break;
            case 3 - 1 + baseOption:
                bankingFunctions(currentBuyer->getAccount());
                break;
            case 4 - 1 + baseOption:
                cout << "--- DUMMY: Browse Store Functionality ---" << endl;
                break;
            case 5 - 1 + baseOption:
                cout << "--- DUMMY: Order Functionality ---" << endl;
                break;
            case 6 - 1 + baseOption:
                cout << "--- DUMMY: Payment Functionality ---" << endl;
                break;
            case 7 - 1 + baseOption:
                cout << "Logging out...\n";
                currentBuyer = nullptr;
                currentSeller = nullptr;
                isLoggedIn = false;
                return;
            case 8 - 1 + baseOption:
                cout << "--- DUMMY: Delete Account Functionality ---" << endl;
                break;
            default:
                cout << "Invalid option." << endl;
                break;
        }
    } while (isLoggedIn);
}

void sellerMenu() {
    if (!currentSeller) return;

    int choice;
    do {
        cout << "\n--- SELLER MENU (Store: " << currentSeller->sellerName << ") ---" << endl;
        cout << "9. Check Inventory (DUMMY)" << endl;
        cout << "10. Add Item to Inventory (DUMMY)" << endl;
        cout << "11. Remove Item from Inventory (DUMMY)" << endl;
        cout << "12. View Orders (DUMMY)" << endl;
        cout << "13. Exit to Buyer Menu" << endl;
        cout << "14. Exit Program" << endl;

        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 9:
                cout << "--- DUMMY: Check Inventory ---" << endl;
                break;
            case 10:
                cout << "--- DUMMY: Add Item ---" << endl;
                break;
            case 11:
                cout << "--- DUMMY: Remove Item ---" << endl;
                break;
            case 12:
                cout << "--- DUMMY: View Orders ---" << endl;
                break;
            case 13:
                return;
            case 14:
                currentBuyer = nullptr;
                currentSeller = nullptr;
                isLoggedIn = false;
                return;
            default:
                cout << "Invalid option." << endl;
                break;
        }
    } while (true);
}

void bankingFunctions(BankCustomer& account) {
    int choice;
    double amount;
    do {
        cout << "\n--- BANKING MENU (ID: " << account.getId() << ") ---" << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Deposit" << endl;
        cout << "3. Withdraw" << endl;
        cout << "4. Back to Buyer Menu" << endl;
        
        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Current Balance: $" << account.getBalance() << endl;
                break;
            case 2:
                cout << "Enter amount to deposit: $"; 
                if (!(cin >> amount) || amount <= 0) {
                    cout << "Invalid amount." << endl; break;
                }
                myBank.deposit(account.getId(), amount);
                break;
            case 3:
                cout << "Enter amount to withdraw: $";
                if (!(cin >> amount) || amount <= 0) {
                    cout << "Invalid amount." << endl; break;
                }
                myBank.withdraw(account.getId(), amount);
                break;
            case 4:
                return;
            default:
                cout << "Invalid option." << endl;
                break;
        }
    } while (true);
}

void upgradeToSeller() {
    if (!currentBuyer) return;

    cout << "\n--- SELLER UPGRADE ---" << endl;
    char confirm;
    string storeName;

    cout << "Your account is linked with Bank ID: " << currentBuyer->getAccount().getId() << endl;
    cout << "Confirm upgrade to Seller? (y/n): ";
    cin >> confirm;
    
    if (tolower(confirm) == 'y') {
        cout << "Enter New Store Name: "; cin.ignore(); getline(cin, storeName);

        sellers.emplace_back(*currentBuyer, currentBuyer->getId(), storeName);
        currentSeller = &sellers.back();

        cout << "\n✅ Successfully upgraded " << currentBuyer->getName() 
             << " to Seller! Store Name: " << storeName << ".\n";
    } else {
        cout << "Seller upgrade cancelled.\n";
    }
}
