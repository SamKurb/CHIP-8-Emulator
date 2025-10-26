#ifndef BAD_OPCODE_EXCEPTION_H
#define BAD_OPCODE_EXCEPTION_H
#include <exception>
#include <stdexcept>

class BadOpcodeException : public std::runtime_error
{
    public:
        explicit BadOpcodeException(const std::string& what)
        : std::runtime_error(what)
        {
        }
};

#endif