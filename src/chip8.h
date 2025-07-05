#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <vector>
#include <SDL_events.h>

#include "utility.h"
#include "random.h"
#include "settings.h"


class Chip8
{
public:
    Chip8()
        : m_fontsLocation{ ChipConfig::fontsLocation }
    {
        loadFonts(m_fontsLocation);
    }

    template <std::size_t r, std::size_t c>
    using Array2DU8 = std::array<std::array<uint8_t, c>, r>;

    const Array2DU8<ChipConfig::screenHeight, ChipConfig::screenWidth>& getScreenBuffer() const { return m_screen; }

    void loadFile(const std::string name);

    void performFDECycle();
    void decrementTimers();

    void setKeyUp(std::size_t key);
    void setKeyDown(std::size_t key);
    void setPrevFrameInputs();

    // For debugging, prints contents of screen buffer to stdout
    void printScreenBuffer();

    static inline constexpr std::array<SDL_Scancode, 16> keyMap{
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
                           // On keypad
    };

    static_assert(std::size(keyMap) == 16);

    // WORK IN PROGRESS
    struct quirkFlags
    {
        bool shift{};
        bool index{};
        bool jump{};
        bool wrapScreen{};
        bool clearLogic{};
    };


private:
    // Given opcode with X, i.e 0x3XNN, extracts only the X nibble
    uint16_t extractX(uint16_t opcode) const { return Utility::toU16((opcode & 0x0F00) >> 8); }

    // Given opcode with Y, i.e 0x5XY0, extracts only the Y nibble
    uint16_t extractY(uint16_t opcode) const { return Utility::toU16((opcode & 0x00F0) >> 4); }

    // Given opcode with an N segment, i.e 0xDXYN, extracts only the N nibble
    uint16_t extractN(uint16_t opcode) const { return Utility::toU16(opcode & 0x000F); }

    // Given opcode with an NN segment, i.e 0x3XNN, extracts only the N byte
    uint16_t extractNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x00FF); }

    // Given opcode with an NNN segment, i.e 0x1NNN, extracts only the NNN segment
    uint16_t extractNNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x0FFF); }

    uint16_t fetchOpcode();
    void decodeAndExecute(uint16_t opcode);

    bool wasKeyReleasedThisFrame()
    {
        for (std::size_t i{ 0 }; i < std::size(m_keyDownThisFrame); ++i)
        {
            const bool keyUpThisFrame{ !m_keyDownThisFrame[i] };
            const bool keyDownLastFrame{ m_keyDownLastFrame[i] };

            if (keyDownLastFrame && keyUpThisFrame)
            {
                return true;
            }
        }  
        return false;
    }

    // Returns the *first* key it finds that was pressed down last frame and released this frame
    uint8_t findKeyReleasedThisFrame()
    {
        for (std::size_t i{ 0 }; i < std::size(m_keyDownThisFrame); ++i)
        {
            const bool keyUpThisFrame{ !m_keyDownThisFrame[i] };
            const bool keyDownLastFrame{ m_keyDownLastFrame[i] };

            if (keyDownLastFrame && keyUpThisFrame)
            {
                return Utility::toU8(i);
            }
        }
        assert(false && "Chip8::findKeyReleasedThisFrame called in context where a key was not released");
        return 0x10;
    }

    // Opcodes
    void op00E0();
    void op00EE();
    void op1NNN(uint16_t opcode);
    void op2NNN(uint16_t opcode);
    void op3XNN(uint16_t opcode);
    void op4XNN(uint16_t opcode);
    void op5XY0(uint16_t opcode);
    void op6XNN(uint16_t opcode);
    void op7XNN(uint16_t opcode);
    void op8XY0(uint16_t opcode);
    void op8XY1(uint16_t opcode);
    void op8XY2(uint16_t opcode);
    void op8XY3(uint16_t opcode);
    void op8XY4(uint16_t opcode);
    void op8XY5(uint16_t opcode);
    void op8XY6(uint16_t opcode);
    void op8XY7(uint16_t opcode);
    void op8XYE(uint16_t opcode);
    void op9XY0(uint16_t opcode);
    void opANNN(uint16_t opcode);
    void opBNNN(uint16_t opcode);
    void opCXNN(uint16_t opcode);
    void opDXYN(uint16_t opcode);
    void opEX9E(uint16_t opcode);
    void opEXA1(uint16_t opcode);
    void opFX07(uint16_t opcode);
    void opFX0A(uint16_t opcode);
    void opFX15(uint16_t opcode);
    void opFX18(uint16_t opcode);
    void opFX1E(uint16_t opcode);
    void opFX29(uint16_t opcode);
    void opFX33(uint16_t opcode);
    void opFX55(uint16_t opcode);
    void opFX65(uint16_t opcode);

    // Input handling
    bool isAKeyPressed();
    uint8_t findFirstPressedKey();

    void loadFonts(const uint16_t startLocation);

    // Memory, registers, and state
    std::array<uint8_t, 4096> m_memory{};

    std::array<uint8_t, 16> m_registers{};
    uint16_t m_pc{ ChipConfig::startAddress };
    uint16_t m_indexReg{ 0 };
    uint8_t m_delayTimer{ 0 };
    uint8_t m_soundTimer{ 0 };

    std::vector<uint16_t> m_stack{};

    Array2DU8 <ChipConfig::screenHeight, ChipConfig::screenWidth> m_screen{};

    // Need to keep track of both current and last frame so that we can detect when a key was released
    std::array<bool, 16> m_keyDownThisFrame{};
    std::array<bool, 16> m_keyDownLastFrame{};

    const uint16_t m_width{ ChipConfig::screenWidth };
    const uint16_t m_height{ ChipConfig::screenHeight };

    const std::array<uint8_t, 80> m_fonts {
                                         // Corresponds to sprite for...
        0xF0, 0x90, 0x90, 0x90, 0xF0,    // 0
        0x20, 0x60, 0x20, 0x20, 0x70,    // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,    // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,    // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,    // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,    // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,    // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,    // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,    // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,    // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,    // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,    // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,    // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,    // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,    // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,    // F
    
    };

    const uint16_t m_fontsLocation{ ChipConfig::fontsLocation };    
};


#endif