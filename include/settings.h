#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstdint>
#include <cstddef>
#include "colour.h"

// This header contains namespaces related to the different configurable settings of the emulator, temporary until JSON parsing is implemented
namespace ChipConfig
{
    // Do not change
    constexpr int levelsOfNesting{ 16 }; // Currently does nothing, need to implement checking
    constexpr int screenWidth{ 64 };
    constexpr int screenHeight{ 32 };
    constexpr std::size_t bitsOfMemory{ 4096 };
    constexpr uint8_t timerStart{ 0 };

    // Can change if you know what you are doing
    constexpr uint16_t startAddress{ 0x200 };
    constexpr uint16_t fontsLocation{ 0x50 };

    // Instructions per frame - at 60fps, 12 IPF gives 700 instructions per second which is the recommended amount
    constexpr int instrPerFrame{ 12 };
}


namespace DisplayConfig
{
    constexpr int targetFPS{ 60 };
    constexpr bool displayGrid{ true };

    // Make sure that the resolution parameters selected are multiples of the CHIP8's 64x32 original resolution, so that everything is rendered properly
    constexpr int resolutionWidth{ 1280 };
    constexpr int resolutionHeight{ 640 };

    static_assert((resolutionWidth% ChipConfig::screenWidth) == 0, "Chosen resolution width is not divisible by Chip8 screen width (in pixel count)");
    static_assert((resolutionHeight% ChipConfig::screenHeight) == 0, "Chosen resolution height is not divisible by Chip8 screen height (in pixel count)");

    constexpr Colour::RGBValues offColour{ Colour::colours[Colour::black] };
    constexpr Colour::RGBValues onColour{ Colour::colours[Colour::white] };
}
#endif
