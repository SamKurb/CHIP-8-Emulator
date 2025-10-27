#ifndef CHIP_OOB_MEMORY_ACCESS_EXCEPTION_H
#define CHIP_OOB_MEMORY_ACCESS_EXCEPTION_H
#include <stdexcept>

class ChipOOBMemoryAccessException : public std::runtime_error
{
    public:
    explicit ChipOOBMemoryAccessException(const std::string& what)
    : std::runtime_error(what)
    {
    }
};

#endif