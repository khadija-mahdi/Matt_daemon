#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <string>
#include <iostream>
#include <exception>

class Tintin_reporter
{
    private:
        std::string name;

    public:
        // Coplien (canonical) form
        Tintin_reporter();
        Tintin_reporter(const Tintin_reporter &other);
        Tintin_reporter &operator=(const Tintin_reporter &other);
        ~Tintin_reporter();

        // additional constructor
        Tintin_reporter(const std::string &name, int grade);

        // accessors (optional but useful)
        int getGrade() const;
        const std::string &getName() const;
};

#endif // TINTIN_REPORTER_HPP