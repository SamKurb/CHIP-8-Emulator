#include "CHIP8.h"

void handleInvalidOpcode(const uint16_t opcode)
{
    std::cout << "Invalid opcode!\n"
        << "Opcode: " << opcode << "\n." << "Exiting program.";
    std::exit(1);
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
        op1NNN(opcode);
        break;
    case 0x2000:
        op2NNN(opcode);
        break;
    case 0x3000:
        op3XNN(opcode);
        break;
    case 0x4000:
        op4XNN(opcode);
        break;
    case 0x5000:
        op5XY0(opcode);
        break;
    case 0x6000:
        op6XNN(opcode);
        break;
    case 0x7000:
        op7XNN(opcode);
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            op8XY0(opcode);
            break;
        case 0x0001:
            op8XY1(opcode);
            break;
        case 0x0002:
            op8XY2(opcode);
            break;
        case 0x0003:
            op8XY3(opcode);
            break;
        case 0x0004:
            op8XY4(opcode);
            break;
        case 0x0005:
            op8XY5(opcode);
            break;
        case 0x0006:
            op8XY6(opcode);
            break;
        case 0x0007:
            op8XY7(opcode);
            break;
        case 0x000E:
            op8XYE(opcode);
            break;
        default:
            handleInvalidOpcode(opcode);
            break;
        }
        break;

    case 0x9000:
        op9XY0(opcode);
        break;
    case 0xA000:
        opANNN(opcode);
        break;
    case 0xB000:
        opBNNN(opcode);
        break;
    case 0xC000:
        opCXNN(opcode);
        break;
    case 0xD000:
        opDXYN(opcode);
        break;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            opEX9E(opcode);
            break;
        case 0x00A1:
            opEXA1(opcode);
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
            opFX07(opcode);
            break;
        case 0x000A:
            opFX0A(opcode);
            break;
        case 0x0015:
            opFX15(opcode);
            break;
        case 0x0018:
            opFX18(opcode);
            break;
        case 0x001E:
            opFX1E(opcode);
            break;
        case 0x0029:
            opFX29(opcode);
            break;
        case 0x0033:
            opFX33(opcode);
            break;
        case 0x0055:
            opFX55(opcode);
            break;
        case 0x0065:
            opFX65(opcode);
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
            op00E0();
            break;
        case 0x00EE:
            op00EE();
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

/*
    All opcodes in the order they are mentioned in:
    The wikipedia page: https://en.wikipedia.org/wiki/CHIP-8
    CG's reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
*/
void Chip8::op00E0()
{
    for (std::size_t y{ 0 }; y < m_height; ++y)
    {
        for (std::size_t x{ 0 }; x < m_width; ++x)
        {
            m_screen[y][x] = 0;
        }
    }
}

void Chip8::op00EE()
{
    assert(m_stack.size() > 0 && "Attempted to pop from empty stack in opcode 00EE");

    m_pc = m_stack.back();
    m_stack.pop_back();
}


void Chip8::op1NNN(const uint16_t opcode)
{

    const uint16_t address{ extractNNN(opcode) };
    m_pc = address;
}

void Chip8::op2NNN(const uint16_t opcode)
{
    const uint16_t address{ extractNNN(opcode) };

    if (std::size(m_stack) >= ChipConfig::levelsOfNesting)
    {
        std::cout << "Stack limit reached, cannot push to stack. Increase ChipConfig::levelsOfNesting or ensure that the program/ROM is not buggy";
        std::exit(1);
    }

    m_stack.push_back(m_pc);
    m_pc = address;
}

void Chip8::op3XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };


    if (m_registers[regNum] == valueToCompare)
    {
        incrementPC();
    }
}

void Chip8::op4XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };

    if (m_registers[regNum] != valueToCompare)
    {
        incrementPC();
    }
}

void Chip8::op5XY0(const uint16_t opcode)
{
    const  uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    if (m_registers[regX] == m_registers[regY])
    {
        incrementPC();;
    }
}

void Chip8::op6XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToPut{ Utility::toU8(extractNN(opcode)) };
    m_registers[regNum] = valueToPut;
}

void Chip8::op7XNN(const uint16_t opcode)
{
    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToAdd{ Utility::toU8(extractNN(opcode)) };

    m_registers[regNum] += valueToAdd;
}

void Chip8::op8XY0(const uint16_t opcode)
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
Register VF is left untouched the operation

