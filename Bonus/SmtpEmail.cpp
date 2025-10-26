#include "SmtpEmail.hpp"
#include "helper.hpp"


// Constructors / assignment / destructor
SmtpMail::SmtpMail()
    : server(nullptr), port(nullptr), username(nullptr), password(nullptr), recipient(nullptr), payload_lines()
{
}

SmtpMail::SmtpMail(const SmtpMail &other)
    : server(other.server), port(other.port), username(other.username), password(other.password), recipient(other.recipient), payload_lines(other.payload_lines)
{
}

SmtpMail &SmtpMail::operator=(const SmtpMail &other)
{
    if (this != &other)
    {
        server = other.server;
        port = other.port;
        username = other.username;
        password = other.password;
        recipient = other.recipient;
        payload_lines = other.payload_lines;
    }
    return *this;
}

SmtpMail::~SmtpMail() = default;

size_t SmtpMail::payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    auto *lines = static_cast<std::vector<std::string> *>(userp);
    static size_t line_index = 0;

    if (line_index < lines->size())
    {
        const std::string &line = (*lines)[line_index++];
        size_t toCopy = std::min(line.size(), size * nmemb);
        memcpy(ptr, line.c_str(), toCopy);
        return toCopy;
    }

    line_index = 0;
    return 0;
}

std::string SmtpMail::getTodayDate()
{
    time_t now = time(nullptr);
    char buf[100];
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %z", localtime(&now));
    return std::string(buf);
}

void SmtpMail::sendEmail(const std::string &message)
{
    loadEnvFile("/home/kmahdi/Desktop/Matt_daemon/Bonus/env");

    server = std::getenv("SMTPSERVER");
    port = std::getenv("SMTPPORT");
    username = std::getenv("USERNAME");
    password = std::getenv("PASSWORD");
    recipient = std::getenv("RECIPIENTS");

    if (!server || !port || !username || !password || !recipient)
    {
        std::cerr << "Missing SMTP environment variables!\n";
        return;
    }

    std::string date = getTodayDate();
    payload_lines = {
        "Date: " + date + "\r\n",
        "To: " + std::string(recipient) + "\r\n",
        "From: " + std::string(username) + "\r\n",
        "Subject: Matt Daemon Error Report\r\n",
        "\r\n",
        message + "\r\n"};

    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return;
    }

    std::string url = "smtps://" + std::string(server) + ":" + std::string(port);
    struct curl_slist *recipients = nullptr;
    recipients = curl_slist_append(recipients, recipient);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, username);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload_lines);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
    else
        std::cout << "Email sent successfully!\n";

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
}
