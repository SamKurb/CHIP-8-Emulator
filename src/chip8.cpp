#include "CHIP8.h"

void printOpcode(uint16_t opcode)
{
    std::cout << "Opcode: " << std::hex << opcode;
}

uint16_t Chip8::fetchOpcode()
{
    const uint16_t firstOpHalf{ m_memory[m_pc++] };
    const uint16_t secondOpHalf{ m_memory[m_pc++] };

    const uint16_t opcode{ Utility::toU16((firstOpHalf << 8) | secondOpHalf) };
    return opcode;
}

void Chip8::decodeAndExecute(uint16_t opcode)
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
            std::cout << "UNKOWN OPCODE!!!n";
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
            std::cout << "INVALID OPCODE!!!!!";
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
            std::cout << "INVALID OPCODE!!!!!";
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
            std::cout << "INVALID OPCODE!!!!!";
            break;
        }
        break;

    default:
        std::cout << "INVALID OPCODE!\n";
        std::cout << std::hex << opcode;
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

    const uint16_t address{ m_stack.back() };

    m_pc = m_stack.back();
    m_stack.pop_back();

    std::cout << "Opcode: " << std::hex << 0x0EEE;
    std::cout << "Returning from subroutine, setting PC to address " << address << '\n';
    std::cout << "PC is now set to address " << m_pc << '\n';
}


void Chip8::op1NNN(const uint16_t opcode)
{

    const uint16_t address{ extractNNN(opcode) };
    m_pc = address;

    printOpcode(opcode);
    std::cout << " Setting PC to address " << address << '\n';
    std::cout << "PC is now set to address " << m_pc << '\n';
}

void Chip8::op2NNN(const uint16_t opcode)
{

    printOpcode(opcode);

    const uint16_t address{ extractNNN(opcode) };
    std::cout << " Pushing current address in PC (" << m_pc << ") to stack, and setting PC to address: " << address << "\n";

    m_stack.push_back(m_pc);
    m_pc = address;

    std::cout << "PC now pointing to address " << m_pc << '\n';
    std::cout << "Address at the top of the stack: " << m_stack.back() << ", Stack size: " << m_stack.size() << '\n';
}

void Chip8::op3XNN(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };

    std::cout << " Comparing value at register V" << regNum << " (" << static_cast<int>(m_registers[regNum]) << ") with " << static_cast<int>(valueToCompare) << '\n';

    if (m_registers[regNum] == valueToCompare)
    {
        m_pc += 2;
        std::cout << "Equal, incremented PC by 2\n";
    }
    else
    {
        std::cout << "Not equal, did not incremenet PC \n";
    }
}

void Chip8::op4XNN(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToCompare{ Utility::toU8(extractNN(opcode)) };

    std::cout << " Comparing value at register V" << regNum << " (" << static_cast<int>(m_registers[regNum])
        << ") with " << valueToCompare << '\n';

    if (m_registers[regNum] != valueToCompare)
    {
        m_pc += 2;
        std::cout << "Not equal, incremented PC by 2\n";
    }
    else
    {
        std::cout << "Equal, did not incremenet PC \n";
    }
}

void Chip8::op5XY0(const uint16_t opcode)
{
    printOpcode(opcode);

    const  uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    std::cout << " Comparing value at register V" << regX << " (" << static_cast<int>(m_registers[regX])
        << ") with value at register" << regY << " (" << static_cast<int>(m_registers[regY]) << ")\n";

    if (m_registers[regX] == m_registers[regY])
    {
        m_pc += 2;
        std::cout << "Equal, incremented PC by 2\n";
    }
    else
    {
        std::cout << "Not equal, did not incremenet PC \n";
    }
}

void Chip8::op6XNN(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToPut{ Utility::toU8(extractNN(opcode)) };
    m_registers[regNum] = valueToPut;

    std::cout << " Setting value " << static_cast<int>(valueToPut) << " to register " << regNum << '\n';
    std::cout << "V" << regNum << " is now = " << static_cast<int>(m_registers[regNum]) << '\n';
}

void Chip8::op7XNN(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regNum{ extractX(opcode) };
    const uint8_t valueToAdd{ Utility::toU8(extractNN(opcode)) };

    std::cout << " Adding value " << static_cast<int>(valueToAdd) << " to register " << regNum << '\n';

    std::cout << static_cast<int>(m_registers[regNum]) << " + " << static_cast<int>(valueToAdd) << " = ";
    m_registers[regNum] += valueToAdd;
    std::cout << static_cast<int>(m_registers[regNum]) << '\n';
}

void Chip8::op8XY0(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    std::cout << " Storing value at register V" << regY << " in register V" << regX << "\n";

    m_registers[regX] = m_registers[regY];

    std::cout << "Register V" << regX << " now contains: " << static_cast<int>(m_registers[regX]) << "\n";
}

