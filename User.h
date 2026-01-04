#pragma once
#include <string>
#include <vector>
#include "Wallet.h"

struct Transaction {
    std::string timestamp;
    std::string product;
    std::string type; // "deposit", "withdraw", "ask", or "bid"
    double amount;
    double price;
};

class User {
public:
    User(std::string _username, std::string _fullName, std::string _email, size_t _passwordHash);

    std::string username;
    std::string fullName;
    std::string email;
    size_t passwordHash;
    Wallet wallet; 
    
    // Task 3: Transaction history storage
    std::vector<Transaction> transactions;
    void addTransaction(std::string timestamp, std::string product, std::string type, double amount, double price);
};