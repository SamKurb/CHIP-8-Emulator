#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>
#include <array>
#include <iterator> // For std::size

#include <SDL_pixels.h>
#include "imgui.h"
#include "utility.h"

namespace Colour
{
    struct RGBA
    {
        uint8_t red{0xFF};
        uint8_t green{0xFF};
        uint8_t blue{0xFF};
        uint8_t alpha{0xFF};

        constexpr RGBA() = default;

        constexpr RGBA(float r, float g, float b)
        : red{ static_cast<uint8_t>(r) }
        , green{ static_cast<uint8_t>(g) }
        , blue{ static_cast<uint8_t>(b) }
        , alpha{ 0xFF }
        {
        }

        RGBA(const ImVec4& colour)
        : red{ static_cast<uint8_t>(colour.x * 255.0f) }
        , green{ static_cast<uint8_t>(colour.y * 255.0f) }
        , blue{ static_cast<uint8_t>(colour.z * 255.0f) }
        , alpha{ static_cast<uint8_t>(colour.w * 255.0f) }
        {}

        operator SDL_Color() const 
        {
            return SDL_Color{ red, green,
                blue, 0xFF };
        }

        operator ImVec4() const
        {
            return ImVec4{ 
                static_cast<float>(red) / 255.0f,
                static_cast<float>(green) / 255.0f,
                static_cast<float>(blue) / 255.0f,
                static_cast<float>(alpha) / 255.0f
            };
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
        RGBA { 0xFF, 0xFF, 0xFF }, // White
        RGBA { 0x00, 0x00, 0x00 }, // Black
        RGBA { 0x0F, 0x38, 0x0F }, // Dark green
        RGBA { 0x9B, 0xBC, 0x0F }, // Light green
        RGBA { 0xFF, 0xBF, 0x00 }, // Amber
        RGBA { 0xE3, 0x96, 0x3E }, // Butterscotch
        //RGBA { 0xFF, 0x7E, 0x00 },
    };

    static_assert(numColours == std::size(colours), "Number of elements in array DisplayConfig::colourArr does not match the number of Colours in enumerator DisplayyConfig::Colours");
}

#endif