#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "bank_customer.h"
#include "buyer.h"         
#include "seller.h"        
#include "bank.h"          
#include "transaction.h"
#include "cash_flow_log.h"
#include "order.h"

using namespace std;

enum PrimaryPrompt { LOGIN, REGISTER, STORE_CAPABILITIES, BANK_ANALYSIS, EXIT };

Bank myBank("Global Commerce Bank");

vector<Buyer> buyers;
vector<seller> sellers;
vector<Transaction> allTransactions; 
vector<CashFlowLog> cashFlows; 

int buyerIdCounter = 0;
int itemIdCounter = 0; 
int transactionIdCounter = 0; 
int orderIdCounter = 0;

bool isLoggedIn = false;
Buyer* currentBuyer = nullptr;
seller* currentSeller = nullptr;

void handleRegister();
void handleLogin();
Buyer* findBuyer(int id);
seller* findSeller(int buyerId);
seller* findSellerByItem(int itemId);
Item* findItemInStore(int itemId, seller*& itemSeller);

void buyerMenu();
void sellerMenu();
void bankingFunctions(BankCustomer& account);
void upgradeToSeller();

void browseStoreFunctionality();
void orderFunctionality();
void paymentFunctionality();
void checkSpendingLastKDays(int k);
void cashFlowMenu(BankCustomer& account);

void itemManagementMenu();
void discoverTopItemsPerMonth();
void discoverLoyalCustomerPerMonth();

void storeCapabilitiesMenu(); 
void bankAnalysisMenu(); 
void listRecentTransactions(int k);
void listUncompletedTransactions();
void listMostFrequentItems(int m);
void listMostActiveUsers(const string& type, int (Transaction::*getIdFunc)() const);
void listMostActiveBuyers();
void listMostActiveSellers();


