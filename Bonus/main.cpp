#include "matt_daemon.hpp"
#include "tintin_reporter.hpp"
#include "daemon.hpp"
#include <cstring>

void createLargeLog()
{
    const char *primary = "/var/log/matt_daemon/matt_daemon.log";
    const char *fallback = "/tmp/matt_daemon.log";

    std::cout << "Creating large log file for testing..." << std::endl;

    std::ofstream test(primary, std::ios::out | std::ios::binary);
    if (!test.is_open())
    {
        int err = errno;
        std::cerr << "Failed to open '" << primary << "' for writing: " << strerror(err) << "\n";
        std::cerr << "Falling back to '" << fallback << "'\n";
        test.clear();
        test.open(fallback, std::ios::out | std::ios::binary);
        if (!test.is_open())
        {
            int err2 = errno;
            std::cerr << "Failed to open fallback file '" << fallback << "': " << strerror(err2) << "\n";
            std::cerr << "Aborting test log creation." << std::endl;
            return;
        }
    }

    for (size_t i = 0; i < 11 * 1024 * 1024; i++)
    {
        test.put('x');
    }
    test.close();
    std::cout << "Test log written to " << (test ? primary : fallback) << std::endl;
}

int main()
{
    // Check root
    if (getuid() != 0) {
        std::cerr << "This program must be run as root" << std::endl;
        return EXIT_FAILURE;
    }

    Tintin_reporter reporter;
    reporter.setFileName("matt_daemon.log");
    reporter.writeLog("Started.", "INFO");

    Daemon daemon(reporter);
    // createLargeLog();
    daemon.start();

    return 0;
}
