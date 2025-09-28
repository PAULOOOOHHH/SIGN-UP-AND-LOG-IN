#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>

// File where user data is stored (username:hashed_password)
const std::string USERS_FILE = "users.txt";

// Trim helpers
static inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch){ return !std::isspace(ch); }));
    return s;
}
static inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
    return s;
}
static inline std::string trim(std::string s) { return ltrim(rtrim(std::move(s))); }

// Simple (non-cryptographic) hash for demo only
std::size_t simpleHash(const std::string &s) {
    // std::hash is NOT cryptographically secure; demo only
    return std::hash<std::string>{}(s);
}

// Load users from file into map: username -> hashed string
std::unordered_map<std::string, std::string> loadUsers() {
    std::unordered_map<std::string, std::string> users;
    std::ifstream fin(USERS_FILE);
    if (!fin.is_open()) return users;

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        auto pos = line.find(':');
        if (pos == std::string::npos) continue;
        std::string user = trim(line.substr(0, pos));
        std::string hash = trim(line.substr(pos + 1));
        if (!user.empty() && !hash.empty()) users[user] = hash;
    }
    return users;
}

// Save a single user entry (append)
bool saveUser(const std::string &username, const std::string &hashed) {
    std::ofstream fout(USERS_FILE, std::ios::app);
    if (!fout.is_open()) return false;
    fout << username << ":" << hashed << "\n";
    return true;
}

bool usernameExists(const std::unordered_map<std::string, std::string> &users, const std::string &username) {
    return users.find(username) != users.end();
}

void signUp() {
    auto users = loadUsers();

    std::string username;
    std::cout << "Choose a username: ";
    std::getline(std::cin, username);
    username = trim(username);
    if (username.empty()) { std::cout << "Username cannot be empty.\n"; return; }

    if (usernameExists(users, username)) {
        std::cout << "Username already taken. Try another.\n";
        return;
    }

    std::string password, password2;
    std::cout << "Choose a password: ";
    std::getline(std::cin, password);
    std::cout << "Confirm password: ";
    std::getline(std::cin, password2);

    if (password != password2) {
        std::cout << "Passwords do not match.\n";
        return;
    }
    if (password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return;
    }

    // Hash password (demo). In real apps use bcrypt/argon2
    std::size_t h = simpleHash(password);
    std::ostringstream oss;
    oss << h;
    if (!saveUser(username, oss.str())) {
        std::cout << "Failed to save user. Check file permissions.\n";
        return;
    }
    std::cout << "Sign-up successful! You can now log in.\n";
}

void logIn() {
    auto users = loadUsers();
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    username = trim(username);
    if (!usernameExists(users, username)) {
        std::cout << "No such user.\n";
        return;
    }
    std::cout << "Password: ";
    std::getline(std::cin, password);

    std::size_t h = simpleHash(password);
    std::ostringstream oss;
    oss << h;
    if (users[username] == oss.str()) {
        std::cout << "Login successful! Welcome, " << username << ".\n";
    } else {
        std::cout << "Invalid password.\n";
    }
}

int main() {
    while (true) {
        std::cout << "=== Simple Auth Demo ===\n";
        std::cout << "1) Sign Up\n";
        std::cout << "2) Log In\n";
        std::cout << "3) Exit\n";
        std::cout << "Choose an option: ";

        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") signUp();
        else if (choice == "2") logIn();
        else if (choice == "3" || choice == "q" || choice == "Q") { std::cout << "Bye!\n"; break; }
        else std::cout << "Invalid option.\n";

        std::cout << "\n";
    }
    return 0;
}