int main() {
    cout << fixed << setprecision(2);

    try {
        BankCustomer& aliceAcc = myBank.createAccount("Alice", 2000.0, "Jl. Utama", "08123", "alice@test.com");
        buyers.emplace_back(++buyerIdCounter, "Alice", aliceAcc); 
        Buyer& aliceBuyer = buyers.back();
        sellers.emplace_back(aliceBuyer, aliceBuyer.getId(), "Alice's Marketplace");

        BankCustomer& bobAcc = myBank.createAccount("Bob", 500.0, "Jl. Kedua", "08987", "bob@test.com");
        buyers.emplace_back(++buyerIdCounter, "Bob", bobAcc);
        
        BankCustomer& charlieAcc = myBank.createAccount("Charlie", 1000.0, "Jl. Ketiga", "08888", "charlie@test.com");
        buyers.emplace_back(++buyerIdCounter, "Charlie", charlieAcc);

        itemIdCounter++; 
        sellers.front().addNewItem(itemIdCounter, "Laptop Gaming", 5, 1200.0);
        sellers.front().makeItemVisibleToCustomer(itemIdCounter);
        itemIdCounter++; 
        sellers.front().addNewItem(itemIdCounter, "Mouse Wireless", 10, 50.0);
        sellers.front().makeItemVisibleToCustomer(itemIdCounter);

        Order dummyOrder(++orderIdCounter, aliceBuyer.getId());
        dummyOrder.addItem(1, "Laptop Gaming", 1200.0, 1, aliceBuyer.getId());
        dummyOrder.markAsPaid();
        aliceBuyer.getOrders().push_back(dummyOrder);
        
        allTransactions.emplace_back(++transactionIdCounter, aliceBuyer.getId(), sellers.front().getId(), "Laptop Gaming", 1200.0);
        
    } catch (const exception& e) {
        cerr << "Initialization Error: " << e.what() << endl;
    }


    PrimaryPrompt prompt = LOGIN;
    while (prompt != EXIT) {
        cout << "\n===================================" << endl;
        cout << "MAIN MENU" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Store Capabilities (Admin/General Analysis)" << endl; 
        cout << "4. Bank Analysis (Admin/General Analysis)" << endl; 
        cout << "5. Exit" << endl; 
        cout << "===================================" << endl;

        int choice;
        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        if (choice < 1 || choice > 5) {
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
            case STORE_CAPABILITIES:
                storeCapabilitiesMenu();
                break;
            case BANK_ANALYSIS:
                bankAnalysisMenu();
                break;
            case EXIT:
                cout << "Exiting program." << std::endl;
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

seller* findSellerByItem(int itemId) {
    for (auto& s : sellers) {
        if (s.findItemById(itemId)) {
            return &s;
        }
    }
    return nullptr;
}

Item* findItemInStore(int itemId, seller*& itemSeller) {
    for (auto& s : sellers) {
        if (Item* item = s.findItemById(itemId)) {
            itemSeller = &s;
            return item;
        }
    }
    itemSeller = nullptr;
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
        cerr << "Registration Failed: " << e.what() << endl;
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
        cout << "Login successful. Welcome, " << currentBuyer->getName() << " (ID: " << currentBuyer->getId() << ")!\n";
        isLoggedIn = true;
        buyerMenu();
    } else {
        cout << "Login failed. Buyer ID not found.\n";
    }
}


void buyerMenu() {
    int choice;
    do {
        bool isSeller = (currentSeller != nullptr);
        
        cout << "\n--- BUYER MENU (User: " << currentBuyer->getName() << ") ---" << endl;
        cout << "1. Check Account Status" << endl;
        
        if (!isSeller) {
            cout << "2. Upgrade Account to Seller" << endl;
        } else {
            cout << "2. Go to Seller Menu" << endl; 
        }
        
        int offset = (isSeller ? 1 : 0);
        
        cout << 2 + offset << ". Banking Functions (Topup/Withdraw/Cash Flow)" << endl;
        cout << 3 + offset << ". Browse Store & Purchase" << endl;
        cout << 4 + offset << ". Order Functionality (List/Filter)" << endl;
        cout << 5 + offset << ". Check Spending Last K Days" << endl; 
        cout << 6 + offset << ". Logout" << endl;
        cout << 7 + offset << ". Delete Account (DUMMY)" << endl;

        cout << "Pilihan >> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if (choice == 1) { 
            cout << "\n--- ACCOUNT STATUS ---" << endl;
            try {
                myBank.printCustomerInfo(currentBuyer->getAccount().getId());
            } catch (const exception& e) {
                cerr << "Error fetching bank info: " << e.what() << endl;
            }
            cout << "Role: " << (isSeller ? "Buyer & Seller (Store: " + currentSeller->sellerName + ")" : "Buyer Only") << endl;
            cout << "----------------------" << endl;
        } 
        else if (choice == 2) { 
            if (isSeller) {
                sellerMenu();
            } else {
                upgradeToSeller();
                currentSeller = findSeller(currentBuyer->getId()); 
            }
        } 
        else if (choice == (2 + offset)) { 
            cashFlowMenu(currentBuyer->getAccount());
        }
        else if (choice == (3 + offset)) { 
            browseStoreFunctionality();
        }
        else if (choice == (4 + offset)) { 
            orderFunctionality();
        }
        else if (choice == (5 + offset)) { 
            int k;
            cout << "Enter K (last number of days): ";
            if (!(cin >> k) || k <= 0) { cout << "Invalid input." << endl; break; }
            checkSpendingLastKDays(k);
        }
        else if (choice == (6 + offset)) { 
            cout << "Logging out...\n";
            currentBuyer = nullptr;
            currentSeller = nullptr;
            isLoggedIn = false;
            return;
        }
        else if (choice == (7 + offset)) { 
            cout << "--- DUMMY: Delete Account Functionality ---" << endl;
        }
        else {
             cout << "Invalid option." << endl;
        }

    } while (isLoggedIn);
}

void sellerMenu() {
    if (!currentSeller) return;

    int choice;
    do {
        cout << "\n--- SELLER MENU (Store: " << currentSeller->sellerName << ") ---" << endl;
        cout << "9. Item Management (Register/Replenish/Set Price)" << endl;
        cout << "10. Discover Top K Most Popular Items (Month)" << endl;
        cout << "11. Discover Loyal Customer (Month)" << endl;
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
                itemManagementMenu(); 
                break;
            case 10:
                discoverTopItemsPerMonth();
                break;
            case 11:
                discoverLoyalCustomerPerMonth();
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
        
        cout << "\n✅ Successfully upgraded " << currentBuyer->getName() 
             << " to Seller! Store Name: " << storeName << ".\n";
    } else {
        cout << "Seller upgrade cancelled.\n";
    }
}


void browseStoreFunctionality() {
    cout << "\n--- BROWSE STORE & ADD TO CART ---" << endl;
    if (sellers.empty()) {
        cout << "No sellers available." << endl;
        return;
    }

    int itemId, qty;
    Order currentCart(++orderIdCounter, currentBuyer->getId());

    do {
        cout << "\nAvailable Items (across all stores):" << endl;
        cout << "ID | Name | Seller | Price | Stock" << endl;
        cout << "---|------|--------|-------|------" << endl;
        int displayCount = 0;
        for (const auto& s : sellers) {
            for (const auto& item : s.getItems()) {
                if (item.isDisplayed()) {
                    cout << item.getId() << " | " << item.getName() << " | "
                         << s.sellerName << " | $" << item.getPrice() << " | " 
                         << item.getQuantity() << endl;
                    displayCount++;
                }
            }
        }
        if (displayCount == 0) {
             cout << "No visible items in any store." << endl;
             break; 
        }

        cout << "\nEnter Item ID to add to cart (0 to checkout): ";
        if (!(cin >> itemId)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }

        if (itemId == 0) break;

        seller* itemSeller = nullptr;
        Item* item = findItemInStore(itemId, itemSeller);

        if (item && itemSeller) {
            cout << "Enter Quantity for " << item->getName() << ": ";
            if (!(cin >> qty)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }

            if (qty <= 0) {
                cout << "Quantity must be positive." << endl;
            } else if (!item->checkAvailability(qty)) {
                cout << "Rejected: Insufficient stock. Available: " << item->getQuantity() << endl;
            } else {
                currentCart.addItem(item->getId(), item->getName(), item->getPrice(), qty, itemSeller->getId());
                cout << "✅ Added " << qty << "x " << item->getName() << " to cart." << endl;
            }
        } else {
            cout << "Item ID not found." << endl;
        }
    } while (true);

    if (currentCart.getTotalAmount() > 0) {
        cout << "\n--- CHECKOUT REQUIRED ---" << endl;
        currentCart.printDetails();
        currentBuyer->getOrders().push_back(currentCart);
        cout << "Invoice " << currentCart.getId() << " created. Please proceed to Payment Functionality." << endl;
    } else {
        orderIdCounter--;
        cout << "Cart is empty. Returning to Buyer Menu." << endl;
    }
}

void orderFunctionality() {
    cout << "\n--- ORDER MANAGEMENT ---" << endl;
    if (currentBuyer->getOrders().empty()) {
        cout << "You have no orders." << endl;
        return;
    }

    int filterChoice;
    cout << "Filter orders by status:" << endl;
    cout << "1. All" << endl;
    cout << "2. PENDING" << endl;
    cout << "3. PAID" << endl;
    cout << "4. CANCELED" << endl;
    cout << "5. COMPLETED" << endl;
    cout << "Pilihan >> ";
    if (!(cin >> filterChoice) || filterChoice < 1 || filterChoice > 5) {
        cout << "Invalid filter choice." << endl; return;
    }

    cout << "\n--- LISTING ORDERS ---" << endl;
    for (Order& order : currentBuyer->getOrders()) {
        bool match = false;
        switch (filterChoice) {
            case 1: match = true; break;
            case 2: if (order.getStatus() == Order::PENDING) match = true; break;
            case 3: if (order.getStatus() == Order::PAID) match = true; break;
            case 4: if (order.getStatus() == Order::CANCELED) match = true; break;
            case 5: if (order.getStatus() == Order::COMPLETED) match = true; break;
        }

        if (match) {
            order.printDetails();
            if (order.getStatus() == Order::PENDING) {
                cout << "    [Action: Proceed to Payment]\n";
            }
        }
    }

    paymentFunctionality(); 
}

void paymentFunctionality() {
    cout << "\n--- PAYMENT FUNCTIONALITY ---" << endl;
    int invoiceId;
    cout << "Enter Invoice ID to pay (0 to skip): ";
    if (!(cin >> invoiceId)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }

    if (invoiceId == 0) return;

    Order* orderToPay = nullptr;
    for (Order& order : currentBuyer->getOrders()) {
        if (order.getId() == invoiceId) {
            orderToPay = &order;
            break;
        }
    }

    if (!orderToPay) {
        cout << "Invoice ID not found." << endl;
        return;
    }

    if (orderToPay->getStatus() != Order::PENDING) {
        cout << "Payment rejected: Invoice is already " << orderToPay->getStatusString() << endl;
        return;
    }

    double total = orderToPay->getTotalAmount();
    cout << "Invoice Total: $" << total << endl;
    cout << "Your Balance: $" << currentBuyer->getAccount().getBalance() << endl;
    cout << "Processing payment..." << endl;

    if (!myBank.withdraw(currentBuyer->getAccount().getId(), total)) {
        cout << "Payment FAILED: Insufficient funds or bank error." << endl;
        return;
    }

    orderToPay->markAsPaid();
    cout << "✅ Order ID " << orderToPay->getId() << " successfully PAID." << endl;
    
    map<int, double> sellerPayments; 
    for (const auto& item : orderToPay->getItems()) {
        double itemTotal = item.price * item.quantity;
        sellerPayments[item.sellerId] += itemTotal;

        if (seller* s = findSeller(item.sellerId)) {
            if (Item* i = s->findItemById(item.itemId)) {
                i->discard(item.quantity); 
            }
        }
    }

    for (const auto& pair : sellerPayments) {
        int sellerId = pair.first;
        double amount = pair.second;
        
        if (seller* s = findSeller(sellerId)) {
            myBank.deposit(s->getAccount().getId(), amount); 
            
            allTransactions.emplace_back(++transactionIdCounter, 
                                         currentBuyer->getId(), 
                                         sellerId, 
                                         "Invoice #" + to_string(orderToPay->getId()) + " (" + to_string(orderToPay->getItems().size()) + " items)", 
                                         amount);
            
            cout << "    - Credited $" << amount << " to Seller ID " << sellerId << " (" << s->sellerName << ")." << endl;
        }
    }
}

void checkSpendingLastKDays(int k) {
    if (k <= 0) { cout << "K must be positive." << endl; return; }

    auto k_days_ago = chrono::system_clock::now() - chrono::hours(24 * k); 
    double totalSpending = 0.0;

    cout << "\n--- SPENDING ANALYSIS (LAST " << k << " DAYS) ---" << endl;
    for (const auto& flow : cashFlows) {
        if (flow.getAccountId() == currentBuyer->getAccount().getId() && 
            flow.getTimestamp() >= k_days_ago && 
            flow.getType() == "Debit") {
            
            totalSpending += flow.getAmount();
            flow.printInfo(); cout << endl;
        }
    }

    cout << "---------------------------------------" << endl;
    cout << "Total Debit Spending: $" << totalSpending << endl;
}

void cashFlowMenu(BankCustomer& account) {
    int choice;
    double amount;
    do {
        cout << "\n--- BANKING MENU (ID: " << account.getId() << ") ---" << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Topup (Deposit)" << endl;
        cout << "3. Withdraw" << endl;
        cout << "4. List Cash Flow (Today)" << endl;
        cout << "5. List Cash Flow (Last Month)" << endl;
        cout << "6. Back to Buyer Menu" << endl;
        
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
                cout << "Enter amount to topup: $"; 
                if (!(cin >> amount) || amount <= 0) { cout << "Invalid amount." << endl; break; }
                myBank.deposit(account.getId(), amount);
                break;
            case 3: 
                cout << "Enter amount to withdraw: $";
                if (!(cin >> amount) || amount <= 0) { cout << "Invalid amount." << endl; break; }
                myBank.withdraw(account.getId(), amount);
                break;
            case 4: 
                {
                    time_t now_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
                    struct tm *ltm = localtime(&now_t);
                    ltm->tm_hour = 0; ltm->tm_min = 0; ltm->tm_sec = 0;
                    auto today_start = chrono::system_clock::from_time_t(mktime(ltm));

                    cout << "\n--- CASH FLOW: TODAY ---" << endl;
                    int count = 0;
                    for(const auto& flow : cashFlows) {
                        if (flow.getAccountId() == account.getId() && flow.getTimestamp() >= today_start) {
                            flow.printInfo(); cout << endl;
                            count++;
                        }
                    }
                    if (count == 0) cout << "No cash flow recorded today." << endl;
                    cout << "------------------------" << endl;
                }
                break;
            case 5: 
                {
                    auto month_ago = chrono::system_clock::now() - chrono::hours(24 * 30);
                    cout << "\n--- CASH FLOW: LAST MONTH ---" << endl;
                    int count = 0;
                    for(const auto& flow : cashFlows) {
                        if (flow.getAccountId() == account.getId() && flow.getTimestamp() >= month_ago) {
                            flow.printInfo(); cout << endl;
                            count++;
                        }
                    }
                    if (count == 0) cout << "No cash flow recorded in the last month." << endl;
                    cout << "-----------------------------" << endl;
                }
                break;
            case 6: 
                return;
            default:
                cout << "Invalid option." << endl;
                break;
        }
    } while (true);
}


