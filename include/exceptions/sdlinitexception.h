#ifndef SDL_INIT_EXCEPTION_H
#define SDL_INIT_EXCEPTION_H
#include <exception>
#include <stdexcept>

class SDLInitException : public std::runtime_error
{
    public:
        explicit SDLInitException(const std::string& message)
        : std::runtime_error(message)
        {
        }
};
#endif