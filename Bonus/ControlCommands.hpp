#ifndef CONTROLCOMMANDS_HPP
#define CONTROLCOMMANDS_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <cstdlib>
#include <time.h>

class ControlCommands
{
private:
    time_t startTime;
    int messageCount;
    int clientCount;

public:
    ControlCommands();
    ControlCommands(const time_t &startTime, const int &messageCount, const int &clientCount);
    ControlCommands(const ControlCommands &other);
    ControlCommands &operator=(const ControlCommands &other);
    ~ControlCommands();

    bool isCommand(const std::string &input);
    std::string executeCommand(const std::string &input, time_t startTime, int messageCount, int activeClients);
    

};

#endif
