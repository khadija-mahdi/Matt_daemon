
#include "daemon.hpp"

Daemon *Daemon::instance = nullptr;

Daemon::Daemon(Tintin_reporter &reporter)
    : running(false),
      lock_file_path("/var/lock/matt_daemon.lock"),
      lock_fd(-1),
      reporter(reporter)
{
    instance = this;
}

void Daemon::start()
{
    // First fork: parent exits so child runs in background
    pid_t pid = fork();
    if (pid < 0)
    {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        // parent exits
        std::exit(EXIT_SUCCESS);
    }

    // Create new session and become process group leader
    if (setsid() < 0)
    {
        std::perror("setsid");
        std::exit(EXIT_FAILURE);
    }

    // Second fork: ensure the daemon cannot acquire a controlling terminal
    pid = fork();
    if (pid < 0)
    {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        // first child exits
        std::exit(EXIT_SUCCESS);
    }

    // Daemon setup
    umask(0);

    // Change working directory to root
    if (chdir("/") != 0)
    {
        std::perror("chdir");
        // Continue even if chdir fails
    }

    // Lock file to prevent multiple instances
    // Use the class member variables (don't redeclare locals which shadow them)
    lock_file_path = "/var/lock/matt_daemon.lock";

    lock_fd = open(lock_file_path.c_str(), O_RDWR | O_CREAT, 0644);
    if (lock_fd < 0)
    {
        std::cerr << "Error opening lock file: " << lock_file_path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Try to acquire exclusive lock
    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1)
    {
        // Some systems may return EWOULDBLOCK or EAGAIN for LOCK_NB failure
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            std::cerr << "Daemon is already running." << std::endl;
        }
        else
        {
            std::perror("flock");
        }
        close(lock_fd);
        std::exit(EXIT_FAILURE);
    }

    // Close all open file descriptors
    int log_fd = reporter.getLogFileDescriptor();
    long maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)
        maxfd = 1024;
    for (long fd = 0; fd < maxfd; ++fd) {
        if (fd != lock_fd && fd != log_fd) {
            close(static_cast<int>(fd));
        }
    }


    // Redirect standard file descriptors to /dev/null
    int devnull = open("/dev/null", O_RDWR);
    if (devnull != -1) {
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        if (devnull > STDERR_FILENO && devnull != lock_fd && devnull != log_fd) {
            close(devnull);
        }
    }
    
    // NOW you can safely log
    reporter.writeLog("Matt_daemon: started. PID: " + std::to_string(getpid()) + ".", "INFO");
    

    // Setup signal handlers
    setupSignalHandlers();
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

    // Ignore SIGHUP to prevent termination when terminal closes
    signal(SIGHUP, SIG_IGN);

    running = true;
    reporter.writeLog("Daemon started successfully.", "INFO");

    // Main daemon loop
    while (running)
    {
        // Your daemon's main work here
        sleep(1);
    }

    // Cleanup before exit
    // stop();
}

void Daemon::staticSignalHandler(int signal) {
    if (instance) {
        instance->signalHandler(signal);
    }
}

void Daemon::signalHandler(int signal) {
    switch(signal) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            running = false;
            break;
    }
}

void Daemon::stop()
{
    running = false;
    
    // Release lock file
    if (lock_fd != -1) {
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        lock_fd = -1;
    }

        // Remove lock file
    unlink(lock_file_path.c_str());
}