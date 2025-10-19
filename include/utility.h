#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

namespace Utility
{
    template<typename T>
    constexpr std::size_t toUZ(T value)
    {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "toUZ only accepts integral or enum types");
        return static_cast<std::size_t>(value);
    }

    template<typename T>
    constexpr uint16_t toU16(T value)
    {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "toU16 only accepts integral or enum types");
        return static_cast<uint16_t>(value);
    }

    template<typename T>
    constexpr uint8_t toU8(T value)
    {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "toU8 only accepts integral or enum types");
        return static_cast<uint8_t>(value);
    }

    template<typename T>
    constexpr int toInt(T value)
    {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "toInt only accepts integral or enum types");
        return static_cast<int>(value);
    }

}

#endif