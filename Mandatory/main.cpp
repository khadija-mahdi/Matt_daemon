#include "matt_daemon.hpp"
#include "tintin_reporter.hpp"
#include "daemon.hpp"

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
    daemon.start();

    return 0;
}