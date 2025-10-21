#ifndef DISPLAY_SETTINGS_H
#define DISPLAY_SETTINGS_H

#include "colour.h"
#include <string>

struct DisplaySettings
{
    int userDesiredWidth{2560};
    int userDesiredHeight{1280};
    bool gridOn{ true };
    bool fullScreenEnabled { false };
    bool renderGameToImGuiWindow { false };
    Colour::RGBA onPixelColour{ Colour::colours[Colour::white]  };
    Colour::RGBA offPixelColour{ Colour::colours[Colour::black] };
    const std::string windowTitle{ "CHIP-8 Emulator" };
};

#endif