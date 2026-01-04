#pragma once
#include <vector>
#include <string>
#include "User.h"

class UserManager {
public:
    UserManager();
    
    /** Task 1: Register a new user */
    std::string registerUser(std::string fullName, std::string email, std::string password);
    
    /** Task 2: Login verification */
    User* login(std::string username, std::string password);

    /** Task 3: Credential reminder */
    std::string findUsernameByEmail(std::string email);

private:
    std::vector<User> users;
    const std::string userFile = "users_db.csv";
    
    void loadUsers();
    void saveUser(const User& user);
    std::string generateUniqueID();
};