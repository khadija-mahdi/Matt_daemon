#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

// #include "matt_deamon.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <cerrno>

 #include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <csignal>


class Tintin_reporter
{
    private:
        std::string message;
        std::string file_name;
        std::string default_folder;

        void createDirectoryIfNotExists();

    public:
        Tintin_reporter();
        Tintin_reporter(const Tintin_reporter &other);
        Tintin_reporter &operator=(const Tintin_reporter &other);
        ~Tintin_reporter();

        void setMessage(const std::string& msg, const std::string &message_type);
        std::string getMessage() const;
        void setFileName(const std::string &fname);
        std::string getFileName() const ;

        void writeLog(const std::string &msg,const std::string &message_type);
};

#endif // TINTIN_REPORTER_HPP