
#include "tintin_reporter.hpp"
#include <cstring>
#include <cerrno>

Tintin_reporter::Tintin_reporter()
{
    // Constructor implementation
    message = "";
    file_name = "";
    default_folder = "/var/log/matt_daemon/";
    createDirectoryIfNotExists();
}

Tintin_reporter::Tintin_reporter(const Tintin_reporter &other)
{
    // Copy constructor implementation
    message = other.message;
    file_name = other.file_name;
    default_folder = other.default_folder;
}

Tintin_reporter &Tintin_reporter::operator=(const Tintin_reporter &other)
{
    // Assignment operator implementation
    if (this != &other)
    {
        message = other.message;
        file_name = other.file_name;
        default_folder = other.default_folder;
    }
    return *this;
}

Tintin_reporter::~Tintin_reporter()
{
    // Destructor implementation
    std::cout << "Tintin_reporter destroyed." << std::endl;
}

void Tintin_reporter::createDirectoryIfNotExists()
{
    std::error_code ec;
    std::filesystem::path dir(default_folder);

    if (!std::filesystem::exists(dir, ec))
    {
        if (!std::filesystem::create_directories(dir, ec))
        {
            std::cerr << "Error creating directory: " << default_folder
                      << " - " << ec.message() << std::endl;
        }
    }
    else if (!std::filesystem::is_directory(dir, ec))
    {
        std::cerr << default_folder << " exists but is not a directory." << std::endl;
    }
}

void Tintin_reporter::setMessage(const std::string &msg,const std::string &message_type)
{

    if (message_type != "INFO" && message_type != "ERROR" && message_type != "LOG")
        throw std::invalid_argument("message type error");
    message = "["+  message_type + "] " + msg;
    std::time_t rawtime;
    std::time(&rawtime);

    // Convert to local time structure
    struct tm *timeinfo = std::localtime(&rawtime);

    // Format the time into the desired string
    char buffer[80]; // Buffer to store the formatted string
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y-%H:%M:%S", timeinfo);

    message = "[" + std::string(buffer) + "] - " + message;
    // Print the formatted timestamp
    // std::cout << "Current timestamp: " << buffer << std::endl;
}

std::string Tintin_reporter::getMessage() const
{
    return message;
}

void Tintin_reporter::setFileName(const std::string &fname)
{
    file_name = fname;
}

std::string Tintin_reporter::getFileName() const
{
    return file_name;
}

#include <iostream>
#include <fstream>
#include <string>

void Tintin_reporter::writeLog(const std::string &msg,const std::string &message_type)
{
    // Create the full path
    this->setMessage(msg, message_type);
    std::string full_path = default_folder + file_name;
    std::ofstream log_stream(full_path, std::ios::app);
    if (!log_stream.is_open())
    {
        std::cerr << "Error opening log file: " << full_path
                  << " - " << std::strerror(errno) << std::endl;
        return;
    }


    log_stream << message << std::endl;
    log_stream.close();
}
