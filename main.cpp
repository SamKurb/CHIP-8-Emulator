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
    const std::string ibm{ "assets/ibm.ch8" };
    const std::string testopc{ "assets/3-corax+.ch8" };
    const std::string testflags{ "assets/4-flags.ch8" };
    const std::string testquirks{ "assets/5-quirks.ch8" };
    const std::string testkeypad{ "assets/6-keypad.ch8" };
    const std::string rngTest{ "assets/rngTest.ch8" };

    // Works completely fine
    const std::string tetris{ "assets/tetris.ch8" };
    const std::string snake{ "assets/snake.ch8" };
    const std::string superPong{ "assets/superpong.ch8" };
    const std::string pong{ "assets/pong.ch8" };
    
    // Works, but need to increase IPF by 2-3x
    const std::string fallingGhosts{ "assets/ghosts.ch8" };

    // Also works but runs slowly, not sure if that is normal
    const std::string cellAuotmata1d{ "assets/1dcell.ch8" };

    // Raises E12 error - might be due to quirk
    const std::string bcTest{ "assets/BC_test.ch8" };

    // Doesnt fully work - likely due to quirks
    const std::string spaceInvaders{ "assets/Space Invaders [David Winter].ch8" };

    // Not sure if this works properly - dont know the controls and dont know where I found it
    const std::string puzzle15{ "assets/15puzzle.ch8" }

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

