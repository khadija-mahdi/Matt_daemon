// #ifndef DAEMON_HPP
// #define DAEMON_HPP

// #include "tintin_reporter.hpp"


// Forward declaration to ensure the type is known when used as a reference.
// class Tintin_reporter;

// class Daemon
// {
//     public:
//         void start(Tintin_reporter &reporter);
//         void stop();
// };

// #endif


#ifndef DAEMON_HPP
#define DAEMON_HPP

#include "tintin_reporter.hpp"
#include <atomic>
#include <string>

class Daemon
{
    private:
        std::atomic<bool> running;
        std::string lock_file_path;
        Tintin_reporter &reporter;
        int lock_fd;
        
    public:
        Daemon(Tintin_reporter &reporter);
        void start();
        void stop();
        static Daemon* instance;
        void setupSignalHandlers();
        void signalHandler(int signal);
        static void staticSignalHandler(int signal);
};

#endif