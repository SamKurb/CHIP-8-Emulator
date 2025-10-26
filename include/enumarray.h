#ifndef ENUM_ARRAY_H
#define ENUM_ARRAY_H

#include <array>

template<class Enum, class T>
struct EnumArray
{
    std::array<T, static_cast<std::size_t>(Enum::MAX_VALUE)> underlyingData{};

    constexpr T& operator[](Enum e)
    {
        return underlyingData[static_cast<std::size_t>(e)];
    }

    constexpr const T& operator[](Enum e) const
    {
        return underlyingData[static_cast<std::size_t>(e)];
    }

    constexpr std::size_t size() const
    {
        return underlyingData.size();
    }

    constexpr std::array<T, static_cast<std::size_t>(Enum::MAX_VALUE)>& data()
    {
        return underlyingData;
    }

    constexpr const std::array<T, static_cast<std::size_t>(Enum::MAX_VALUE)>& data() const
    {
        return underlyingData;
    }

    auto begin() { return underlyingData.begin(); }
    auto end() { return underlyingData.end(); }
    auto begin() const { return underlyingData.begin(); }
    auto end() const { return underlyingData.end(); }

};

#endif