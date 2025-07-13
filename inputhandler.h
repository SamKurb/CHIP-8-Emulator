#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL_events.h>
#include <array>
#include "chip8.h"

class InputHandler
{
public:

    void readInputs(Chip8&)
    {
        SDL_Event event{};
        bool userHasQuit{ false };

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                userHasQuit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    userHasQuit = true;
                }

                for (std::size_t i{ 0 }; i < 16; ++i)
                {
                    if (event.key.keysym.scancode == chipKeyMap[i])
                    {
                        chip.setKeyDown(i);
                    }
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                for (std::size_t i{ 0 }; i < 16; ++i)
                {
                    if (event.key.keysym.scancode == chipKeyMap[i])
                    {
                        chip.setKeyUp(i);
                    }
                }
            }
        }
    }

private:
    // Keys directly related to the CHIP8 input system implementation
    enum ChipKeyInputs
    {
        K_0,
        K_1,
        K_2,
        K_3,
        K_4,
        K_5,
        K_6,
        K_7,
        K_8,
        K_9,
        K_A,
        K_B,
        K_C,
        K_D,
        K_E,
        K_F,
        numChipKeys,
    };

    static inline constexpr std::array<SDL_Scancode, 16> chipKeyMap{
                           // Corresponds to...
        SDL_SCANCODE_X,    // 0 
        SDL_SCANCODE_1,    // 1 
        SDL_SCANCODE_2,    // 2 
        SDL_SCANCODE_3,    // 3
        SDL_SCANCODE_Q,    // 4
        SDL_SCANCODE_W,    // 5
        SDL_SCANCODE_E,    // 6
        SDL_SCANCODE_A,    // 7
        SDL_SCANCODE_S,    // 8
        SDL_SCANCODE_D,    // 9
        SDL_SCANCODE_Z,    // A
        SDL_SCANCODE_C,    // B
        SDL_SCANCODE_4,    // C
        SDL_SCANCODE_R,    // D
        SDL_SCANCODE_F,    // E
        SDL_SCANCODE_V,    // F
                           // On CHIP-8 hex keypad
    };

    static_assert(std::size(chipKeyMap) == numChipKeys);

    // Keys that are not directly related to the CHIP8 implementation. Used for things like exiting emulator or pausing
    enum SystemKeyInputs
    {
        K_ESC,   // Escape key to quit
        K_SPACE,
        numSystemKeys
    };

   
};

#endif