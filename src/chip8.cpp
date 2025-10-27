#include "chip8.h"

#include "../include/exceptions/badopcodeexception.h"
#include "../include/exceptions/fileinputexception.h"
#include "../include/exceptions/chipstackerrorexception.h"

#include "../include/utils/random.h"

#include <ranges>
#include <algorithm>
#include <utility>

Chip8::Chip8(const QuirkFlags& quirks)
: m_fontsLocation{ InitialConfig::fontsStartLocation }
, m_isQuirkEnabled{ quirks }
, m_runtimeMetaData{}
{
    m_stack.reserve(InitialConfig::maxStackDepth);
    loadFonts(m_fontsLocation);
}

const Chip8::Array2DU8<Chip8::InitialConfig::numPixelsVertically, Chip8::InitialConfig::numPixelsHorizontally>&
    Chip8::getScreenBuffer() const
{
    return m_screen;
}


uint8_t Chip8::getDelayTimer() const { return m_delayTimer; }
uint8_t Chip8::getSoundTimer() const { return m_soundTimer; }

bool Chip8::executedDXYN() const { return m_executedDXYNFlag; }
void Chip8::resetDXYNFlag() { m_executedDXYNFlag = false; }

Chip8::QuirkFlags& Chip8::getEnabledQuirks() { return m_isQuirkEnabled; }

uint64_t Chip8::getNumInstructionsExecuted() const { return m_runtimeMetaData.numInstructionsExecuted; }
uint16_t Chip8::getFontStartAddress() const { return m_runtimeMetaData.fontStartAddress; }
uint16_t Chip8::getFontEndAddress() const { return m_runtimeMetaData.fontEndAddress; }
uint16_t Chip8::getProgramStartAddress() const { return m_runtimeMetaData.programStartAddress; }
uint16_t Chip8::getProgramEndAddress() const { return m_runtimeMetaData.programEndAddress; }
bool Chip8::isRomLoaded() const { return m_runtimeMetaData.romIsLoaded; }

int Chip8::getTargetNumInstrPerSecond() const { return m_targetNumInstrPerSecond; }

std::array<uint8_t, 4096> Chip8::getMemoryContents() const { return m_memory; }
std::array<uint8_t, 16> Chip8::getRegisterContents() const { return m_registers; }
uint16_t Chip8::getPCAddress() const { return m_pc; }
uint16_t Chip8::getIndexRegisterContents() const { return m_indexReg; }
EnumArray<Chip8::KeyInputs, bool> Chip8::getKeysDownThisFrame() const { return m_keyDownThisFrame; };

const std::vector<uint16_t>& Chip8::getStackContents() const { return m_stack; }

void Chip8::setTargetNumInstrPerSecond(int newTarget) { m_targetNumInstrPerSecond = newTarget; }


void Chip8::handleInvalidOpcode(const uint16_t opcode)
{
    std::string opcodeAsString { std::format("{:X}", opcode) };
    throw BadOpcodeException("Invald opcode! Opcode: " + opcodeAsString);
}

uint16_t Chip8::fetchOpcode()
{
    const uint16_t firstOpHalf{ readMemory(m_pc) };
    const uint16_t secondOpHalf{ readMemory(m_pc + 1u) };

    incrementPC();

    const uint16_t opcode{ Utility::toU16((firstOpHalf << 8) | secondOpHalf) };
    return opcode;
}

