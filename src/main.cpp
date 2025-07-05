#include <iostream>

#include <iomanip>
#include "renderer.h"
#include "utility.h"
#include "random.h"
#include "chip8.h"
#include "settings.h"

namespace ROMS
{
    // Currently works
    const std::string ibm{ "roms/ibm.ch8" };
    const std::string testopc{ "roms/3-corax+.ch8" };
    const std::string testflags{ "roms/4-flags.ch8" };
    const std::string testquirks{ "roms/5-quirks.ch8" };
    const std::string testkeypad{ "roms/6-keypad.ch8" };
    const std::string rngTest{ "roms/rngTest.ch8" };

    // Works completely fine
    const std::string tetris{ "roms/tetris.ch8" };
    const std::string snake{ "roms/snake.ch8" };
    const std::string superPong{ "roms/superpong.ch8" };
    const std::string pong{ "roms/pong.ch8" };
    
    // Works, but need to increase IPF by 2-3x
    const std::string fallingGhosts{ "roms/ghosts.ch8" };

    // Also works but runs slowly, not sure if that is normal
    const std::string cellAuotmata1d{ "roms/1dcell.ch8" };

    // Raises E12 error - might be due to quirk
    const std::string bcTest{ "roms/BC_test.ch8" };

    // Doesnt fully work - likely due to quirks
    const std::string spaceInvaders{ "roms/Space Invaders [David Winter].ch8" };

    // Not sure if this works properly - dont know the controls and dont know where I found it
    const std::string puzzle15{ "roms/15puzzle.ch8" };

}

int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    Chip8 chip{};
    Renderer renderer{DisplayConfig::resolutionWidth, DisplayConfig::resolutionHeight, DisplayConfig::displayGrid};

    chip.loadFile(ROMS::puzzle15);

    bool quit{ false };
    SDL_Event e{};

    Uint32 frameStartTimeMs{};
    Uint32 frameEndTimeMs{};
    Uint32 timeElapsedMs{};

    // For 60 fps this shoild be 16ms
    const int targetFrameDelayMs{ 1000 / DisplayConfig::targetFPS };

    while (!quit)
    {
        frameStartTimeMs = SDL_GetTicks();
        
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    quit = true;
                }

                for (std::size_t i{ 0 }; i < 16; ++i)
                {
                    if (e.key.keysym.scancode == chip.keyMap[i])
                    {
                        chip.setKeyDown(i);
                    }
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                for (std::size_t i{ 0 }; i < 16; ++i)
                {
                    if (e.key.keysym.scancode == chip.keyMap[i])
                    {
                        chip.setKeyUp(i);
                    }
                }
            }
        }

        chip.decrementTimers();

        for (int i = 0; i < ChipConfig::instrPerFrame; ++i)
        {
            chip.performFDECycle();
        }

        renderer.drawToScreen(chip.getScreenBuffer());
        renderer.render();
            
        chip.setPrevFrameInputs();

        frameEndTimeMs = SDL_GetTicks();
        timeElapsedMs = frameEndTimeMs - frameStartTimeMs;

        // Frames may process faster than the target frametime, so we delay to make sure that we only move on to the next frame when enough time has passed
        if (timeElapsedMs < targetFrameDelayMs)
        {
            SDL_Delay(targetFrameDelayMs - timeElapsedMs);
        }
    }

    return 0;
}

