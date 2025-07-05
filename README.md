# CHIP-8 Emulator/interpreter written in C++, using SDL2 for graphics

## Key notes:
- Passes main tests
- Keyboard input works. Mapping can be changed
- Able to run tetris, snake and other games

## Other notes:
- There is *alot* of static casting going on in the code, because I have narrowing conversions disabled on my compiler. Most arithmetic and logic operations promote to int, so doing something like Unsigned & Unsigned will create a (signed) int temporary and require the result to be casted back to unsigned

## To-do:
- Improve user friendliness (add an easier way to swap ROMS and alter settings rather than directly changing source code)
- Clean up debug output. Add optional flag to disable it alltogether
- Add configurable quirk settings (choose which quirks to enable/disable, so that different games can be played)
- Add configurable colours to display
- Maybe add a GUI to see memory contents etc
- Iron out some opcodes. Currently the emualator doesnt check if stack size is exceeded. This doesnt really matter for 99% of roms but would be good to add

## References used:
https://en.wikipedia.org/wiki/CHIP-8 For general information on how everything is structured, as well as opcode specifications
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM - For opcode specifications
https://tobiasvl.github.io/blog/write-a-chip-8-emulator/ - For information regarding quirks/ambiguous instructions as well as typical keyboard-to-input mappings
