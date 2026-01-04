#include "User.h"
#include <fstream>

User::User(std::string _username, std::string _fullName, std::string _email, size_t _passwordHash)
: username(_username), fullName(_fullName), email(_email), passwordHash(_passwordHash) {}

void User::addTransaction(std::string timestamp, std::string product, std::string type, double amount, double price)
{
    transactions.push_back({timestamp, product, type, amount, price});
    
    // Task 3.2: Log to temporary CSV file
    std::ofstream file("transactions_log.csv", std::ios::app);
    if (file.is_open())
    {
        file << username << "," << timestamp << "," << product << "," << type << "," << amount << "," << price << "\n";
        file.close();
    }
}