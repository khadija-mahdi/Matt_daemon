#ifndef SMTP_EMAIL_HPP
#define SMTP_EMAIL_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <curl/curl.h>

class SmtpMail
{
private:
    const char *server;
    const char *port;
    const char *username;
    const char *password;
    const char *recipient;

    std::vector<std::string> payload_lines;

    static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);
    std::string getTodayDate();

public:
    SmtpMail();
    SmtpMail(const SmtpMail &other);
    SmtpMail &operator=(const SmtpMail &other);
    ~SmtpMail();

    void sendEmail(const std::string &message);
};

#endif