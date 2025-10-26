#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL_events.h>
#include <array>
#include "types/enumarray.h"

#include "chip8.h"

class InputHandler
{
public:
    enum class SystemKeyInputs
    {
        K_QUIT,

        K_ACTIVATE_DEBUG,

        // Can only be pressed if debug mode is active
        K_ACTIVATE_STEP,
        K_NEXT_FRAME,

        K_ACTIVATE_MANUAL,
        K_NEXT_INSTRUCTION,

        K_DEACTIVATE_DEBUG,
        K_TOGGLE_DEBUG_WINDOWS,
        MAX_VALUE,
    };

    void readSystemInputs();

    void readChipAndSystemInputs(Chip8& chip);

    bool isSystemKeyPressed(const SystemKeyInputs key) const { return m_isSystemKeyPressed[key]; }

    void resetSystemKeysState() { std::fill(m_isSystemKeyPressed.begin(),
                                             m_isSystemKeyPressed.end(), false); }

private:
    static constexpr EnumArray<Chip8::KeyInputs, SDL_Scancode> chipKeyMap {{
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
    }};

    static constexpr EnumArray<SystemKeyInputs, SDL_Scancode> systemKeyMap {{
                              // Corresponds to...
        SDL_SCANCODE_ESCAPE,  // Quit emulator

        SDL_SCANCODE_6,       // Activate debug mode 

        SDL_SCANCODE_7,       // Activate step mode
        SDL_SCANCODE_SPACE,   // Step through a frame

        SDL_SCANCODE_8,       // Activate manual mode
        SDL_SCANCODE_I,       // Step through an instruction

        SDL_SCANCODE_0,        // Deactivate debug mode

        SDL_SCANCODE_G         // Toggle debug windows
    }};

    void checkForChipInput(const SDL_Event& event, Chip8& chip);
    void checkForSystemInput(const SDL_Event event);

    EnumArray<SystemKeyInputs, bool> m_isSystemKeyPressed{};
};

#endif