--With the quirk *enabled*:
Register VF is always set to 0 at the end of the instruction
*/
void Chip8::op8XY1(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] |= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY2(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] &= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY3(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] ^= m_registers[regY];

    if (m_isQuirkEnabled.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY4(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint16_t sum{ Utility::toU16(m_registers[regX] + m_registers[regY]) };

    m_registers[regX] = Utility::toU8(sum & 0xFF);

    m_registers[0xF] = sum > 255;
}

void Chip8::op8XY5(const uint16_t opcode)
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
void Chip8::op8XY6(const uint16_t opcode)
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

void Chip8::op8XY7(const uint16_t opcode)
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

void Chip8::op8XYE(const uint16_t opcode)
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

void Chip8::op9XY0(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    if (m_registers[regX] != m_registers[regY])
    {
        incrementPC();
    }
}

void Chip8::opANNN(const uint16_t opcode)
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
void Chip8::opBNNN(const uint16_t opcode)
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

void Chip8::opCXNN(const uint16_t opcode)
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
void Chip8::opDXYN(const uint16_t opcode)
{
    if (m_isQuirkEnabled.displayWait)
    {
        m_executedDXYNFlag = true;
    }
    const uint16_t registerX{ Utility::toU16(extractX(opcode)) };
    const uint16_t registerY{ Utility::toU16(extractY(opcode)) };

    const uint8_t xCoord{ Utility::toU8(m_registers[registerX] % ChipConfig::screenWidth) };
    const uint8_t yCoord{ Utility::toU8(m_registers[registerY] % ChipConfig::screenHeight) };

    uint16_t spriteWidth{ 8 };
    uint16_t spriteHeight{ Utility::toU16(extractN(opcode)) };

    uint16_t currAddress{ m_indexReg };


    bool pixelTurnedOff{ false };

    for (std::size_t i{ 0 }; i < spriteHeight; ++i)
    {
        uint8_t nextByte{ readMemory(currAddress) };
        ++currAddress;
        for (std::size_t j{ 0 }; j < spriteWidth; ++j)
        {
            const uint8_t currBit{ Utility::toU8(nextByte >> (7 - j) & 1) };

            uint16_t nextPixelX{ Utility::toU16((xCoord + j)) };
            uint16_t nextPixelY{ Utility::toU16((yCoord + i)) };

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
                pixelTurnedOff = true;

            m_screen[nextPixelY][nextPixelX] ^= currBit;
        }
    }

    m_registers[0xF] = pixelTurnedOff;
}

void Chip8::opEX9E(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t keyToCheck{ m_registers[regX] };

    if (m_keyDownThisFrame[keyToCheck])
    {
        incrementPC();
    }
}

void Chip8::opEXA1(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t keyToCheck{ m_registers[regX] };

    if (!m_keyDownThisFrame[keyToCheck])
    {
        incrementPC();
    }
}

void Chip8::opFX07(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_registers[regX] = m_delayTimer;
}

void Chip8::opFX0A(const uint16_t opcode)
{
    if (!wasKeyReleasedThisFrame())
    {
        m_pc -= 2;
        return;
    }

    const uint16_t regX{ extractX(opcode) };
    const uint8_t key{ findKeyReleasedThisFrame() };

    assert(key != 0x10);

    m_registers[regX] = key;
}

void Chip8::opFX15(const uint16_t opcode)
{
    uint16_t regX{ extractX(opcode) };

    m_delayTimer = m_registers[regX];
}

void Chip8::opFX18(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_soundTimer = m_registers[regX];
}

void Chip8::opFX1E(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    m_indexReg += m_registers[regX];
}

void Chip8::opFX29(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };
    const uint16_t character{ m_registers[regX] };
    const uint16_t spriteLocation{ Utility::toU16((character * 5) + m_fontsLocation) };

    m_indexReg = spriteLocation;
}

void Chip8::opFX33(const uint16_t opcode)
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
void Chip8::opFX55(const uint16_t opcode)
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

void Chip8::opFX65(const uint16_t opcode)
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

    uint16_t currLocation{ startLocation };

    for (auto const fontInfo : m_fonts)
    {
        writeToMemory(currLocation, fontInfo);
        ++currLocation;
    }
}

void Chip8::loadFile(const std::string name)
{
    std::cout << "Loading ROM: " << name << '\n';
    std::ifstream ROM{ name, std::ios::binary };

    if (!ROM)
    {
        std::cout << "Could not open ROM file!\n";
    }

    std::uint8_t nextByte{};
    std::uint16_t currAddress{ m_pc };

    while (ROM.read(reinterpret_cast<char*>(&nextByte), sizeof(nextByte)))
    {
        writeToMemory(currAddress, nextByte);
        ++currAddress;
    }

    std::cout << "Done loading\n";
}

// Prints contents of screen buffer For debugging. W for on pixels, whitespace for off. 
void Chip8::printScreenBuffer()
{
    for (auto row : m_screen)
    {
        for (auto element : row)
        {
            if (element == 1)
                std::cout << " W";
            else
                std::cout << "  ";
        }
        std::cout << '\n';
    }
}

void Chip8::setKeyDown(std::size_t key)
{
    m_keyDownThisFrame[key] = true;
}

void Chip8::setKeyUp(std::size_t key)
{
    m_keyDownThisFrame[key] = false;
}

bool Chip8::isAKeyPressed()
{
    for (const bool isPressed : m_keyDownThisFrame)
    {
        if (isPressed)
        {
            return true;
        }
    }
    return false;
}

void Chip8::setPrevFrameInputs()
{
    m_keyDownLastFrame = m_keyDownThisFrame;
}

uint8_t Chip8::findFirstPressedKey()
{
    for (uint8_t key{ 0x0 }; key <= 0xF; ++key)
    {
        if (m_keyDownThisFrame[key])
        {
            return key;
        }
    }
    return 0x10;
}