void Chip8::decodeAndExecute(const uint16_t opcode)
{
    switch (opcode & 0xF000)
    {
    case 0x1000:
        executeOp1NNN(opcode);
        break;
    case 0x2000:
            executeOp2NNN(opcode);
        break;
    case 0x3000:
        executeOp3XNN(opcode);
        break;
    case 0x4000:
        executeOp4XNN(opcode);
        break;
    case 0x5000:
        executeOp5XY0(opcode);
        break;
    case 0x6000:
        executeOp6XNN(opcode);
        break;
    case 0x7000:
        executeOp7XNN(opcode);
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            executeOp8XY0(opcode);
            break;
        case 0x0001:
            executeOp8XY1(opcode);
            break;
        case 0x0002:
            executeOp8XY2(opcode);
            break;
        case 0x0003:
            executeOp8XY3(opcode);
            break;
        case 0x0004:
            executeOp8XY4(opcode);
            break;
        case 0x0005:
            executeOp8XY5(opcode);
            break;
        case 0x0006:
            executeOp8XY6(opcode);
            break;
        case 0x0007:
            executeOp8XY7(opcode);
            break;
        case 0x000E:
            executeOp8XYE(opcode);
            break;
        default:
            handleInvalidOpcode(opcode);
        }
        break;

    case 0x9000:
        executeOp9XY0(opcode);
        break;
    case 0xA000:
        executeOpANNN(opcode);
        break;
    case 0xB000:
        executeOpBNNN(opcode);
        break;
    case 0xC000:
        executeOpCXNN(opcode);
        break;
    case 0xD000:
        executeOpDXYN(opcode);
        break;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            executeOpEX9E(opcode);
            break;
        case 0x00A1:
            executeOpEXA1(opcode);
            break;
        default:
            handleInvalidOpcode(opcode);
            break;
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            executeOpFX07(opcode);
            break;
        case 0x000A:
            executeOpFX0A(opcode);
            break;
        case 0x0015:
            executeOpFX15(opcode);
            break;
        case 0x0018:
            executeOpFX18(opcode);
            break;
        case 0x001E:
            executeOpFX1E(opcode);
            break;
        case 0x0029:
            executeOpFX29(opcode);
            break;
        case 0x0033:
            executeOpFX33(opcode);
            break;
        case 0x0055:
            executeOpFX55(opcode);
            break;
        case 0x0065:
            executeOpFX65(opcode);
            break;
        default:
            handleInvalidOpcode(opcode);
            break;
        }
        break;

    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            executeOp00E0();
            break;
        case 0x00EE:
            executeOp00EE();
            break;
        default:
            handleInvalidOpcode(opcode);
            break;
        }
        break;

    default:
        handleInvalidOpcode(opcode);
        break;
    }

    m_runtimeMetaData.numInstructionsExecuted += 1;
}

bool Chip8::wasKeyReleasedThisFrame() const
{
    return std::ranges::any_of(
        std::views::zip(m_keyDownLastFrame, m_keyDownThisFrame),
        [](const auto& keyStates)
        {
            const auto& [wasKeyDownLastFrame, isKeyDownThisFrame] = keyStates;
            const bool isKeyUpThisFrame{ !isKeyDownThisFrame };
            return wasKeyDownLastFrame && isKeyUpThisFrame;
        }
    );
}

Chip8::KeyInputs Chip8::findKeyReleasedThisFrame() const
{
    auto currAndLastFrameKeyStates { std::views::zip(m_keyDownThisFrame.data(), m_keyDownLastFrame.data()) };
    for (auto [index, prevAndCurrKeyState] : std::views::enumerate(currAndLastFrameKeyStates))
    {
        auto [isKeyDownThisFrame, wasKeyDownLastFrame] { prevAndCurrKeyState };
        const bool isKeyUpThisFrame{ !isKeyDownThisFrame };
        if (wasKeyDownLastFrame && isKeyUpThisFrame)
        {
            return static_cast<KeyInputs>(index);
        }
    }
    assert(false && "Chip8::findKeyReleasedThisFrame called in context where a key was not released");
}

void Chip8::decrementTimers()
{
    if (m_soundTimer > 0)
    {
        m_soundTimer -= 1;
    }

    if (m_delayTimer > 0)
    {
        m_delayTimer -= 1;
    }
}

void Chip8::performFDECycle()
{
    uint16_t opcode{ fetchOpcode() };
    decodeAndExecute(opcode);
}

void Chip8::executeInstructions(int count)
{
    for (int i{ 0 } ; i < count ; ++i)
    {
        performFDECycle();

        if (m_isQuirkEnabled.displayWait && executedDXYN())
        {
            resetDXYNFlag();
            break;
        }
    }
}

