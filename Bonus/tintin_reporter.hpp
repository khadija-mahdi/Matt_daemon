#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include "matt_daemon.hpp"




class Tintin_reporter
{
    private:
        std::string message;
        std::string file_name;
        std::string default_folder;
        int log_fd;

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
        int getLogFileDescriptor() const;
        void writeLog(const std::string &msg,const std::string &message_type);
};

#endif // TINTIN_REPORTER_HPP