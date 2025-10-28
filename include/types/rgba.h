#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>
#include <array>
#include <iterator> // For std::size

#include <SDL_pixels.h>

#include "enumarray.h"
#include "imgui.h"
#include "../utils/utility.h"

struct RGBA
{
    uint8_t red { 0xFF };
    uint8_t green { 0xFF };
    uint8_t blue { 0xFF };
    uint8_t alpha { 0xFF };

    constexpr RGBA() = default;

    constexpr RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF)
    : red{ red }
    , green{ green }
    , blue{ blue }
    , alpha{ alpha }
    {
    }

    RGBA(const ImVec4& colour)
    : red{ static_cast<uint8_t>(colour.x * 255.0f) }
    , green{ static_cast<uint8_t>(colour.y * 255.0f) }
    , blue{ static_cast<uint8_t>(colour.z * 255.0f) }
    , alpha{ static_cast<uint8_t>(colour.w * 255.0f) }
    {
    }

    constexpr operator SDL_Color() const
    {
        return SDL_Color{ red, green,blue, 0xFF };
    }

    operator ImVec4() const
    {
        return ImVec4
        {
            static_cast<float>(red) / 255.0f,
            static_cast<float>(green) / 255.0f,
            static_cast<float>(blue) / 255.0f,
            static_cast<float>(alpha) / 255.0f
        };
    }

    static constexpr RGBA white()       { return RGBA{ 0xFF, 0xFF, 0xFF }; }
    static constexpr RGBA black()       { return RGBA{ 0x00, 0x00, 0x00 }; }
    static constexpr RGBA darkGreen()   { return RGBA{ 0x0F, 0x38, 0x0F }; }
    static constexpr RGBA lightGreen()  { return RGBA{ 0x9B, 0xBC, 0x0F }; }
    static constexpr RGBA amber()       { return RGBA{ 0xFF, 0xBF, 0x00 }; }
    static constexpr RGBA butterScotch(){ return RGBA{ 0xE3, 0x96, 0x3E }; }


};

#endif