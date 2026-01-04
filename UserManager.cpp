#include "UserManager.h"
#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <random>
#include <functional>

UserManager::UserManager() {
    loadUsers();
}

void UserManager::loadUsers() {
    std::ifstream file(userFile);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
            if (tokens.size() == 4) {
                // Task 1.4: Parse stored hashed password
                size_t hash = std::stoull(tokens[3]);
                users.emplace_back(tokens[0], tokens[1], tokens[2], hash);
            }
        }
        file.close();
    }
}

std::string UserManager::registerUser(std::string fullName, std::string email, std::string password) {
    // Task 1.5: Avoid duplicates
    for (const auto& u : users) {
        if (u.email == email) return "EXISTS";
    }

    // Task 1.2: Generate 10-digit ID
    std::string id = generateUniqueID();
    
    // Task 1.3: Hash password
    size_t hash = std::hash<std::string>{}(password);
    
    User newUser{id, fullName, email, hash};
    users.push_back(newUser);
    saveUser(newUser);
    
    return id;
}

User* UserManager::login(std::string username, std::string password) {
    // Task 2.1: Compare input hash against stored record
    size_t inputHash = std::hash<std::string>{}(password);
    for (auto& u : users) {
        if (u.username == username && u.passwordHash == inputHash) {
            return &u;
        }
    }
    return nullptr;
}

std::string UserManager::findUsernameByEmail(std::string email) {
    for (const auto& u : users) {
        if (u.email == email) return u.username;
    }
    return "";
}

std::string UserManager::generateUniqueID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1000000000LL, 9999999999LL);
    return std::to_string(dis(gen));
}

void UserManager::saveUser(const User& user) {
    std::ofstream file(userFile, std::ios::app);
    file << user.username << "," << user.fullName << "," << user.email << "," << user.passwordHash << "\n";
    file.close();
}