void itemManagementMenu() {
    cout << "\n--- ITEM MANAGEMENT (Store: " << currentSeller->sellerName << ") ---" << endl;
    int choice;
    
    cout << "\n--- Current Inventory ---" << endl;
    cout << "ID | Name | Price | Stock" << endl;
    cout << "---|------|-------|------" << endl;
    for (const auto& item : currentSeller->items) {
        cout << item.getId() << " | " << item.getName() << " | $" << item.getPrice() << " | " << item.getQuantity() << endl;
    }

    cout << "\nSelect Action:" << endl;
    cout << "1. Register New Item" << endl;
    cout << "2. Replenish (Add Stock)" << endl;
    cout << "3. Discard (Remove Stock)" << endl;
    cout << "4. Set Price" << endl;
    cout << "5. Back to Seller Menu" << endl;

    cout << "Pilihan >> ";
    if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }

    int itemId, amount;
    double price;
    string name;

    switch (choice) {
        case 1: 
            cout << "Enter Item Name: "; cin.ignore(); getline(cin, name);
            cout << "Enter Initial Quantity: "; 
            if (!(cin >> amount) || amount <= 0) { cout << "Invalid quantity." << endl; break; }
            cout << "Enter Price: $"; 
            if (!(cin >> price) || price <= 0) { cout << "Invalid price." << endl; break; }
            currentSeller->addNewItem(++itemIdCounter, name, amount, price);
            currentSeller->makeItemVisibleToCustomer(itemIdCounter);
            cout << "✅ Item '" << name << "' added with ID: " << itemIdCounter << endl;
            break;
        case 2: 
            cout << "Enter Item ID to replenish: "; 
            if (!(cin >> itemId)) break;
            cout << "Enter amount to add: "; 
            if (!(cin >> amount) || amount <= 0) { cout << "Invalid amount." << endl; break; }
            if (currentSeller->replenishItem(itemId, amount)) {
                cout << "✅ Replenished " << amount << " units to Item ID " << itemId << endl;
            } else { cout << "Item ID not found." << endl; }
            break;
        case 3: 
            cout << "Enter Item ID to discard: "; 
            if (!(cin >> itemId)) break;
            cout << "Enter amount to discard: "; 
            if (!(cin >> amount) || amount <= 0) { cout << "Invalid amount." << endl; break; }
            if (currentSeller->discardItem(itemId, amount)) {
                cout << "✅ Discarded " << amount << " units from Item ID " << itemId << endl;
            } else { cout << "Discard failed (Insufficient stock or Item ID not found)." << endl; }
            break;
        case 4: 
            cout << "Enter Item ID to set price: "; 
            if (!(cin >> itemId)) break;
            cout << "Enter new price: $"; 
            if (!(cin >> price) || price <= 0) { cout << "Invalid price." << endl; break; }
            currentSeller->setItemPrice(itemId, price);
            cout << "✅ Price for Item ID " << itemId << " set to $" << price << endl;
            break;
        default:
            break;
    }
}

