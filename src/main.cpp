#include <iostream>

#include "emulator.h"

int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    try
    {
        Emulator emulator{};
        emulator.run();
        return 0;
    }
    catch (const std::runtime_error& exception)
    {
        std::cerr << "FATAL ERROR. Execution halted: " << exception.what() << std::endl;
    }
}

