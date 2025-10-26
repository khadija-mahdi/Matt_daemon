#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>


void loadEnvFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream is_line(line);
        std::string key, value;

        if (std::getline(is_line, key, '=') && std::getline(is_line, value))
        {
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (value.size() >= 2)
            {
                if ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\''))
                {
                    value = value.substr(1, value.size() - 2);
                }
            }

            setenv(key.c_str(), value.c_str(), 1);
        }
    }
}

std::string xorCipher(const std::string& text, const std::string& key) {
    // If key is empty, return the original text unchanged
    if (key.empty())
        return text;

    std::string result = text;
    for (size_t i = 0; i < text.size(); ++i)
        result[i] = text[i] ^ key[i % key.size()];
    return result;
}