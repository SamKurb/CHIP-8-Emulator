#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL_events.h>
#include <array>

#include "chip8.h"

class InputHandler
{
public:

    enum SystemKeyInputs
    {
        K_QUIT,

        K_ACTIVATE_DEBUG,

        // Can only be pressed if debug mode is active
        K_ACTIVATE_STEP,
        K_NEXT_FRAME,

        K_ACTIVATE_MANUAL,
        K_NEXT_INSTRUCTION,

        K_DEACTIVATE_DEBUG,
        numSystemKeys,
    };

    void readChipAndSystemInputs(Chip8& chip) 
    {
        SDL_Event event{};

        while (SDL_PollEvent(&event) != 0)
        {            
            checkForChipInput(event, chip);
            checkForSystemInput(event);
        }
    }

    bool isSystemKeyPressed(SystemKeyInputs key) const
    {
        return m_isSystemKeyPressed[key];
    }

    void resetSystemKeysState()
    {
        std::fill(m_isSystemKeyPressed.begin(), m_isSystemKeyPressed.end(), false);
    }

private:
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

    static inline constexpr std::array chipKeyMap {
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

    static inline constexpr std::array systemKeyMap{
                              // Corresponds to...
        SDL_SCANCODE_ESCAPE,  // Quit emulator

        SDL_SCANCODE_6,       // Activate debug mode 

        SDL_SCANCODE_7,       // Activate step mode
        SDL_SCANCODE_SPACE,   // Step through a frame

        SDL_SCANCODE_8,       // Activate manual mode
        SDL_SCANCODE_I,       // Step through an instruction

        SDL_SCANCODE_0        // Deactivate debug mode
    };

    static_assert(std::size(systemKeyMap) == numSystemKeys);

    std::array<bool, numSystemKeys> m_isSystemKeyPressed{};


    void checkForChipInput(const SDL_Event event, Chip8& chip)
    {
        if (event.type == SDL_KEYDOWN)
        {
            for (std::size_t i{ 0 }; i < numChipKeys; ++i)
            {
                if (event.key.keysym.scancode == chipKeyMap[i])
                { 
                    chip.setKeyDown(i);
                }
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            for (std::size_t i{ 0 }; i < numChipKeys; ++i)
            {
                if (event.key.keysym.scancode == chipKeyMap[i])
                {
                    chip.setKeyUp(i);
                }
            }
        }
    }

    void checkForSystemInput(const SDL_Event event)
    {
        if (event.type != SDL_KEYDOWN) { return; }

        for (std::size_t i{ 0 }; i < numSystemKeys; ++i)
        {
            if (event.key.keysym.scancode == systemKeyMap[i])
            {
                m_isSystemKeyPressed[i] = true;
            }
        }
    }


};

#endif