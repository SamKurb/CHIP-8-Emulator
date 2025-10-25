#ifndef RENDERER_INIT_EXCEPTION_H
#define RENDERER_INIT_EXCEPTION_H
#include <exception>
#include <stdexcept>

class RendererInitException : public std::runtime_error
{
    public:
        explicit RendererInitException(const std::string& message)
        : std::runtime_error(message)
        {
        }
};
#endif