/*
    All opcodes in the order they are mentioned in:
    The wikipedia page: https://en.wikipedia.org/wiki/CHIP-8
    CG's reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
*/
void Chip8::executeOp00E0()
{
    const int valueForOffPixel{ 0 };
    for (auto& row : m_screen)
    {
        std::ranges::fill(row, valueForOffPixel);
    }
}

void Chip8::executeOp00EE()
{
    if (std::size(m_stack) == 0)
    {
        std::string errorMsg { "Attempt to pop from empty stack in execution of opcode 00EE. "
                               "Ensure the ROM is not buggy!" };
        throw ChipStackErrorException(errorMsg);
    }

    m_pc = m_stack.back();
    m_stack.pop_back();
}

void Chip8::executeOp1NNN(const uint16_t opcode)
{
    const uint16_t address{ extractNNN(opcode) };
    m_pc = address;
}

void Chip8::executeOp2NNN(const uint16_t opcode)
{
    const uint16_t address{ extractNNN(opcode) };

    if (std::size(m_stack) >= InitialConfig::maxStackDepth)
    {
        std::string errorMsg { "Attempt to push to full stack in execution of opcode 2NNN. "
                               "Ensure the ROM is not buggy!" };
        throw ChipStackErrorException(errorMsg);
    }

    m_stack.push_back(m_pc);
    m_pc = address;
}

void Chip8::executeOp3XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };

    if (m_registers[regNum] == valueToCompare)
    {
        incrementPC();
    }
}

void Chip8::executeOp4XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };

    if (m_registers[regNum] != valueToCompare)
    {
        incrementPC();
    }
}

void Chip8::executeOp5XY0(const uint16_t opcode)
{
    const  uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    if (m_registers[regX] == m_registers[regY])
    {
        incrementPC();;
    }
}

void Chip8::executeOp6XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToPut{ Utility::toU8(extractNN(opcode)) };
    m_registers[regNum] = valueToPut;
}

void Chip8::executeOp7XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToAdd{ Utility::toU8(extractNN(opcode)) };

    m_registers[regNum] += valueToAdd;
}

void Chip8::executeOp8XY0(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] = m_registers[regY];
}

/*
|==== QUIRK-RELATED INSTRUCTIONs ====|
The next 3 instructions (8XY1, 8XY2, 8XY3), which are the AND, OR and XOR opcodes respectively are similar/related to eachother
and all have a quirk version that alters behaviour in the same way

--With the quirk *disabled*:
Register VF is left untouched through the operation

--With the quirk *enabled*:
Register VF is always set to 0 at the end of the instruction
*/
void Chip8::executeOp8XY1(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] |= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::executeOp8XY2(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] &= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::executeOp8XY3(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] ^= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::executeOp8XY4(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint16_t sum{ Utility::toU16(m_registers[regX] + m_registers[regY]) };

    m_registers[regX] = Utility::toU8(sum & 0xFF);

    m_registers[0xF] = sum > 255;
}

void Chip8::executeOp8XY5(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint8_t subtractionResult{ Utility::toU8(m_registers[regX] - m_registers[regY]) };

    const bool noBorrow{ m_registers[regX] >= m_registers[regY] };

    m_registers[regX] = subtractionResult;

    m_registers[0xF] = noBorrow;
}


/*
|==== QUIRK-RELATED INSTRUCTION ====|
--With the quirk *disabled*:
At the end of the instruction, register X will hold the value of register Y's value shifted to the *right* by 1. e.g. regX = regY >> 1

--With the quirk *enabled*:
At the end of the instruction, register X will hold its own value but shifted to the *right* by 1. e.g. regX >>= 1
So we ignore Y entirely (In CHIP8 code, the assumption is usually that X == Y at all times)

Note: Register VF needs to be assigned to at the very end incase register VF happens to be register X. Otherwise, instead of storing whether or not overflow occured,
it would store the result of the shift which is not intended.
*/
void Chip8::executeOp8XY6(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    if (!m_isQuirkEnabled.shift)
    {
        const uint16_t regY{ extractY(opcode) };
        m_registers[regX] = m_registers[regY];
    }

    bool bitShiftedOut{ (m_registers[regX] & 0x01) == 1 };

    m_registers[regX] >>= 1;
    m_registers[0xF] = bitShiftedOut;
}

