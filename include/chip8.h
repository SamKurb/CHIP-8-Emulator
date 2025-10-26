#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "utility.h"
#include "random.h"

class Chip8
{
public:
    struct QuirkFlags
    {
        bool resetVF{};
        bool index{};
        bool wrapScreen{};
        bool shift{};
        bool jump{};
        bool displayWait{};
    };

    struct RuntimeMetaData
    {
        bool romIsLoaded{ false };

        uint64_t numInstructionsExecuted{ 0 };

        uint16_t fontStartAddress{};
        uint16_t fontEndAddress{};

        uint16_t programStartAddress{};
        uint16_t programEndAddress{};
    };

    struct InitialConfig
    {
        // Do not change
        static constexpr int maxStackDepth{ 16 };
        static constexpr int numPixelsHorizontally{ 64 };
        static constexpr int numPixelsVertically{ 32 };
        static constexpr std::size_t bitsOfMemory{ 4096 };
        static constexpr uint8_t timerStartVal{ 0 };

        // Can change if you know what you are doing. Current values adhere to CHIP-8 spec/conventions
        static constexpr uint16_t programStartAddress{ 0x200 };
        static constexpr uint16_t fontsStartLocation{ 0x50 };
    };

    Chip8()
    : Chip8(baseChip8Quirks)
    {
    }

    Chip8(const QuirkFlags& quirks);

    template <std::size_t r, std::size_t c>
    using Array2DU8 = std::array<std::array<uint8_t, c>, r>;

    const Array2DU8<InitialConfig::numPixelsVertically, InitialConfig::numPixelsHorizontally>& getScreenBuffer() const;

    uint8_t getDelayTimer() const;
    uint8_t getSoundTimer() const;

    bool executedDXYN() const;
    void resetDXYNFlag();

    QuirkFlags& getEnabledQuirks();

    uint64_t getNumInstructionsExecuted() const { return m_runtimeMetaData.numInstructionsExecuted; }
    uint16_t getFontStartAddress() const { return m_runtimeMetaData.fontStartAddress; }
    uint16_t getFontEndAddress() const { return m_runtimeMetaData.fontEndAddress; }
    uint16_t getProgramStartAddress() const { return m_runtimeMetaData.programStartAddress; }
    uint16_t getProgramEndAddress() const { return m_runtimeMetaData.programEndAddress; }
    bool isRomLoaded() const { return m_runtimeMetaData.romIsLoaded; }

    int getTargetNumInstrPerSecond() const { return m_targetNumInstrPerSecond; }

    std::array<uint8_t, 4096> getMemoryContents() const { return m_memory; }
    std::array<uint8_t, 16> getRegisterContents() const { return m_registers; }
    uint16_t getPCAddress() const { return m_pc; }
    uint16_t getIndexRegisterContents() const { return m_indexReg; }
    std::array<bool, 16> getKeysDownThisFrame() const { return m_keyDownThisFrame; };

    const std::vector<uint16_t>& getStackContents() const { return m_stack; }

    void setTargetNumInstrPerSecond(int newTarget) { m_targetNumInstrPerSecond = newTarget; }


    void loadFile(const std::string& name);

    void performFDECycle();
    void executeInstructions(int count);
    void handleInvalidOpcode(const uint16_t opcode);

    void decrementTimers();

    void setKeyUp(std::size_t key);
    void setKeyDown(std::size_t key);
    void setPrevFrameInputs();

    // We increment by 2 because memeory is 1 bytes per location but instructions are 2 bytes each, so to get to the next instruction PC needs to be icremeneted by 2 rather than 1
    void incrementPC() { m_pc += 2; };

    // For debugging, prints contents of screen buffer to stdout
    void printScreenBuffer();

private:
    // Given opcode with X, i.e. 0x3XNN, extracts only the X nibble
    uint16_t extractX(uint16_t opcode) const { return Utility::toU16((opcode & 0x0F00) >> 8); }