void discoverTopItemsPerMonth() {
    int k;
    cout << "Enter K (Top number of items): ";
    if (!(cin >> k) || k <= 0) { cout << "Invalid input." << endl; return; }

    auto month_ago = chrono::system_clock::now() - chrono::hours(24 * 30);
    map<string, int> itemSaleCounts;

    for (const auto& t : allTransactions) {
        if (t.getTimestamp() >= month_ago && t.getSellerId() == currentSeller->getId()) {
            itemSaleCounts[t.getItemName()]++;
        }
    }

    vector<pair<int, string>> sortedItems;
    for (const auto& pair : itemSaleCounts) {
        sortedItems.push_back({pair.second, pair.first});
    }

    sort(sortedItems.rbegin(), sortedItems.rend());

    cout << "\n--- TOP " << k << " POPULAR ITEMS (LAST MONTH) ---" << endl;
    if (sortedItems.empty()) { cout << "No sales data found this month." << endl; return; }
    
    for (int i = 0; i < min((int)sortedItems.size(), k); ++i) {
        cout << i + 1 << ". " << sortedItems[i].second 
             << " (" << sortedItems[i].first << " transactions)" << endl;
    }
    cout << "------------------------------------------------" << endl;
}

void discoverLoyalCustomerPerMonth() {
    auto month_ago = chrono::system_clock::now() - chrono::hours(24 * 30);
    map<int, int> buyerTransactionCounts;

    for (const auto& t : allTransactions) {
        if (t.getTimestamp() >= month_ago && t.getSellerId() == currentSeller->getId()) {
            buyerTransactionCounts[t.getBuyerId()]++;
        }
    }

    vector<pair<int, int>> sortedBuyers; 
    for (const auto& pair : buyerTransactionCounts) {
        sortedBuyers.push_back({pair.second, pair.first});
    }

    sort(sortedBuyers.rbegin(), sortedBuyers.rend());

    cout << "\n--- LOYAL CUSTOMERS (LAST MONTH) ---" << endl;
    if (sortedBuyers.empty()) { cout << "No loyal customers found this month." << endl; return; }

    for (int i = 0; i < min((int)sortedBuyers.size(), 5); ++i) {
        int buyerId = sortedBuyers[i].second;
        int count = sortedBuyers[i].first;
        
        string name = "Unknown";
        if (Buyer* b = findBuyer(buyerId)) { name = b->getName(); }

        cout << i + 1 << ". ID " << buyerId << " (" << name << ") - " 
             << count << " total orders" << endl;
    }
    cout << "-------------------------------------" << endl;
}


