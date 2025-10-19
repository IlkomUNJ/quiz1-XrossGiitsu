#include "transaction.h"
#include <iostream>
#include <ctime>
#include <iomanip>

using namespace std;

void Transaction::printTimestamp() const {
    time_t tt = chrono::system_clock::to_time_t(timestamp);
    struct tm * ptm = localtime(&tt);
    
    cout << "Date/Time: " << put_time(ptm, "%Y-%m-%d %H:%M:%S") << endl;
}
