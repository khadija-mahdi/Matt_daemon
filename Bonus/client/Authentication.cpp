#include "Authentication.hpp"
#include <sys/stat.h>
#include <unistd.h>

Authentication::Authentication()
{
}

Authentication::Authentication(const Authentication &other)
{
    encryptionKey = other.encryptionKey;
    credentials = other.credentials;
}

Authentication &Authentication::operator=(const Authentication &other)
{
    if (this != &other)
    {
        credentials = other.credentials;
    }
    return *this;
}

Authentication::~Authentication() = default;


bool Authentication::verifyUser(const std::string &username, const std::string &password)
{
    std::string env_user = "test";
    std::string env_pass = "test";
    if (!env_user.empty() && !env_pass.empty())
    {
        if (username == env_user && password == env_pass)
            return true;
    }
    return false;
}