    // Given opcode with Y, i.e. 0x5XY0, extracts only the Y nibble
    uint16_t extractY(uint16_t opcode) const { return Utility::toU16((opcode & 0x00F0) >> 4); }

    // Given opcode with an N segment, i.e. 0xDXYN, extracts only the N nibble
    uint16_t extractN(uint16_t opcode) const { return Utility::toU16(opcode & 0x000F); }

    // Given opcode with an NN segment, i.e. 0x3XNN, extracts only the NN Byte
    uint16_t extractNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x00FF); }

    // Given opcode with an NNN segment, i.e. 0x1NNN, extracts only the NNN segment
    uint16_t extractNNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x0FFF); }

    uint16_t fetchOpcode();
    void decodeAndExecute(uint16_t opcode);

    bool wasKeyReleasedThisFrame() const;

    // Returns the *first* key it finds that was pressed down last frame and released this frame
    uint8_t findKeyReleasedThisFrame() const;

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

    // Wrapper functions to access memory, so that OOB is checked for. Will add logging in the future
    template <typename T>
    uint8_t readMemory(T location)
    {
        static_assert(std::is_unsigned<T>::value);

        if (location >= std::size(m_memory))
        {
            std::cout << "Attempted to access OOB memory\n" << std::endl;
            std::exit(1);
        }

        return m_memory[location];
    }

    template <typename T>
    void writeToMemory(T location, uint8_t value)
    {
        static_assert(std::is_unsigned<T>::value);

        if (location > std::size(m_memory))
        {
            std::cout << "Attempted to write to OOB memory\n" << std::endl;
            std::exit(1);
        }

        m_memory[location] = value;
    }

    // Input handling
    bool isAKeyPressed();
    uint8_t findFirstPressedKey();

    void loadFonts(const uint16_t startLocation);

    // Memory, registers, and state
    std::array<uint8_t, 4096> m_memory{};

    std::array<uint8_t, 16> m_registers{};
    uint16_t m_pc{ InitialConfig::programStartAddress };
    uint16_t m_indexReg{ 0 };
    uint8_t m_delayTimer{ 0 };
    uint8_t m_soundTimer{ 0 };

    static constexpr uint8_t s_timerDecrementsPerSecond { 60 };

    // At 720 IPS, it will be as if the chip8 is doing 12 instructions per frame @ 60 FPS, which is the standard
    int m_targetNumInstrPerSecond{ 720 };

    std::vector<uint16_t> m_stack{};

    Array2DU8 <InitialConfig::numPixelsVertically, InitialConfig::numPixelsHorizontally> m_screen{};

    // Need to keep track of inputs from both current and last frame so that we can detect when a key was released
    std::array<bool, 16> m_keyDownThisFrame{};
    std::array<bool, 16> m_keyDownLastFrame{};

    uint16_t m_width{ InitialConfig::numPixelsHorizontally };
    uint16_t m_height{ InitialConfig::numPixelsVertically };

    std::array<uint8_t, 80> m_fonts {
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

    uint16_t m_fontsLocation{ InitialConfig::fontsStartLocation };
    
    // Used for implementing the display wait quirk. Assumption is that whenever execution of instructions is interrupted to draw a frame, this flag is reset back to false.
    bool m_executedDXYNFlag{ false };

	// Quirk configurations (We alter the functionality of certain opcodes based on whether or not a quirk is enabled)
    QuirkFlags m_isQuirkEnabled{};
    RuntimeMetaData m_runtimeMetaData{};

    static constexpr QuirkFlags baseChip8Quirks {
        true,   // reset register VF on bitwise AND/OR/XOR operation
        true,   // index register quirk
        false,  // wrap around screen quirk
        false,  // shift quirk
        false,  // jump quirk
        false,  // display wait quirk
    };

    // FOR TESTING WITH testQuirks ROM
    static constexpr QuirkFlags superChipQuirks{
        false,  // reset register VF on bitwise AND/OR/XOR operation
        false,  // index register quirk
        false,  // wrap around screen quirk
        true,   // shift quirk
        true,   // jump quirk
        false,  // display wait quirk
    };
};


#endif