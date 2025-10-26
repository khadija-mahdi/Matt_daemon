#include "server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <signal.h>
#include "helper.hpp"

// Add these at the top of server.cpp, after includes

bool isHttpRequest(const std::string &data)
{
    return (data.size() >= 4 &&
            (data.substr(0, 4) == "GET " ||
             data.substr(0, 5) == "POST " ||
             data.substr(0, 5) == "HEAD "));
}



void sendHttpResponse(int client_fd, const std::string &content,
                      const std::string &contentType = "text/html")
{
    std::stringstream response;

    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << contentType << "; charset=utf-8\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Cache-Control: no-cache\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;

    std::string resp = response.str();
    send(client_fd, resp.c_str(), resp.length(), 0);
}

void sendStatusJSON(int client_fd, time_t start_time, int client_count, int total_messages)
{
    time_t now = time(NULL);
    time_t uptime = now - start_time;

    std::stringstream json;
    json << "{\n";
    json << "  \"status\": \"running\",\n";
    json << "  \"uptime\": " << uptime << ",\n";
    json << "  \"active\": " << client_count << ",\n";
    json << "  \"max\": 3,\n";
    json << "  \"active_connections\": \"" << client_count << "/3\",\n";
    json << "  \"total_messages\": " << total_messages << ",\n";
    json << "  \"timestamp\": " << now << "\n";
    json << "}";

    sendHttpResponse(client_fd, json.str(), "application/json");
}

Server::Server(Tintin_reporter &reporter)
    : reporter(reporter), running(false), start_time(0),
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
    start_time = time(NULL);
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
                ::close(new_socket);
            }
        }
        std::string response;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];

            if (sd > 0 && FD_ISSET(sd, &readfds))
            {
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
                // ===== CHECK IF IT'S AN HTTP REQUEST =====
                if (isHttpRequest(msg))
                {
                    reporter.writeLog("HTTP request received", "INFO");

                    // Check if requesting status JSON
                    if (msg.find("GET /status") != std::string::npos)
                    {
                        sendStatusJSON(sd, start_time, client_count.load(), total_messages.load());
                        reporter.writeLog("Sent status JSON", "INFO");
                    }
                    // Otherwise send dashboard HTML
                    else
                    {
                        std::string html = loadDashboardHTML();
                        sendHttpResponse(sd, html);
                        reporter.writeLog("Sent dashboard HTML", "INFO");
                    }

                    // Close connection after HTTP response
                    closeClient(i);
                    --client_count;
                    continue;
                }

                if (!msg.empty())
                {
                    msg.erase(msg.find_last_not_of("\r\n") + 1);
                    if (msg == "quit")
                    {
                        reporter.writeLog("Request quit.", "INFO");
                        running = false;
                        break;
                    }

                    std::string response;
                    reporter.writeLog("Received command: " + msg, "INFO");
                    if (msg == "help")
                    {
                        response = "Available commands:\n";
                        response += "  status  - Show daemon status\n";
                        response += "  stats   - Show statistics\n";
                        response += "  uptime  - Show how long daemon has been running\n";
                        response += "  clients - Show connected clients\n";
                        response += "  help    - Show this help\n";
                        response += "  quit    - Stop daemon\n";

                        send(sd, response.c_str(), response.length(), 0);
                        reporter.writeLog("Command: help", "INFO");
                    }
                    else if (msg == "status")
                    {
                        time_t now = time(NULL);
                        time_t uptime = now - start_time.load();

                        std::stringstream ss;
                        ss << "Daemon Status:\n";
                        ss << "  Uptime: " << uptime << " seconds\n";
                        ss << "  Active connections: " << client_count.load() << "/3\n";
                        ss << "  Total messages: " << total_messages.load() << "\n";

                        response = ss.str();
                        send(sd, response.c_str(), response.length(), 0);
                        reporter.writeLog("Command: status", "INFO");
                    }
                    else if (msg == "stats")
                    {
                        std::stringstream ss;
                        ss << "Statistics:\n";
                        ss << "  Total messages received: " << total_messages.load() << "\n";
                        ss << "  Total clients connected: " << client_count.load() << "\n";
                        ss << "  Errors logged: 0\n";

                        response = ss.str();
                        send(sd, response.c_str(), response.length(), 0);
                        reporter.writeLog("Command: stats", "INFO");
                    }
                    else if (msg == "uptime")
                    {
                        time_t now = time(NULL);
                        time_t uptime = now - start_time.load();

                        int hours = uptime / 3600;
                        int minutes = (uptime % 3600) / 60;
                        int seconds = uptime % 60;

                        std::stringstream ss;
                        ss << "Uptime: " << hours << "h " << minutes << "m " << seconds << "s\n";

                        response = ss.str();
                        send(sd, response.c_str(), response.length(), 0);
                        reporter.writeLog("Command: uptime", "INFO");
                    }
                    else if (msg == "clients")
                    {
                        std::stringstream ss;
                        ss << "Connected clients: " << client_count.load() << "/3\n";

                        response = ss.str();
                        send(sd, response.c_str(), response.length(), 0);
                        reporter.writeLog("Command: clients", "INFO");
                    }
                    else
                    {
                        // Regular message - just log it
                        reporter.writeLog("User input: " + msg, "LOG");
                        response = "Message logged\n";
                        send(sd, response.c_str(), response.length(), 0);
                    }

                    ++total_messages;
                }
            }
        }
    }

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