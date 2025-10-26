#include "ControlCommands.hpp"
#include <sstream>

ControlCommands::ControlCommands()
    : startTime(time(nullptr)), messageCount(0), clientCount(0)
{
}

ControlCommands::ControlCommands(const ControlCommands &other)
{
    startTime = other.startTime;
    messageCount = other.messageCount;
    clientCount = other.clientCount;
}

ControlCommands &ControlCommands::operator=(const ControlCommands &other)
{
    if (this == &other)
        return *this;
    startTime = other.startTime;
    messageCount = other.messageCount;
    clientCount = other.clientCount;
    return *this;
}

ControlCommands::ControlCommands(const time_t &startTime, const int &messageCount, const int &clientCount)
    : startTime(startTime), messageCount(messageCount), clientCount(clientCount)
{
}

ControlCommands::~ControlCommands()
{
}

bool ControlCommands::isCommand(const std::string &input)
{
    if (input == "status" ||
        input == "stats" ||
        input == "uptime" ||
        input == "clients" ||
        input == "help" ||
        input == "quit")
    {
        return true;
    }
    return false;
}

std::string ControlCommands::executeCommand(const std::string &input, time_t startTime, int messageCount, int activeClients)
{
    std::stringstream response;

    if (input == "help")
    {
        response << "Available commands:\n"
                 << "  status  - Show daemon status\n"
                 << "  stats   - Show statistics\n"
                 << "  uptime  - Show uptime\n"
                 << "  clients - Show connected clients\n"
                 << "  help    - Show this help\n"
                 << "  quit    - Stop daemon\n";
    }
    else if (input == "status")
    {
        time_t elapsed = time(NULL) - startTime;
        int h = elapsed / 3600, m = (elapsed % 3600) / 60, s = elapsed % 60;
        response << "Daemon Status:\n"
                 << "  Uptime: " << h << "h " << m << "m " << s << "s\n"
                 << "  Active connections: " << activeClients << "/3\n"
                 << "  Messages processed: " << messageCount << "\n";
    }
    else if (input == "stats")
    {
        response << "Statistics:\n"
                 << "  Total messages: " << messageCount << "\n"
                 << "  Connected clients: " << activeClients << "\n";
    }
    else if (input == "uptime")
    {
        time_t elapsed = time(NULL) - startTime;
        int h = elapsed / 3600, m = (elapsed % 3600) / 60, s = elapsed % 60;
        response << "Uptime: " << h << "h " << m << "m " << s << "s\n";
    }
    else if (input == "clients")
    {
        response << "Connected clients: " << activeClients << "/3\n";
    }

    return response.str();
}

