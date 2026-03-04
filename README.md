# Matt_daemon

## 👥 Collaborators

- [**hasabir**](https://github.com/hasabir)
- [**khadija-mahdi**](https://github.com/khadija-mahdi)

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

A Unix daemon implementation in C++ that runs as a background service with network capabilities, comprehensive logging, and remote management features.

##  Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Commands](#commands)
- [Configuration](#configuration)
- [Project Structure](#project-structure)
- [Troubleshooting](#troubleshooting)

##  Overview

`Matt_daemon` is a Unix daemon with client-server architecture, comprehensive logging, and process management. It listens on **port 4242** and supports up to **3 concurrent clients**.

**Key Features:**
- Requires root privileges
- Single-instance protection via `/var/lock/matt_daemon.lock`
- Logs to `/var/log/matt_daemon/matt_daemon.log`
- Graceful shutdown on SIGTERM, SIGINT, SIGQUIT
- TCP server on port 4242 (max 3 clients)

## ✨ Features

**Core Features:**
- Full daemonization (fork/setsid)
- Signal handling (SIGTERM, SIGINT, SIGQUIT)
- Structured logging with timestamps
- TCP server with multi-client support (max 3)
- Lock file to prevent multiple instances
- Root privilege verification

**Bonus Features:**
- Real-time statistics and control commands
- Automatic log archiving and rotation
- SMTP email notifications
- Authenticated remote shell
- Web-based dashboard
- Uptime and message tracking

##  Architecture

```
Matt_daemon Process
├── Signal Handlers (SIGTERM, SIGINT, SIGQUIT)
├── Daemon Core (Lock, Daemonization, Resources)
├── Server (Port 4242, Max 3 clients)
└── Tintin_reporter (Logging, Rotation)
    │
    ├── Client 1
    ├── Client 2
    └── Client 3
```

## Requirements

**System:**
- Linux (Ubuntu 20.04+)
- g++ with C++17 support
- Root privileges

**Dependencies:**

```bash
sudo apt-get install build-essential libcurl4-openssl-dev
```

##  Installation

```bash
# Mandatory version
cd Mandatory && make

# Bonus version (with advanced features)
cd Bonus && make
```

##  Usage

**Start the daemon:**

```bash
sudo ./Matt_daemon
```

**Connect as a client:**

```bash
# Using netcat
nc localhost 4242

# Using telnet
telnet localhost 4242

# Using bonus client
cd Bonus/client && ./client
```

**Stop the daemon:**

```bash
sudo kill -SIGTERM $(pgrep Matt_daemon)
# Or use quit command from client
echo "quit" | nc localhost 4242
```

## 🎮 Commands

| Command | Description |
|---------|-------------|
| `status` | Show daemon statistics |
| `uptime` | Display daemon uptime |
| `clients` | Number of active clients |
| `messages` | Total messages processed |
| `quit` | Shutdown daemon |
| `help` | Show available commands |

## Configuration

**Logs:** `/var/log/matt_daemon/matt_daemon.log`  
**Format:** `[YYYY-MM-DD HH:MM:SS] [LEVEL] Message`

**Port & Clients:** Defined in `server.hpp`
```cpp
#define PORT 4242
#define MAX_CLIENTS 3
```

**Lock File:** `/var/lock/matt_daemon.lock`

##  Project Structure

```
Matt_daemon/
├── Mandatory/              # Core daemon implementation
│   ├── daemon.cpp/hpp     # Daemon management
│   ├── server.cpp/hpp     # Network server
│   ├── tintin_reporter.cpp/hpp  # Logging system
│   ├── main.cpp
│   └── Makefile
│
└── Bonus/                  # Advanced features
    ├── ControlCommands     # Command handling
    ├── LogArchiver         # Log rotation
    ├── SmtpEmail           # Email notifications
    ├── dashboard.html      # Web dashboard
    ├── helper              # Utilities
    └── client/             # Remote client
        ├── Authentication
        └── RemoteShell
```

##  Troubleshooting

| Issue | Solution |
|-------|----------|
| "Must be run as root" | `sudo ./Matt_daemon` |
| "Address already in use" | `sudo kill -9 $(pgrep Matt_daemon)` or `sudo lsof -i :4242` |
| "Cannot create lock file" | `sudo rm /var/lock/matt_daemon.lock` |
| "Cannot open log file" | `sudo mkdir -p /var/log/matt_daemon && sudo chmod 755 /var/log/matt_daemon` |
| "Connection refused" | Check daemon status: `ps aux \| grep Matt_daemon` |

**View logs:**
```bash
sudo tail -f /var/log/matt_daemon/matt_daemon.log
```

---

**Created for learning systems programming**