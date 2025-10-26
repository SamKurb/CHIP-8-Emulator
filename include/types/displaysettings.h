#ifndef DISPLAY_SETTINGS_H
#define DISPLAY_SETTINGS_H

#include "colour.h"
#include <string>

struct DisplaySettings
{
    int mainWindowWidth{1920};
    int mainWindowHeight{1080};
    int gameDisplayTextureWidth{ 1920 };
    int gameDisplayTextureHeight{ 960 };
    int targetFPS{ 60 };

    bool showDebugWindows{ true };
    bool gridOn{ true };
    bool fullScreenEnabled { false };
    bool renderGameToImGuiWindow { false };

    Colour::RGBA onPixelColour{ Colour::colours[Colour::white]  };
    Colour::RGBA offPixelColour{ Colour::colours[Colour::black] };
    Colour::RGBA gridColour{ Colour::colours[Colour::white] };

    const std::string windowTitle{ "CHIP-8 Emulator" };
};

#endif