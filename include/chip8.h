#ifndef CHIP8_H
#define CHIP8_H


#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "exceptions/chipoobmemoryaccessexception.h"
#include "utils/utility.h"

#include "types/enumarray.h"

class Chip8
{
public:
    enum class KeyInputs
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
        MAX_VALUE,
    };

    struct QuirkFlags
    {
        bool resetVF{};
        bool index{};
        bool wrapScreen{};
        bool shift{};
        bool jump{};
        bool displayWait{};
        bool haltOnOOBAccess{};
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

    uint64_t getNumInstructionsExecuted() const;
    uint16_t getFontStartAddress() const;
    uint16_t getFontEndAddress() const;
    uint16_t getProgramStartAddress() const;
    uint16_t getProgramEndAddress() const;
    bool isRomLoaded() const;

    int getTargetNumInstrPerSecond() const;
    std::array<uint8_t, 4096> getMemoryContents() const;
    std::array<uint8_t, 16> getRegisterContents() const;
    uint16_t getPCAddress() const;
    uint16_t getIndexRegisterContents() const;

    EnumArray<KeyInputs, bool> getKeysDownThisFrame() const;

    const std::vector<uint16_t>& getStackContents() const;

    void setTargetNumInstrPerSecond(int newTarget);


    void loadFile(const std::string& name);

    void performFDECycle();
    void executeInstructions(int count);
    void handleInvalidOpcode(const uint16_t opcode);

    void decrementTimers();

    void setKeyUp(KeyInputs key);
    void setKeyDown(KeyInputs key);
    void setPrevFrameInputs();

    // We increment by 2 because memeory is 1 bytes per location but instructions are 2 bytes each, so to get to the next instruction PC needs to be icremeneted by 2 rather than 1
    void incrementPC() { m_pc += 2; };

private:
    // Given opcode with X, i.e. 0x3XNN, extracts only the X nibble
    [[nodiscard]] uint16_t extractX(uint16_t opcode) const { return Utility::toU16((opcode & 0x0F00) >> 8); }

    // Given opcode with Y, i.e. 0x5XY0, extracts only the Y nibble
    [[nodiscard]] uint16_t extractY(uint16_t opcode) const { return Utility::toU16((opcode & 0x00F0) >> 4); }

    // Given opcode with an N segment, i.e. 0xDXYN, extracts only the N nibble
    [[nodiscard]] uint16_t extractN(uint16_t opcode) const { return Utility::toU16(opcode & 0x000F); }

    // Given opcode with an NN segment, i.e. 0x3XNN, extracts only the NN Byte
    [[nodiscard]] uint16_t extractNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x00FF); }

    // Given opcode with an NNN segment, i.e. 0x1NNN, extracts only the NNN segment
    [[nodiscard]] uint16_t extractNNN(uint16_t opcode) const { return Utility::toU16(opcode & 0x0FFF); }

    uint16_t fetchOpcode();
    void decodeAndExecute(uint16_t opcode);

    bool wasKeyReleasedThisFrame() const;

    // Returns the *first* key it finds that was pressed down last frame and released this frame
    KeyInputs findKeyReleasedThisFrame() const;

    // Opcodes
    void executeOp00E0();
    void executeOp00EE();
    void executeOp1NNN(uint16_t opcode);
    void executeOp2NNN(uint16_t opcode);
    void executeOp3XNN(uint16_t opcode);
    void executeOp4XNN(uint16_t opcode);
    void executeOp5XY0(uint16_t opcode);
    void executeOp6XNN(uint16_t opcode);
    void executeOp7XNN(uint16_t opcode);
    void executeOp8XY0(uint16_t opcode);
    void executeOp8XY1(uint16_t opcode);
    void executeOp8XY2(uint16_t opcode);
    void executeOp8XY3(uint16_t opcode);
    void executeOp8XY4(uint16_t opcode);
    void executeOp8XY5(uint16_t opcode);
    void executeOp8XY6(uint16_t opcode);
    void executeOp8XY7(uint16_t opcode);
    void executeOp8XYE(uint16_t opcode);
    void executeOp9XY0(uint16_t opcode);
    void executeOpANNN(uint16_t opcode);
    void executeOpBNNN(uint16_t opcode);
    void executeOpCXNN(uint16_t opcode);

    // DXYN helper
    void drawSprite(uint8_t xCoord, uint8_t yCoord, uint16_t spriteWidth, uint16_t spriteHeight, uint16_t currAddress);

    void executeOpDXYN(uint16_t opcode);
    void executeOpEX9E(uint16_t opcode);
    void executeOpEXA1(uint16_t opcode);
    void executeOpFX07(uint16_t opcode);
    void executeOpFX0A(uint16_t opcode);
    void executeOpFX15(uint16_t opcode);
    void executeOpFX18(uint16_t opcode);
    void executeOpFX1E(uint16_t opcode);
    void executeOpFX29(uint16_t opcode);
    void executeOpFX33(uint16_t opcode);
    void executeOpFX55(uint16_t opcode);
    void executeOpFX65(uint16_t opcode);

    // Wrapper functions to access memory, so that OOB is checked for. Will add logging in the future
    template <typename T>
    uint8_t readMemory(T location)
    {
        static_assert(std::is_unsigned<T>::value);

        if (m_isQuirkEnabled.haltOnOOBAccess && location > std::size(m_memory))
        {
            std::string errorMsg{ " Attempted to read from OOB memory in ROM!" };
            throw ChipOOBMemoryAccessException(errorMsg);
        }

        const std::size_t wrappedLocation{ location % m_memory.size() };
        return m_memory[wrappedLocation];
    }

    template <typename T>
    void writeToMemory(T location, uint8_t value)
    {
        static_assert(std::is_unsigned<T>::value);

        if (m_isQuirkEnabled.haltOnOOBAccess && location > std::size(m_memory))
        {
            std::string errorMsg{ " Attempted to write to OOB memory in ROM!" };
            throw ChipOOBMemoryAccessException(errorMsg);
        }

        const std::size_t wrappedLocation{ location % m_memory.size() };
        m_memory[wrappedLocation] = value;
    }

    // Input handling
    bool isAKeyPressed();
    Chip8::KeyInputs findFirstPressedKey();

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
    EnumArray<KeyInputs, bool> m_keyDownThisFrame{};
    EnumArray<KeyInputs, bool> m_keyDownLastFrame{};

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
        false,
    };

    // FOR TESTING WITH testQuirks ROM
    static constexpr QuirkFlags superChipQuirks{
        false,  // reset register VF on bitwise AND/OR/XOR operation
        false,  // index register quirk
        false,  // wrap around screen quirk
        true,   // shift quirk
        true,   // jump quirk
        false,  // display wait quirk
        false,
    };
};


#endif