void Chip8::executeOp8XY7(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint8_t subtractionResult{ Utility::toU8(m_registers[regY] - m_registers[regX]) };

    const bool noBorrow{ m_registers[regY] >= m_registers[regX] };

    m_registers[regX] = subtractionResult;

    m_registers[0xF] = noBorrow;
}

/*
|==== QUIRK-RELATED INSTRUCTION ====|
--With the quirk *disabled*:
At the end of the instruction, register X will hold the value of register Y's value shifted to the *left* by 1. e.g. regX = regY << 1

--With the quirk *enabled*:
At the end of the instruction, register X will hold its own value but shifted to the *left* by 1. e.g. regX <<= 1
So we ignore Y entirely (In CHIP8 code, the assumption is usually that X == Y at all times)

Note: Register VF needs to be assigned to at the very end incase register VF happens to be register X. Otherwise, instead of storing whether or not overflow occured,
it would store the result of the shift which is not intended.
*/

void Chip8::executeOp8XYE(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    if (!m_isQuirkEnabled.shift)
    {
        const uint16_t regY{ extractY(opcode) };
        m_registers[regX] = m_registers[regY];
    }

    // is MSB == 1?
    bool bitShiftedOut{ (m_registers[regX] & 0b1000'0000) == 128 };

    m_registers[regX] <<= 1;
    m_registers[0xF] = bitShiftedOut;
}

void Chip8::executeOp9XY0(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    if (m_registers[regX] != m_registers[regY])
    {
        incrementPC();
    }
}

void Chip8::executeOpANNN(const uint16_t opcode)
{
    const uint16_t addressToSet{ extractNNN(opcode)};

    m_indexReg = addressToSet;
}


/*
|==== QUIRK-RELATED INSTRUCTION ====|
--With the quirk *disabled*:
PC is set to the address NNN + the value stored in register 0 (always register 0, no matter what)

--With the quirk *enabled*:
PC is set to the address NNN + register X, where X is the first nibble in NNN i.e the leftmost N.
                                                                          |
                                                                        this one
*/
void Chip8::executeOpBNNN(const uint16_t opcode)
{
	if (!m_isQuirkEnabled.jump)
    {
        const uint16_t address{ extractNNN(opcode) };
        m_pc = Utility::toU16(address + m_registers[0x0]);
    }
    else
    {
		const uint16_t regX{ extractX(opcode) };
		const uint16_t address{ extractNNN(opcode) };
		m_pc = Utility::toU16(address + m_registers[regX]);
    }
}

void Chip8::executeOpCXNN(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t valueToAnd{ Utility::toU8(extractNN(opcode)) };

    const uint8_t randomByte{ Utility::toU8(Random::get(0, 255)) };

    m_registers[regX] = Utility::toU8(randomByte & valueToAnd);
}

/*
|==== QUIRK-RELATED INSTRUCTION (2 quirks) ====|
--With the quirk *disabled*:
There are 2 quirks related to this instruction. The wrapscreen quirk is "directly" related to it, while the displayWait quirk is related to it due to my implementation.

For the wrap screen quirk:
--With the quirk *disabled*:
For sprites positioned such that some pixels partially go offscreen, the pixels are directly wrapped around to the opposite side of the screen.
So if the sprite for letter 'H' were to on the very right side of the screen, with one half inside of the screen and one half technically outside,
then one on side of the screen we would half something like '|-' and on the other we would have '-|'

--With the quirk *enabled*:
For sprites positioned such that some pixels partially go offscreen, instead of wrapping the pixels around to the other side of the screen, we simply do not render them.
So if half of a sprite is onscreen and the other is not, then we just dont render the part that is off-screen

For the displayWait quirk:
--With the quirk *disabled*:
Do not alter the DXYN flag, as we dont need to remember if we did this instruction or not
--With the quirk *enabled*:
Set flag to true so that we can detect that we did a DXYN elsewhere in the code and act accordingly (stop doing further instructions, immediately render the next screen)
*/

void Chip8::drawSprite(const uint8_t xCoord, const uint8_t yCoord, uint16_t spriteWidth, uint16_t spriteHeight, uint16_t currAddress)
{
    bool pixelWasTurnedOff{ false };
    for (std::size_t yOffset{ 0 }; yOffset < spriteHeight; ++yOffset)
    {
        uint8_t nextByte{ readMemory(currAddress) };
        ++currAddress;
        for (std::size_t xOffset{ 0 }; xOffset < spriteWidth; ++xOffset)
        {
            const uint8_t currBit{ Utility::toU8(nextByte >> (7 - xOffset) & 1) };

            uint16_t nextPixelX{ Utility::toU16((xCoord + xOffset)) };
            uint16_t nextPixelY{ Utility::toU16((yCoord + yOffset)) };

            if (m_isQuirkEnabled.wrapScreen)
            {
                nextPixelX %= m_width;
                nextPixelY %= m_height;
            }

            // Skip rendering off-screen pixels if screenwrap quirk is off
            if (!m_isQuirkEnabled.wrapScreen && (nextPixelX > m_width - 1 || nextPixelY > m_height - 1))
            {
                break;
            }

            if (currBit == 1 && m_screen[nextPixelY][nextPixelX] == 1)
                pixelWasTurnedOff = true;

            m_screen[nextPixelY][nextPixelX] ^= currBit;
        }
    }
    m_registers[0xF] = pixelWasTurnedOff;
}

void Chip8::executeOpDXYN(const uint16_t opcode)
{
    if (m_isQuirkEnabled.displayWait)
    {
        m_executedDXYNFlag = true;
    }
    const uint16_t registerX{ Utility::toU16(extractX(opcode)) };
    const uint16_t registerY{ Utility::toU16(extractY(opcode)) };

    const uint8_t xCoord{ Utility::toU8(m_registers[registerX] % InitialConfig::numPixelsHorizontally) };
    const uint8_t yCoord{ Utility::toU8(m_registers[registerY] % InitialConfig::numPixelsVertically) };

    uint16_t spriteWidth{ 8 };
    uint16_t spriteHeight{ Utility::toU16(extractN(opcode)) };

    uint16_t currAddress{ m_indexReg };

    drawSprite(xCoord, yCoord, spriteWidth, spriteHeight, currAddress);
}

void Chip8::executeOpEX9E(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t keyToCheck{ m_registers[regX] };
    const uint8_t keyToCheckSanitised{ Utility::toU8(keyToCheck & 0x0F) };

    const KeyInputs keyInput{ static_cast<KeyInputs>(keyToCheckSanitised) };
    if (m_keyDownThisFrame[keyInput])
    {
        incrementPC();
    }
}

void Chip8::executeOpEXA1(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t keyToCheck{ m_registers[regX] };
    const uint8_t keyToCheckSanitised{ Utility::toU8(keyToCheck & 0x0F) };

    const KeyInputs keyInput{ static_cast<KeyInputs>(keyToCheckSanitised) };
    if (!m_keyDownThisFrame[keyInput])
    {
        incrementPC();
    }
}

void Chip8::executeOpFX07(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_registers[regX] = m_delayTimer;
}

void Chip8::executeOpFX0A(const uint16_t opcode)
{
    if (!wasKeyReleasedThisFrame())
    {
        m_pc -= 2;
        return;
    }

    const uint16_t regX{ extractX(opcode) };
    const KeyInputs keyReleased{ findKeyReleasedThisFrame() };

    m_registers[regX] = Utility::toU8(std::to_underlying(keyReleased));
}

void Chip8::executeOpFX15(const uint16_t opcode)
{
    uint16_t regX{ extractX(opcode) };

    m_delayTimer = m_registers[regX];
}

void Chip8::executeOpFX18(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_soundTimer = m_registers[regX];
}

void Chip8::executeOpFX1E(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_indexReg += m_registers[regX];
}

void Chip8::executeOpFX29(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint16_t character{ m_registers[regX] };
    const uint16_t characterSanitised{ Utility::toU16(character & 0x000F) };

    const uint16_t spriteWidthInBytes{ 5u };
    const uint16_t spriteLocation{ Utility::toU16((characterSanitised * spriteWidthInBytes) + m_fontsLocation) };

    m_indexReg = spriteLocation;
}

void Chip8::executeOpFX33(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t hundredsDigit{ Utility::toU8((m_registers[regX] % 1000) / 100) };
    writeToMemory(m_indexReg, hundredsDigit);

    const uint8_t tensDigit{ Utility::toU8((m_registers[regX] % 100) / 10) };
    writeToMemory(m_indexReg + 1u, tensDigit);

    const uint8_t onesDigit{ Utility::toU8(m_registers[regX] % 10) };
    writeToMemory(m_indexReg + 2u, onesDigit);
}

/*
|==== QUIRK-RELATED INSTRUCTIONS ====|
The next 2 instructions (FX55, FX65) both have a quirk version that alters behaviour in the same way
--With the quirk *disabled*:
The index register is not affected during the instruction

--With the quirk *enabled*:
As we access/store information in a register, we increment the index register once for every register accessed/written to
*/
void Chip8::executeOpFX55(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    uint16_t currMemLocation{ m_indexReg };

    for (uint16_t currReg{ 0x0 }; currReg <= regX; ++currReg)
    {
        writeToMemory(currMemLocation, m_registers[currReg] );
        ++currMemLocation;

        if (m_isQuirkEnabled.index)
        {
            ++m_indexReg;
        }
    }
}

void Chip8::executeOpFX65(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    uint16_t currMemLocation{ m_indexReg };

    for (uint16_t currReg{ 0x0 }; currReg <= regX; ++currReg)
    {
        m_registers[currReg] = readMemory(currMemLocation);
        ++currMemLocation;

        if (m_isQuirkEnabled.index)
        {
            ++m_indexReg;
        }
    }
}


void Chip8::loadFonts(const uint16_t startLocation)
{
    assert(startLocation + 80 < 0x1FF);

    m_runtimeMetaData.fontStartAddress = startLocation;

    uint16_t currLocation{ startLocation };

    for (auto const fontInfo : m_fonts)
    {
        writeToMemory(currLocation, fontInfo);
        ++currLocation;
    }

    m_runtimeMetaData.fontEndAddress = currLocation - 1;
}

void Chip8::loadFile(const std::string& name)
{

    std::cout << "Loading ROM: " << name << '\n';
    std::ifstream ROM{ name, std::ios::binary };

    if (!ROM)
    {
        std::string errorMsg{ "Error opening ROM file. Path: " + name };
        std::cerr << errorMsg << '\n';
        ROM.close();
        throw FileInputException(errorMsg);
    }

    std::uint8_t nextByte{};
    std::uint16_t currAddress{ m_pc };

    m_runtimeMetaData.programStartAddress = m_pc;

    while (ROM.read(reinterpret_cast<char*>(&nextByte), sizeof(nextByte)))
    {
        writeToMemory(currAddress, nextByte);
        ++currAddress;
    }

    m_runtimeMetaData.programEndAddress = currAddress - 1;
    m_runtimeMetaData.romIsLoaded = true;

    std::cout << "Done loading\n";
    ROM.close();
}

void Chip8::setKeyDown(KeyInputs key)
{
    m_keyDownThisFrame[key] = true;
}

void Chip8::setKeyUp(KeyInputs key)
{
    m_keyDownThisFrame[key] = false;
}

Chip8::KeyInputs Chip8::findFirstPressedKey()
{
    for (const auto& [key, isKeyDown] : std::views::enumerate(m_keyDownLastFrame))
    {
        if (isKeyDown)
        {
            return static_cast<KeyInputs>(key);
        }
    }
    assert(false && "Chip8::findFirstPressedKey() called in "
                     "context where no key was pressed -> likely bug in input handling");
}

bool Chip8::isAKeyPressed()
{
    return std::ranges::any_of(m_keyDownThisFrame, [](bool isPressed) { return isPressed; });
}

void Chip8::setPrevFrameInputs()
{
    m_keyDownLastFrame = m_keyDownThisFrame;
}