void storeCapabilitiesMenu() {
    int choice;
    do {
        cout << "\n--- STORE CAPABILITIES ---" << endl;
        cout << "1. List all transaction of the latest K days" << endl; 
        cout << "2. List all paid transaction but yet to be completed" << endl;
        cout << "3. List all most M frequent item transactions" << endl;
        cout << "4. List all most active buyer (by total transactions)" << endl;
        cout << "5. List all most active sellers (by total transactions)" << endl;
        cout << "6. Back to Main Menu" << endl;
        
        cout << "Pilihan >> ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        
        int k, m;
        switch (choice) {
            case 1:
                cout << "Enter K (number of latest days, 7 for a week): "; 
                if (!(cin >> k) || k <= 0) { cout << "Invalid input." << endl; break; }
                listRecentTransactions(k);
                break;
            case 2:
                listUncompletedTransactions();
                break;
            case 3:
                cout << "Enter M (number of top items): "; 
                if (!(cin >> m) || m <= 0) { cout << "Invalid input." << endl; break; }
                listMostFrequentItems(m);
                break;
            case 4:
                listMostActiveBuyers();
                break;
            case 5:
                listMostActiveSellers();
                break;
            case 6:
                return;
            default:
                cout << "Invalid option." << endl;
        }
    } while (true);
}

