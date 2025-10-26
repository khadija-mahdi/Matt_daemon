
#include "daemon.hpp"

void Daemon::start(Tintin_reporter &reporter)
{
    // First fork: parent exits so child runs in background
    pid_t pid = fork();
    if (pid < 0) {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // parent exits
        std::exit(EXIT_SUCCESS);
    }

    // Create new session and become process group leader
    if (setsid() < 0) {
        std::perror("setsid");
        std::exit(EXIT_FAILURE);
    }
    
    // Second fork: ensure the daemon cannot acquire a controlling terminal
    pid = fork();
    if (pid < 0) {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // first child exits
        std::exit(EXIT_SUCCESS);
    }
    
    // Daemon setup
    umask(0);
    
    // Change working directory to root
    if (chdir("/") != 0) {
        std::perror("chdir");
        // Continue even if chdir fails
    }

    // Lock file to prevent multiple instances
    std::string lock_file_path = "/var/lock/matt_daemon.lock";

    int lock_fd = open(lock_file_path.c_str(), O_RDWR | O_CREAT, 0644);
    if (lock_fd < 0) {
        std::cerr << "Error opening lock file: " << lock_file_path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Try to acquire exclusive lock
    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            std::cerr << "Daemon is already running." << std::endl;
        } else {
            std::perror("flock");
        }
        close(lock_fd);
        std::exit(EXIT_FAILURE);
    }

    // Close all open file descriptors
    long maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1) maxfd = 1024;
    for (long fd = 0; fd < maxfd; ++fd) {
        close(static_cast<int>(fd));
    }

    // Redirect standard file descriptors to /dev/null
    int devnull = open("/dev/null", O_RDWR);
    if (devnull != -1) {
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        if (devnull > STDERR_FILENO) {
            close(devnull);
        }
    }

    reporter.writeLog("Daemon started successfully.", "INFO");
}

void Daemon::stop()
{
    // Implementation for stopping the daemon
}