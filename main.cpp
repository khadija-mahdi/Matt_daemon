#include "matt_deamon.hpp"
#include "tintin_reporter.hpp"
#include "daemon.hpp"




int main()
{
    if (getuid() != 0) {
    std::cerr << "This program must be run as root" << std::endl;
    return EXIT_FAILURE;
    }
    Tintin_reporter reporter;
    reporter.setFileName("daemon.log");
    reporter.writeLog(std::string("Started."), std::string("INFO"));
    Daemon daemon(reporter);
    daemon.start();
    // start server operations here

    return 0;
}