void bankAnalysisMenu() {
    int choice;
    do {
        cout << "\n--- BANK ANALYSIS ---" << endl;
        cout << "1. List all transaction within a week (7 days)" << endl;
        cout << "2. List all bank customers" << endl;
        cout << "3. List all dormant accounts (no activity within a month)" << endl;
        cout << "4. List N top users that conduct most transaction for today" << endl;
        cout << "5. Back to Main Menu" << endl;

        cout << "Pilihan >> ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        
        int n;
        switch (choice) {
            case 1:
                listRecentTransactions(7); 
                break;
            case 2:
                myBank.listAllCustomers();
                break;
            case 3:
                myBank.listDormantAccounts();
                break;
            case 4:
                cout << "Enter N (number of top users): "; 
                if (!(cin >> n) || n <= 0) { cout << "Invalid input." << endl; break; }
                myBank.listTopUsersToday(n);
                break;
            case 5:
                return;
            default:
                cout << "Invalid option." << endl;
        }
    } while (true);
}

void listRecentTransactions(int k) {
    if (k <= 0) { cout << "K must be positive." << endl; return; }

    auto k_days_ago = chrono::system_clock::now() - chrono::hours(24 * k); 

    cout << "\n--- TRANSACTIONS OF LAST " << k << " DAYS ---" << endl;
    int count = 0;
    for (const auto& t : allTransactions) {
        if (t.getTimestamp() >= k_days_ago) {
            cout << "ID: " << t.getId() 
                 << ", Buyer: " << t.getBuyerId() 
                 << ", Seller: " << t.getSellerId()
                 << ", Item: " << t.getItemName()
                 << ", Amount: $" << t.getAmount()
                 << ", Status: " << t.getStatusString() << endl;
            count++;
        }
    }
    if (count == 0) cout << "No transactions found in the last " << k << " days." << endl;
}

