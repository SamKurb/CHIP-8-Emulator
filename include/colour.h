#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>
#include <array>
#include <iterator> // For std::size

#include <SDL_pixels.h>

namespace Colour
{
    struct RGBValues
    {
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};

        operator SDL_Color() const 
        {
            return SDL_Color{ r, g, b, 255 };
        }
    };

    // More colours to be added
    enum Type
    {
        white,
        black,
        darkGreen,
        lightGreen,
        amber,
        butterScotch,
        numColours,
    };

    constexpr inline std::array colours{
        RGBValues { 0xFF, 0xFF, 0xFF }, // White
        RGBValues { 0x00, 0x00, 0x00 }, // Black
        RGBValues { 0x0F, 0x38, 0x0F }, // Dark green
        RGBValues { 0x9B, 0xBC, 0x0F }, // Light green
        RGBValues { 0xFF, 0xBF, 0x00 }, // Amber
        RGBValues { 0xE3, 0x96, 0x3E }, // Butterscotch
        //RGBValues { 0xFF, 0x7E, 0x00 },
    };

    static_assert(numColours == std::size(colours), "Number of elements in array DisplayConfig::colourArr does not match the number of Colours in enumerator DisplayyConfig::Colours");
}

#endif