#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>

void loadEnvFile(const std::string &filename);
std::string xorCipher(const std::string& text, const std::string& key);

#endif