void listUncompletedTransactions() {
    cout << "\n--- PAID BUT UNCOMPLETED TRANSACTIONS ---" << endl;
    int count = 0;
    for (const auto& t : allTransactions) {
        if (t.getStatus() == Transaction::PAID) {
             cout << "ID: " << t.getId() 
                 << ", Buyer: " << t.getBuyerId() 
                 << ", Seller: " << t.getSellerId()
                 << ", Item: " << t.getItemName()
                 << ", Amount: $" << t.getAmount()
                 << endl;
            count++;
        }
    }
    if (count == 0) cout << "No uncompleted transactions found." << endl;
}

void listMostFrequentItems(int m) {
    map<string, int> itemCounts;
    for (const auto& t : allTransactions) {
        itemCounts[t.getItemName()]++;
    }

    vector<pair<int, string>> sortedItems;
    for (const auto& pair : itemCounts) {
        sortedItems.push_back({pair.second, pair.first});
    }

    sort(sortedItems.rbegin(), sortedItems.rend());

    cout << "\n--- TOP " << m << " MOST FREQUENT ITEMS (All Time) ---" << endl;
    for (int i = 0; i < min((int)sortedItems.size(), m); ++i) {
        cout << i + 1 << ". " << sortedItems[i].second 
             << " (" << sortedItems[i].first << " transactions)" << endl;
    }
}

void listMostActiveUsers(const string& type, int (Transaction::*getIdFunc)() const) {
    map<int, int> userCounts;
    for (const auto& t : allTransactions) {
        userCounts[(t.*getIdFunc)()]++;
    }

    vector<pair<int, int>> sortedUsers;
    for (const auto& pair : userCounts) {
        sortedUsers.push_back({pair.second, pair.first}); 
    }

    sort(sortedUsers.rbegin(), sortedUsers.rend());

    cout << "\n--- MOST ACTIVE " << type << " (Total Transactions) ---" << endl;
    for (int i = 0; i < min((int)sortedUsers.size(), 5); ++i) { 
        int userId = sortedUsers[i].second;
        int count = sortedUsers[i].first;

        string name = "Unknown";
        if (Buyer* b = findBuyer(userId)) { name = b->getName(); }

        cout << i + 1 << ". ID " << userId << " (" << name << ") - " 
             << count << " transactions\n";
    }
}

void listMostActiveBuyers() {
    listMostActiveUsers("BUYERS", &Transaction::getBuyerId);
}

void listMostActiveSellers() {
    listMostActiveUsers("SELLERS", &Transaction::getSellerId);
}
