#include <iostream>

#include "emulator.h"

int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    try
    {
        Emulator emulator{};
        emulator.run();
        return EXIT_SUCCESS;
    }
    catch (const std::runtime_error& exception)
    {
        std::cerr << "FATAL ERROR. Execution halted: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}

