#ifndef CHIP_STACK_ERROR_EXCEPTION_H
#define CHIP_STACK_ERROR_EXCEPTION_H

#include <stdexcept>

// To be used if the chip stack size exceeds limit, or a pop is attempted when it is empty
class ChipStackErrorException : public std::runtime_error
{
    public:
    explicit ChipStackErrorException(const std::string& what)
    : std::runtime_error(what)
    {
    }
};

#endif