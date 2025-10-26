#include "server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <signal.h>

Server::Server(Tintin_reporter &reporter)
    : reporter(reporter), running(false), start_time(false),
      client_count(0), total_messages(0), listen_fd(-1)
{
    std::memset(buffer, 0, sizeof(buffer));
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = -1;
    }
}

void Server::start()
{
    if (running.load())
        return;

    listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        reporter.writeLog("Socket creation failed", "ERROR");
        return;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        reporter.writeLog("Bind failed", "ERROR");
        ::close(listen_fd);
        return;
    }

    if (listen(listen_fd, 10) < 0)
    {
        reporter.writeLog("Listen failed", "ERROR");
        ::close(listen_fd);
        return;
    }

    running = true;
    start_time = true;

    fd_set readfds;
    int max_sd;

    while (running.load())
    {
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        max_sd = listen_fd;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int activity = select(max_sd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0 && running.load())
        {
            continue;
        }

        // Check for new connection
        if (FD_ISSET(listen_fd, &readfds))
        {
            int new_socket = accept(listen_fd, NULL, NULL);
            if (new_socket < 0)
            {
                if (!running.load())
                    break;
                continue;
            }

            // Find empty slot
            bool added = false;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == -1)
                {
                    client_sockets[i] = new_socket;
                    ++client_count;
                    added = true;
                    break;
                }
            }

            if (!added)
            {
                // Max clients reached
                ::close(new_socket);
            }
        }

        // Check for I/O on existing clients
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];

            if (sd > 0 && FD_ISSET(sd, &readfds))
            {
                // IMPORTANT: Clear buffer before reading
                std::memset(buffer, 0, sizeof(buffer));
                
                ssize_t res = recv(sd, buffer, sizeof(buffer) - 1, 0);

                if (res <= 0)
                {
                    // Client disconnected or error
                    closeClient(i);
                    continue;
                }

                buffer[res] = '\0';
                std::string msg(buffer);

                // Strip trailing newlines and carriage returns
                while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
                {
                    msg.pop_back();
                }

                if (!msg.empty())
                {
                    if (msg == "quit")
                    {
                        reporter.writeLog("Request quit.", "INFO");
                        running = false;
                        break;
                    }

                    // Log the message
                    reporter.writeLog("User input: " + msg, "LOG");
                    ++total_messages;
                }
            }
        }
    }
    
    // Cleanup all connections when exiting
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        closeClient(i);
    }
}

void Server::closeClient(int index)
{
    if (index >= 0 && index < MAX_CLIENTS && client_sockets[index] != -1)
    {
        ::close(client_sockets[index]);
        client_sockets[index] = -1;
    }
}

void Server::stop()
{
    if (!running.load())
        return;

    running = false;

    // Close all client connections
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        closeClient(i);
    }

    // Close listening socket
    if (listen_fd >= 0)
    {
        ::close(listen_fd);
        listen_fd = -1;
    }

    reporter.writeLog("Server stopped", "INFO");
}

Server::~Server()
{
    stop();
}