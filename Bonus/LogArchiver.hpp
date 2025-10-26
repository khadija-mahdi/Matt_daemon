#ifndef LOGARCHIVER_HPP
#define LOGARCHIVER_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <cstdlib>

class LogArchiver {
private:
    std::string logDirectory;
    std::string logFile;
    size_t maxFileSize;
    int maxArchiveCount;
    int maxArchiveDays;

    size_t getLogFileSize();
    int getLogAgeDays(const std::string &fullPath) ;
    void compressFile(const std::string &filepath);
    void deleteOldArchives();

public:

    LogArchiver();
    LogArchiver(const LogArchiver &other);
    LogArchiver &operator=(const LogArchiver &other);
    ~LogArchiver();
    void AdvancedLogArchiving();
    void checkAndMaintain();
};

#endif