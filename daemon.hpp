#ifndef DAEMON_HPP
#define DAEMON_HPP

#include "tintin_reporter.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <cerrno>

 #include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <csignal>

// Forward declaration to ensure the type is known when used as a reference.
class Tintin_reporter;

class Daemon
{
    public:
        void start(Tintin_reporter &reporter);
        void stop();
};

#endif