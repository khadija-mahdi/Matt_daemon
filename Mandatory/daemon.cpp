#include "daemon.hpp"
#include "server.hpp"

Daemon *Daemon::instance = nullptr;

Daemon::Daemon(Tintin_reporter &reporter)
    : running(false),
      lock_file_path("/var/lock/matt_daemon.lock"),
      lock_fd(-1),
      reporter(reporter),
      server(nullptr)
{
    instance = this;
}

Daemon::~Daemon() {
    stop();
}

void Daemon::start()
{
    // Check and create lock file BEFORE forking
    lock_fd = open(lock_file_path.c_str(), O_RDWR | O_CREAT, 0644);
    if (lock_fd < 0) {
        std::cerr << "Can't open :" << lock_file_path << std::endl;
        reporter.writeLog("Error opening lock file.", "ERROR");
        reporter.writeLog("Quitting.", "INFO");
        std::exit(EXIT_FAILURE);
    }

    // Try to acquire exclusive lock
    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) {
        std::cerr << "Can't open :" << lock_file_path << std::endl;
        reporter.writeLog("Error file locked.", "ERROR");
        reporter.writeLog("Quitting.", "INFO");
        close(lock_fd);
        std::exit(EXIT_FAILURE);
    }

    reporter.writeLog("Creating server.", "INFO");
    reporter.writeLog("Server created.", "INFO");
    reporter.writeLog("Entering Daemon mode.", "INFO");

    // First fork
    pid_t pid = fork();
    if (pid < 0) {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // Parent exits
        std::exit(EXIT_SUCCESS);
    }

    // Create new session
    if (setsid() < 0) {
        std::perror("setsid");
        std::exit(EXIT_FAILURE);
    }

    // Second fork
    pid = fork();
    if (pid < 0) {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // First child exits
        std::exit(EXIT_SUCCESS);
    }

    // Daemon is now running
    umask(0);
    chdir("/");

    // Get log fd before closing everything
    int log_fd = reporter.getLogFileDescriptor();

    // Close all file descriptors except lock_fd and log_fd
    long maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)
        maxfd = 1024;
    
    for (long fd = 0; fd < maxfd; ++fd) {
        if (fd != lock_fd && fd != log_fd) {
            close(static_cast<int>(fd));
        }
    }

    // Redirect stdin, stdout, stderr to /dev/null
    int devnull = open("/dev/null", O_RDWR);
    if (devnull != -1) {
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        if (devnull > STDERR_FILENO && devnull != lock_fd && devnull != log_fd) {
            close(devnull);
        }
    }

    // Log that daemon started
    reporter.writeLog("started. PID: " + std::to_string(getpid()) + ".", "INFO");

    // Setup signal handlers
    setupSignalHandlers();

    // Create and start server
    running = true;
    Server srv(reporter);
    server = &srv;
    
    server->start();  // This blocks until quit is received

    // When server stops, cleanup
    stop();
}

void Daemon::setupSignalHandlers()
{
    struct sigaction sa;
    sa.sa_handler = staticSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    signal(SIGHUP, SIG_IGN);
}

void Daemon::staticSignalHandler(int signal)
{
    if (instance) {
        instance->signalHandler(signal);
    }
}

void Daemon::signalHandler(int signal)
{
    reporter.writeLog("Signal handler.", "INFO");
    
    switch (signal) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            running = false;
            if (server) {
                server->stop();
            }
            break;
    }
    
    reporter.writeLog("Quitting.", "INFO");
    stop();
    std::exit(0);
}

void Daemon::stop()
{
    running = false;

    // Release and remove lock file
    if (lock_fd != -1) {
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        lock_fd = -1;
    }

    unlink(lock_file_path.c_str());
}