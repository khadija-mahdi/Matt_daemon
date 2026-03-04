# Matt_daemon
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

A professional daemon implementation in C++ that runs as a background service with network capabilities, logging, and advanced features.

##  Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Commands](#commands)
- [Configuration](#configuration)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)
- [License](#license)

##  Overview

`Matt_daemon` is a Unix daemon that implements a client-server architecture with comprehensive logging, signal handling, and process management. The daemon listens on port **4242** and supports up to **3 concurrent clients**. It features automatic log rotation, email notifications, and a web-based dashboard.

### Key Highlights

- **Root Privilege Requirement**: Must run with root privileges
- **Lock File Protection**: Prevents multiple instances using `/var/lock/matt_daemon.lock`
- **Comprehensive Logging**: All events logged to `/var/log/matt_daemon/matt_daemon.log`
- **Signal Handling**: Graceful shutdown on SIGTERM, SIGINT, and SIGQUIT
- **Network Server**: TCP server on port 4242
- **Client Limit**: Maximum 3 concurrent connections

## ✨ Features

### Mandatory Features

- ✅ **Daemon Process**: Full daemonization with proper fork/setsid
- ✅ **Signal Handling**: Clean shutdown and signal management
- ✅ **Logging System**: Structured logging with timestamps
- ✅ **Network Server**: TCP server accepting client connections
- ✅ **Client Management**: Handle multiple clients (max 3)
- ✅ **Lock File**: Prevent multiple daemon instances
- ✅ **Root Check**: Verify root privileges on startup

### Bonus Features

- 🎁 **Control Commands**: Real-time daemon statistics and control
- 🎁 **Log Archiving**: Automatic compression and rotation
- 🎁 **SMTP Notifications**: Email alerts for critical events
- 🎁 **Remote Shell**: Authenticated remote access
- 🎁 **Web Dashboard**: HTML-based monitoring interface
- 🎁 **Advanced Statistics**: Uptime, message count, client tracking

##  Architecture

```
┌─────────────────────────────────────────────┐
│           Matt_daemon Process               │
│  ┌─────────────────────────────────────┐   │
│  │      Signal Handlers                │   │
│  │  (SIGTERM, SIGINT, SIGQUIT)         │   │
│  └─────────────────────────────────────┘   │
│                    │                        │
│  ┌─────────────────┴─────────────────┐     │
│  │       Daemon Core                 │     │
│  │  - Lock File Management           │     │
│  │  - Process Daemonization          │     │
│  │  - Resource Management            │     │
│  └─────────────────┬─────────────────┘     │
│                    │                        │
│  ┌────────────────┴──────────────────┐     │
│  │       Server (Port 4242)          │     │
│  │  - Accept Connections             │     │
│  │  - Manage Clients (Max 3)         │     │
│  │  - Process Commands               │     │
│  └────────────────┬──────────────────┘     │
│                   │                         │
│  ┌────────────────┴─────────────────┐      │
│  │    Tintin_reporter (Logger)      │      │
│  │  - File Logging                  │      │
│  │  - Log Rotation                  │      │
│  │  - Timestamp Management          │      │
│  └──────────────────────────────────┘      │
└─────────────────────────────────────────────┘
         │              │              │
    ┌────┴───┐     ┌───┴────┐    ┌───┴────┐
    │Client 1│     │Client 2│    │Client 3│
    └────────┘     └────────┘    └────────┘
```

## Requirements

### System Requirements

- **OS**: Linux (tested on Ubuntu 20.04+)
- **Compiler**: g++ with C++17 support
- **Privileges**: Root access required
- **Architecture**: x86_64

### Dependencies

```bash
# Build tools
sudo apt-get install build-essential

# Required libraries
sudo apt-get install libcurl4-openssl-dev

# Optional (for bonus features)
sudo apt-get install netcat telnet
```

##  Installation

### Mandatory Version

```bash
cd Mandatory
make
sudo ./Matt_daemon
```

### Bonus Version

```bash
cd Bonus
make
sudo ./Matt_daemon
```

### Compilation Flags

The project uses the following compilation flags:

```makefile
CPPFLAGS = -Wall -Wextra -Werror -std=c++17 -fsanitize=address -g -lcurl
```

##  Usage

### Starting the Daemon

```bash
# Must be run as root
sudo ./Matt_daemon

# Or with sudo -i
sudo -i
./Matt_daemon
```

### Expected Startup Output

```
Matt_daemon started successfully
PID: 12345
Listening on port 4242
Log file: /var/log/matt_daemon/matt_daemon.log
```

### Connecting as a Client

Using **netcat**:

```bash
nc localhost 4242
```

Using **telnet**:

```bash
telnet localhost 4242
```

Using the **bonus client** (if available):

```bash
cd Bonus/client
./client
```

### Stopping the Daemon

Send a termination signal:

```bash
# Find the daemon PID
ps aux | grep Matt_daemon

# Send signal
sudo kill -SIGTERM <PID>

# Or use quit command from client
echo "quit" | nc localhost 4242
```

## 🎮 Commands

### Client Commands (Bonus)

When connected to the daemon, you can use these commands:

| Command | Description | Example |
|---------|-------------|---------|
| `status` | Show daemon statistics | `status` |
| `uptime` | Display daemon uptime | `uptime` |
| `clients` | Number of active clients | `clients` |
| `messages` | Total messages processed | `messages` |
| `quit` | Disconnect and shutdown daemon | `quit` |
| `help` | Show available commands | `help` |

### Command Examples

```bash
# Connect to daemon
nc localhost 4242

# Check status
> status
Daemon Status:
  Uptime: 01:23:45
  Active Clients: 2/3
  Messages Processed: 127

# Check uptime
> uptime
Daemon has been running for: 01:23:45

# Quit
> quit
Shutting down daemon gracefully...
```

## Configuration

### Log Configuration

Logs are written to: `/var/log/matt_daemon/matt_daemon.log`

**Log Format:**

```
[YYYY-MM-DD HH:MM:SS] [LEVEL] Message
```

**Example:**

```
[2026-03-04 10:30:15] [INFO] Started.
[2026-03-04 10:30:15] [INFO] Server listening on port 4242
[2026-03-04 10:30:20] [INFO] Client connected from 127.0.0.1
[2026-03-04 10:31:00] [WARNING] Maximum clients reached
[2026-03-04 10:32:00] [INFO] Received quit command
[2026-03-04 10:32:01] [INFO] Daemon shutting down
```

### Port Configuration

The server port is defined in `server.hpp`:

```cpp
#define PORT 4242
#define MAX_CLIENTS 3
```

To change the port, modify these values and recompile:

```cpp
#define PORT 8080      // Custom port
#define MAX_CLIENTS 5  // Custom client limit
```

### Lock File

Location: `/var/lock/matt_daemon.lock`

This file prevents multiple daemon instances from running simultaneously.

##  Project Structure

```
matt/
├── README.md
├── Mandatory/                    # Basic daemon implementation
│   ├── daemon.cpp               # Daemon initialization and management
│   ├── daemon.hpp
│   ├── main.cpp                 # Entry point
│   ├── Makefile                 # Build configuration
│   ├── Matt_daemon              # Compiled binary
│   ├── matt_daemon.hpp          # Main header with includes
│   ├── server.cpp               # Network server implementation
│   ├── server.hpp
│   ├── tintin_reporter.cpp      # Logging system
│   ├── tintin_reporter.hpp
│   └── objects/                 # Object files directory
│
└── Bonus/                        # Extended features
    ├── ControlCommands.cpp      # Command handling
    ├── ControlCommands.hpp
    ├── daemon.cpp
    ├── daemon.hpp
    ├── dashboard.html           # Web dashboard
    ├── env                      # Environment configuration
    ├── helper.cpp               # Utility functions
    ├── helper.hpp
    ├── LogArchiver.cpp          # Log rotation and compression
    ├── LogArchiver.hpp
    ├── main.cpp
    ├── Makefile
    ├── Matt_daemon
    ├── matt_daemon.hpp
    ├── server.cpp
    ├── server.hpp
    ├── SmtpEmail.cpp            # Email notification system
    ├── SmtpEmail.hpp
    ├── tintin_reporter.cpp
    ├── tintin_reporter.hpp
    └── client/                   # Remote client application
        ├── Authentication.cpp    # Client authentication
        ├── Authentication.hpp
        ├── main.cpp
        ├── RemoteShell.cpp      # Remote shell interface
        └── RemoteShell.hpp
```

## Technical Details

### Daemon Class

```cpp
class Daemon {
private:
    std::atomic<bool> running;
    std::string lock_file_path;
    Tintin_reporter &reporter;
    int lock_fd;
    Server *server;
    
public:
    Daemon(Tintin_reporter &reporter);
    ~Daemon();
    void start();
    void stop();
    static Daemon* instance;
    void setupSignalHandlers();
    void signalHandler(int signal);
    static void staticSignalHandler(int signal);
};
```

### Server Class

```cpp
class Server {
private:
    Tintin_reporter &reporter;
    std::atomic<bool> running;
    std::atomic<bool> start_time;
    std::atomic<int> client_count;
    std::atomic<int> total_messages;
    
    int listen_fd;
    int client_sockets[MAX_CLIENTS];
    char buffer[1024];
    
    void handleClient(int client_fd);
    void closeClient(int index);
    
public:
    Server(Tintin_reporter &reporter);
    ~Server();
    
    void start();
    void stop();
    
    int getClientCount() const;
    int getTotalMessages() const;
};
```

### Logger Class (Tintin_reporter)

```cpp
class Tintin_reporter {
private:
    std::string logFile;
    std::ofstream logStream;
    
    std::string getCurrentTime();
    void ensureLogDirectory();
    
public:
    Tintin_reporter();
    ~Tintin_reporter();
    
    void setFileName(const std::string &filename);
    void writeLog(const std::string &message, const std::string &level);
};
```

### Daemonization Process

The daemon follows the standard Unix daemonization procedure:

1. **Fork** the parent process
2. **Set session ID** (setsid)
3. **Change working directory** to root (/)
4. **Close standard file descriptors** (stdin, stdout, stderr)
5. **Create lock file** to prevent multiple instances
6. **Setup signal handlers**
7. **Start server** on port 4242

```cpp
// Simplified daemonization flow
void Daemon::start() {
    // Check if already running
    if (!acquireLock()) {
        reporter.writeLog("Daemon already running", "ERROR");
        exit(EXIT_FAILURE);
    }
    
    // Fork process
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);  // Parent exits
    
    // Create new session
    if (setsid() < 0) exit(EXIT_FAILURE);
    
    // Fork again
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    
    // Change directory
    chdir("/");
    
    // Close file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Setup signals
    setupSignalHandlers();
    
    // Start server
    server = new Server(reporter);
    server->start();
}
```

### Signal Handling

```cpp
void Daemon::setupSignalHandlers() {
    signal(SIGTERM, staticSignalHandler);
    signal(SIGINT, staticSignalHandler);
    signal(SIGQUIT, staticSignalHandler);
}

void Daemon::staticSignalHandler(int signal) {
    if (instance) {
        instance->signalHandler(signal);
    }
}

void Daemon::signalHandler(int signal) {
    std::string sig_name;
    switch(signal) {
        case SIGTERM: sig_name = "SIGTERM"; break;
        case SIGINT:  sig_name = "SIGINT"; break;
        case SIGQUIT: sig_name = "SIGQUIT"; break;
        default:      sig_name = "UNKNOWN"; break;
    }
    
    reporter.writeLog("Received " + sig_name, "INFO");
    stop();
}
```

##  Examples

### Example 1: Basic Usage

```bash
# Terminal 1: Start daemon
sudo ./Matt_daemon

# Terminal 2: Connect and send commands
nc localhost 4242
Hello daemon!
status
quit

# Check logs
sudo tail -f /var/log/matt_daemon/matt_daemon.log
```

### Example 2: Multiple Clients

```bash
# Terminal 1: Start daemon
sudo ./Matt_daemon

# Terminal 2-4: Connect three clients
nc localhost 4242  # Client 1
nc localhost 4242  # Client 2
nc localhost 4242  # Client 3

# Terminal 5: Fourth client will be rejected
nc localhost 4242  # ERROR: Maximum clients reached
```

### Example 3: Testing Log Rotation (Bonus)

```bash
# Create a large log file
sudo ./Matt_daemon

# Logs will automatically rotate when size exceeds 10MB
# Archives created: matt_daemon.log.1.gz, matt_daemon.log.2.gz, etc.

# Check archives
ls -lh /var/log/matt_daemon/
```

### Example 4: Email Notifications (Bonus)

Configure SMTP in the `env` file:

```bash
# Bonus/env
SMTP_SERVER=smtp.gmail.com
SMTP_PORT=587
SMTP_USERNAME=your-email@gmail.com
SMTP_PASSWORD=your-app-password
SMTP_RECIPIENT=admin@example.com
```

The daemon will send email notifications for:
- Daemon startup
- Critical errors
- Daemon shutdown

### Example 5: Web Dashboard (Bonus)

```bash
# Start daemon
sudo ./Matt_daemon

# Open dashboard in browser
firefox Bonus/dashboard.html

# Or with Python HTTP server
cd Bonus
python3 -m http.server 8080
# Navigate to http://localhost:8080/dashboard.html
```

##  Troubleshooting

### Issue: "This program must be run as root"

**Solution:**
```bash
sudo ./Matt_daemon
# OR
sudo -i
./Matt_daemon
```

### Issue: "Address already in use"

**Solution:**
```bash
# Check if daemon is already running
ps aux | grep Matt_daemon

# Kill existing process
sudo kill -9 <PID>

# Or check port 4242
sudo lsof -i :4242
sudo kill -9 <PID>
```

### Issue: "Cannot create lock file"

**Solution:**
```bash
# Remove stale lock file
sudo rm /var/lock/matt_daemon.lock

# Ensure proper permissions
sudo mkdir -p /var/lock
sudo chmod 755 /var/lock
```

### Issue: "Cannot open log file"

**Solution:**
```bash
# Create log directory
sudo mkdir -p /var/log/matt_daemon

# Set permissions
sudo chmod 755 /var/log/matt_daemon

# Check disk space
df -h
```

### Issue: "Connection refused on port 4242"

**Solution:**
```bash
# Check if daemon is running
ps aux | grep Matt_daemon

# Check if port is open
sudo netstat -tulpn | grep 4242

# Check firewall
sudo ufw status
sudo ufw allow 4242/tcp
```

### Debugging

Enable debug mode by checking logs:

```bash
# Watch logs in real-time
sudo tail -f /var/log/matt_daemon/matt_daemon.log

# Search for errors
sudo grep ERROR /var/log/matt_daemon/matt_daemon.log

# Check last 50 lines
sudo tail -50 /var/log/matt_daemon/matt_daemon.log
```

##  Development

### Building from Source

```bash
# Clean build
make fclean
make

# Debug build (with sanitizers)
make debug

# Release build
make release
```

### Testing

```bash
# Test daemon startup
sudo ./Matt_daemon

# Test client connection
echo "test message" | nc localhost 4242

# Test signal handling
sudo kill -SIGTERM $(pgrep Matt_daemon)

# Test multiple clients
for i in {1..5}; do
    echo "Client $i" | nc localhost 4242 &
done
```

### Code Style

The project follows these conventions:
- **Classes**: PascalCase (e.g., `Daemon`, `Server`)
- **Methods**: camelCase (e.g., `start()`, `writeLog()`)
- **Variables**: snake_case (e.g., `lock_file_path`, `client_count`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_CLIENTS`, `PORT`)

##  Security Considerations

- **Root Privileges**: Required but use carefully
- **Lock File**: Prevents multiple instances
- **Signal Handling**: Clean shutdown prevents resource leaks
- **Client Limit**: Prevents resource exhaustion
- **Input Validation**: Commands validated before execution
- **Authentication** (Bonus): Client authentication in remote shell



##  Author

Created as part of a systems programming curriculum to demonstrate:
- Unix daemon programming
- Network programming
- Signal handling
- Process management
- Logging systems
- Client-server architecture

##  Acknowledgments

- Unix/Linux daemon best practices
- Stevens' "Advanced Programming in the UNIX Environment"
- System programming fundamentals

---

**Made with ❤️ for learning systems programming**