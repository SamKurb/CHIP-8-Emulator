#ifndef FILEINPUTEXCEPTION_H
#define FILEINPUTEXCEPTION_H
#include <stdexcept>

class FileInputException : public std::runtime_error
{
    public:
        explicit FileInputException(const std::string& what)
        : std::runtime_error(what)
        {
        }
};

#endif