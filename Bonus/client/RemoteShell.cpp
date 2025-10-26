#include "RemoteShell.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

RemoteShell::RemoteShell(Server &server) 
    : server(server), clientSocket(-1), isAuthenticated(false) {
}

RemoteShell::RemoteShell(const RemoteShell &other) 
    : server(other.server), clientSocket(-1), isAuthenticated(other.isAuthenticated) {
}

RemoteShell &RemoteShell::operator=(const RemoteShell &other) {
    if (this == &other)
        return *this;
    isAuthenticated = other.isAuthenticated;
    return *this;
}

RemoteShell::~RemoteShell() {
    if (clientSocket >= 0) {
        close(clientSocket);
    }
}

void RemoteShell::connectToServer() {
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error: Cannot create socket" << std::endl;
        return;
    }
    
    // Server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(4242);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Connect to daemon
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error: Cannot connect to Matt_daemon on port 4242" << std::endl;
        std::cerr << "Make sure the daemon is running: sudo ./Matt_daemon" << std::endl;
        close(clientSocket);
        clientSocket = -1;
        return;
    }
    
    std::cout << "✓ Connected to Matt_daemon successfully!" << std::endl;
}

std::string RemoteShell::sendAndReceive(const std::string &message) {
    if (clientSocket < 0) {
        return "Error: Not connected to server";
    }
    
    // Send message with newline
    std::string msg = message + "\n";
    ssize_t sent = send(clientSocket, msg.c_str(), msg.length(), 0);
    
    if (sent < 0) {
        return "Error: Failed to send message";
    }
    
    // Receive response
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (received > 0) {
        buffer[received] = '\0';
        return std::string(buffer);
    } else if (received == 0) {
        return "Server closed connection";
    } else {
        return "Error receiving response";
    }
}

void RemoteShell::handleCommand(const std::string &command) {
    if (command.empty()) {
        return;
    }
    
    // Local commands (don't send to server)
    if (command == "exit" || command == "logout") {
        std::cout << "Closing remote shell session..." << std::endl;
        if (clientSocket >= 0) {
            close(clientSocket);
            clientSocket = -1;
        }
        isAuthenticated = false;
        return;
    }
    
    if (command == "clear") {
        system("clear");
        return;
    }
    
    // Send command to daemon and get response
    std::string response = sendAndReceive(command);
    
    // Display response
    if (!response.empty()) {
        std::cout << response;
        if (response.back() != '\n') {
            std::cout << std::endl;
        }
    }
    
    // If quit command, mark as not authenticated
    if (command == "quit") {
        std::cout << "Daemon is shutting down..." << std::endl;
        isAuthenticated = false;
        if (clientSocket >= 0) {
            close(clientSocket);
            clientSocket = -1;
        }
    }
}

void RemoteShell::createRemoteShellSession() {
    std::cout << "\n=== Matt_daemon Remote Shell ===" << std::endl;
    std::cout << "Version 1.0\n" << std::endl;
    
    bool sessionActive = true;
    
    while (sessionActive) {
        // Check if authenticated
        if (!isAuthenticated) {
            std::cout << "You must authenticate to use the remote shell." << std::endl;
            std::cout << "Type 'login' to authenticate or 'exit' to quit.\n" << std::endl;
            
            std::string input;
            std::cout << "remote-shell> ";
            std::getline(std::cin, input);
            
            // Trim whitespace
            while (!input.empty() && (input.back() == ' ' || input.back() == '\t')) {
                input.pop_back();
            }
            
            if (input.empty()) {
                continue;
            }
            
            if (input == "exit") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            
            if (input == "login") {
                std::string username, password;
                
                std::cout << "\n--- Authentication Required ---" << std::endl;
                std::cout << "Username: ";
                std::getline(std::cin, username);
                
                std::cout << "Password: ";
                std::getline(std::cin, password);
                
                Authentication auth;
                if (auth.verifyUser(username, password)) {
                    std::cout << "\n✓ Authentication successful! Welcome, " << username << "!" << std::endl;
                    
                    // Connect to daemon
                    connectToServer();
                    
                    if (clientSocket >= 0) {
                        isAuthenticated = true;
                        std::cout << "\nType 'help' for available commands or 'exit' to quit.\n" << std::endl;
                    } else {
                        std::cout << "Failed to connect to daemon. Please try again." << std::endl;
                    }
                } else {
                    std::cout << "\n✗ Authentication failed! Invalid username or password.\n" << std::endl;
                }
            } else {
                std::cout << "Unknown command. Type 'login' to authenticate.\n" << std::endl;
            }
        } else {
            // Authenticated - handle commands
            std::string command;
            std::cout << "remote-shell> ";
            std::getline(std::cin, command);
            
            // Trim whitespace
            while (!command.empty() && (command.back() == ' ' || command.back() == '\t')) {
                command.pop_back();
            }
            
            if (command.empty()) {
                continue;
            }
            
            if (command == "exit" || command == "logout") {
                handleCommand(command);
                sessionActive = false;
                break;
            }
            
            handleCommand(command);
            
            // Check if still connected
            if (clientSocket < 0) {
                isAuthenticated = false;
            }
        }
    }
    
    std::cout << "\nRemote shell session ended." << std::endl;
}