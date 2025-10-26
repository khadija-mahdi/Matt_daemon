#ifndef SERVER_HPP
#define SERVER_HPP

#include "tintin_reporter.hpp"
#include <atomic>
#include <ctime>
#include <vector>
#include <sys/select.h>

#define PORT 4242
#define MAX_CLIENTS 3

class Server {
private:
    Tintin_reporter &reporter;
    std::atomic<bool> running;
    std::atomic<bool> start_time;
    std::atomic<int> client_count;
    std::atomic<int> total_messages;
    
    int listen_fd;
    int client_sockets[MAX_CLIENTS];
    char buffer[1024];
    
    void handleClient(int client_fd);
    void closeClient(int index);
    
public:
    Server(Tintin_reporter &reporter);
    ~Server();
    
    void start();
    void stop();
    
    int getClientCount() const { return client_count.load(); }
    int getTotalMessages() const { return total_messages.load(); }
};

#endif