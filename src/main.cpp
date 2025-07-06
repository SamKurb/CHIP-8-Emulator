#include <iostream>

#include <iomanip>
#include "renderer.h"
#include "utility.h"
#include "random.h"
#include "chip8.h"
#include "settings.h"
#include "audioplayer.h"

namespace ROMS
{
	const std::string brix{ "roms/brix.ch8" };
    const std::string hidden{ "roms/Hidden [David Winter, 1996].ch8" };
    const std::string danm8ku{ "roms/danm8ku.ch8" };

    // Currently works
    const std::string ibm{ "roms/ibm.ch8" };
    const std::string testOpc{ "roms/3-corax+.ch8" };
    const std::string testFlags{ "roms/4-flags.ch8" };
    const std::string testQuirks{ "roms/5-quirks.ch8" };
    const std::string testKeypad{ "roms/6-keypad.ch8" };
    const std::string testSound{ "roms/7-beep.ch8" };
    const std::string rngTest{ "roms/rngTest.ch8" };

    // Works completely fine
    const std::string tetris{ "roms/tetris.ch8" };
    const std::string snake{ "roms/snake.ch8" };
    const std::string superPong{ "roms/superpong.ch8" };
    const std::string pong{ "roms/pong.ch8" };

    // Doesnt fully work - likely due to quirks
    const std::string spaceInvaders{ "roms/Space Invaders [David Winter].ch8" };
    
    // Works, but need to increase IPF by 2-3x
    const std::string fallingGhosts{ "roms/ghosts.ch8" };

    // Also works but runs slowly, not sure if that is normal
    const std::string cellAuotmata1d{ "roms/1dcell.ch8" };

    // Raises E12 error - not an issue with the emulator. Test is outdated/incorrect
    const std::string bcTest{ "roms/BC_test.ch8" };



    // Not sure if this works properly - dont know the controls and dont know where I found it
    const std::string puzzle15{ "roms/15puzzle.ch8" };
}

struct FrameInfo
{
    Uint32 startTimeMs{};
    Uint32 endTimeMs{};
    Uint32 timeElapsedMs{};
};

int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    constexpr Chip8::QuirkFlags baseChip8Quirks {
        true,   // reset register VF on bitwise AND/OR/XOR operation
        true,   // index register quirk
        false,  // wrap around screen quirk
        false,  // shift quirk
        false,  // jump quirk
    };

    // FOR TESTING WITH testQuirks ROM
    constexpr Chip8::QuirkFlags superChipQuirks{
        false,  // reset register VF on bitwise AND/OR/XOR operation
        false,  // index register quirk
        false,  // wrap around screen quirk
        true,   // shift quirk
        true,   // jump quirk
    };

    Chip8 chip{ baseChip8Quirks };


    Renderer renderer {
        DisplayConfig::resolutionWidth,
        DisplayConfig::resolutionHeight, 
        !DisplayConfig::displayGrid,
        DisplayConfig::offColour, 
        DisplayConfig::onColour
    };

    chip.loadFile(ROMS::testQuirks);

    
    SDL_Event e{};

    bool userHasQuit{ false };
    
    FrameInfo frameInfo{};

    // For a target fps of 60 this will be 16ms (rounded down because it is an int), so we will actually be rendering roughly 62-63 frames rather than 60.
    const int targetFrameDelayMs{ 1000 / DisplayConfig::targetFPS };

    AudioPlayer audio{ "assets/beep.wav" };

    while (!userHasQuit)
    {
        frameInfo.startTimeMs = SDL_GetTicks();
        
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                userHasQuit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    userHasQuit = true;
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

        for (int i = 0; i < ChipConfig::instrPerFrame - 5; ++i)
        {
            chip.performFDECycle();
        }

        if (chip.getSoundTimer() > 0)
        {
            audio.playSound();
        }

        renderer.drawToScreen(chip.getScreenBuffer());
        renderer.render();
            
        chip.setPrevFrameInputs();

        frameInfo.endTimeMs = SDL_GetTicks();
        frameInfo.timeElapsedMs = frameInfo.endTimeMs - frameInfo.startTimeMs;

        // Frames may process faster than the target frametime, so we delay to make sure that we only move on to the next frame when enough time has passed
        if (frameInfo.timeElapsedMs < targetFrameDelayMs)
        {
            SDL_Delay(targetFrameDelayMs - frameInfo.timeElapsedMs);
        }
    }

    return 0;
}