void Chip8::op8XY1(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] |= m_registers[regY];

    if (m_enabledQuirks.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY2(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] &= m_registers[regY];

    if (m_enabledQuirks.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY3(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    m_registers[regX] ^= m_registers[regY];

    if (m_enabledQuirks.resetVF)
    {
        m_registers[0xF] = 0;
    }
}

void Chip8::op8XY4(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint16_t sum{ Utility::toU16(m_registers[regX] + m_registers[regY]) };

    m_registers[regX] = Utility::toU8(sum & 0xFF);

    m_registers[0xF] = sum > 255;
}

void Chip8::op8XY5(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint8_t subtractionResult{ Utility::toU8(m_registers[regX] - m_registers[regY]) };

    const bool noBorrow{ m_registers[regX] >= m_registers[regY] };

    m_registers[regX] = subtractionResult;

    m_registers[0xF] = noBorrow;
}

void Chip8::op8XY6(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };

    if (!m_enabledQuirks.shift)
    {
        const uint16_t regY{ extractY(opcode) };
        m_registers[regX] = m_registers[regY];
    }

    // is LSB == 1?
    bool bitShiftedOut{ (m_registers[regX] & 0x01) == 1 };

    m_registers[regX] >>= 1;
    m_registers[0xF] = bitShiftedOut;
}

void Chip8::op8XY7(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    const uint8_t subtractionResult{ Utility::toU8(m_registers[regY] - m_registers[regX]) };

    const bool noBorrow{ m_registers[regY] >= m_registers[regX] };

    m_registers[regX] = subtractionResult;

    m_registers[0xF] = noBorrow;
}

void Chip8::op8XYE(const uint16_t opcode)
{
    printOpcode(opcode);

    // Register X is the destination, Y the source
    const uint16_t regX{ extractX(opcode) };

    if (!m_enabledQuirks.shift)
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
    printOpcode(opcode);

    const uint16_t regX{ extractX(opcode) };
    const uint16_t regY{ extractY(opcode) };

    if (m_registers[regX] != m_registers[regY])
    {
        m_pc += 2;
    }
}

void Chip8::opANNN(const uint16_t opcode)
{
    printOpcode(opcode);

    const uint16_t addressToSet{ extractNNN(opcode)};
    std::cout << addressToSet << '\n';

    std::cout << " Setting index register to address " << addressToSet << '\n';

    m_indexReg = addressToSet;

    std::cout << "Address is now set to " << m_indexReg << '\n';
}

void Chip8::opBNNN(const uint16_t opcode)
{
    printOpcode(opcode);

	if (!m_enabledQuirks.jump)
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

void Chip8::opDXYN(const uint16_t opcode)
{
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
        uint8_t nextByte{ m_memory[currAddress++] };
        for (std::size_t j{ 0 }; j < spriteWidth; ++j)
        {
            const uint8_t currBit{ Utility::toU8(nextByte >> (7 - j) & 1) };

            uint16_t nextPixelX{ Utility::toU16((xCoord + j)) };
            uint16_t nextPixelY{ Utility::toU16((yCoord + i)) };

            if (m_enabledQuirks.wrapScreen)
            {
                nextPixelX %= m_width;
                nextPixelY %= m_height;
            }

            // Skip rendering off-screen pixels if screenwrap quirk is off
            if (!m_enabledQuirks.wrapScreen && (nextPixelX > m_width - 1 || nextPixelY > m_height - 1))
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
        m_pc += 2;
    }
}

void Chip8::opEXA1(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    const uint8_t keyToCheck{ m_registers[regX] };

    if (!m_keyDownThisFrame[keyToCheck])
    {
        m_pc += 2;
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

    std::cout << "\n\n\n KEY DETECTED: " << std::hex << static_cast<int>(key) << "\n\n\n";
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
    m_memory[Utility::toUZ(m_indexReg)] = hundredsDigit;

    const uint8_t tensDigit{ Utility::toU8((m_registers[regX] % 100) / 10) };
    m_memory[Utility::toUZ(m_indexReg + 1)] = tensDigit;

    const uint8_t onesDigit{ Utility::toU8(m_registers[regX] % 10) };
    m_memory[Utility::toUZ(m_indexReg + 2)] = onesDigit;
}

void Chip8::opFX55(const uint16_t opcode)
{
    const uint16_t regX{ extractX(opcode) };

    uint16_t currMemLocation{ m_indexReg };

    for (uint16_t currReg{ 0x0 }; currReg <= regX; ++currReg)
    {
        m_memory[currMemLocation] = m_registers[currReg];
        ++currMemLocation;

        if (m_enabledQuirks.index)
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
        m_registers[currReg] = m_memory[currMemLocation];
        ++currMemLocation;
        if (m_enabledQuirks.index)
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
        m_memory[currLocation++] = fontInfo;
    }
}

void Chip8::loadFile(const std::string name)
{
    std::cout << "Loading ROM: " << name << '\n';
    std::ifstream ROM{ name, std::ios::binary };

    if (!ROM)
    {
        std::cout << "Could not open ROM file";
    }

    std::uint8_t nextByte{};

    std::uint16_t currAddress{ m_pc };
    while (ROM.read(reinterpret_cast<char*>(&nextByte), sizeof(nextByte)))
    {
        m_memory[currAddress++] = nextByte;
        //print2DigitHex(nextByte);
    }

    std::cout << "Done loading\n";
}

// Prints contents of screen buffer. W for on pixels, whitespace for off. For debugging
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
