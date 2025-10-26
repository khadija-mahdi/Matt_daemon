#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <cstring>
#include <map>

class Authentication
{
private:
    std::map<std::string, std::string> credentials;
    std::string encryptionKey;

public:
    Authentication();
    Authentication(const Authentication &other);
    Authentication &operator=(const Authentication &other);
    ~Authentication();

    bool verifyUser(const std::string &username, const std::string &password);
};

#endif