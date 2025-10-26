#ifndef REMOTESHELL_HPP
#define REMOTESHELL_HPP

#include <string>
#include "../server.hpp"
#include "Authentication.hpp"

class RemoteShell {
private:
    Server &server;
    int clientSocket;
    bool isAuthenticated;
    
    void connectToServer();
    void handleCommand(const std::string &command);
    std::string sendAndReceive(const std::string &message);
    
public:
    RemoteShell(Server &server);
    RemoteShell(const RemoteShell &other);
    RemoteShell &operator=(const RemoteShell &other);
    ~RemoteShell();
    
    void createRemoteShellSession();
};

#endif