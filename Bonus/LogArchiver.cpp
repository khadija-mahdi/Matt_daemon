
#include "LogArchiver.hpp"
#include <unistd.h>
#include <iostream>
#include <fstream>

// Constructors / assignment / destructor
LogArchiver::LogArchiver() : logDirectory("/var/log/matt_daemon/"),
                             logFile("matt_daemon.log"),
                             maxFileSize(10 * 1024 * 1024),
                             maxArchiveCount(5),
                             maxArchiveDays(30)
{
}

LogArchiver::LogArchiver(const LogArchiver &other) : logDirectory(other.logDirectory),
                                                     logFile(other.logFile),
                                                     maxFileSize(other.maxFileSize),
                                                     maxArchiveCount(other.maxArchiveCount),
                                                     maxArchiveDays(other.maxArchiveDays)
{
}

LogArchiver &LogArchiver::operator=(const LogArchiver &other)
{
    if (this != &other)
    {
        logDirectory = other.logDirectory;
        logFile = other.logFile;
        maxFileSize = other.maxFileSize;
        maxArchiveCount = other.maxArchiveCount;
        maxArchiveDays = other.maxArchiveDays;
    }
    return *this;
}

LogArchiver::~LogArchiver() = default;

void LogArchiver::AdvancedLogArchiving()
{
    // Check if rotation is needed
    if (getLogFileSize() <= maxFileSize)
    {
        return;
    }

    std::cout << "Log size (" << getLogFileSize() << " bytes) exceeds max ("
              << maxFileSize << " bytes). Rotating..." << std::endl;

    std::string oldestArchivePath = logDirectory + logFile + "." + std::to_string(maxArchiveCount) + ".gz";
    if (access(oldestArchivePath.c_str(), F_OK) == 0)
    {
        std::cout << "Removing oldest archive: " << oldestArchivePath << std::endl;
        unlink(oldestArchivePath.c_str());
    }

    // Step 2: Rotate existing archives (.4.gz -> .5.gz, .3.gz -> .4.gz, etc.)
    for (int i = maxArchiveCount - 1; i >= 1; --i)
    {
        std::string srcPath = logDirectory + logFile + "." + std::to_string(i) + ".gz";
        std::string dstPath = logDirectory + logFile + "." + std::to_string(i + 1) + ".gz";

        if (access(srcPath.c_str(), F_OK) == 0)
        {
            std::cout << "Renaming archive: " << srcPath << " -> " << dstPath << std::endl;
            rename(srcPath.c_str(), dstPath.c_str());
        }
    }

    // Step 3: Move current log to .1 (uncompressed first)
    std::string currentLogPath = logDirectory + logFile;
    std::string rotatedLogPath = logDirectory + logFile + ".1";

    std::cout << "Moving current log: " << currentLogPath << " -> " << rotatedLogPath << std::endl;
    rename(currentLogPath.c_str(), rotatedLogPath.c_str());

    // Step 4: Compress the .1 file
    compressFile(rotatedLogPath); // Creates .1.gz

    // Step 5: Create new empty log file
    std::cout << "Creating new empty log file: " << currentLogPath << std::endl;
    std::ofstream ofs(currentLogPath, std::ios::out);
    ofs.close();

    chmod(currentLogPath.c_str(), 0644);

    deleteOldArchives();
}

size_t LogArchiver::getLogFileSize()
{
    struct stat st;
    std::string fullPath = logDirectory + logFile;
    if (stat(fullPath.c_str(), &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
    return 0;
}

int LogArchiver::getLogAgeDays(const std::string &fullPath)
{
    struct stat st;
    if (stat(fullPath.c_str(), &st) == 0)
    {
        time_t modificationTime = st.st_mtime;
        time_t now = time(nullptr);
        int daysOld = static_cast<int>((now - modificationTime) / (24 * 3600));
        return daysOld;
    }
    return 0;
}

void LogArchiver::compressFile(const std::string &filepath)
{
    std::string cmd = "gzip " + filepath;
    std::cout << "Compressing: " << filepath << std::endl;
    system(cmd.c_str());
}

void LogArchiver::deleteOldArchives()
{
    std::cout << "Checking for archives older than " << maxArchiveDays << " days..." << std::endl;

    for (int i = 1; i <= maxArchiveCount; i++)
    {
        std::string archivePath = logDirectory + logFile + "." + std::to_string(i) + ".gz";

        if (access(archivePath.c_str(), F_OK) == 0)
        {
            int age = getLogAgeDays(archivePath);
            std::cout << "Archive " << archivePath << " is " << age << " days old" << std::endl;

            if (age > maxArchiveDays)
            {
                std::cout << "Deleting old archive: " << archivePath << std::endl;
                unlink(archivePath.c_str());
            }
        }
    }
}

void LogArchiver::checkAndMaintain()
{
    AdvancedLogArchiving();
    deleteOldArchives();
}