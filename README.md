# WORK IN PROGRESS: Will add building instructions soon

# Overview
This is an emulator/interpreter for the CHIP-8. It is written in C++, and uses the SDL2 library for both graphics, input handling and audio output

## Key features:
- Passes all relevant modern day tests (In particular, Timendus' tests: https://github.com/Timendus/chip8-test-suite)
- Quirks are configurable. Supports all 6 main quirks (more detail below to be added)
- Input works, with the input mapping being as follows: <br>
  <pre>
   CHIP-8 Keypad   Keyboard   
     +-+-+-+-+    +-+-+-+-+  
     |1|2|3|C|    |1|2|3|4|  
     +-+-+-+-+    +-+-+-+-+  
     |4|5|6|D|    |Q|W|E|R|  
     +-+-+-+-+ => +-+-+-+-+  
     |7|8|9|E|    |A|S|D|F| 
     +-+-+-+-+    +-+-+-+-+ 
     |A|0|B|F|    |Z|X|C|V|  
     +-+-+-+-+    +-+-+-+-+   
  </pre>
  Note: the input mapping can be changed by editing the source code in chip8.h, However I plan to make it configurable via editing a JSON file
- Audio works. By default, uses a 440Hz sine wave as the audio to be outputted (beep.wav in assets folder). Can be changed by swapping the beep.wav audio file with another of the same name
- Runs all games I have tested (some require quirks to be configured differently)
- Can change the colour scheme of the display
- Has an integrated debugging mode. Can pause emulation and step through the program frame by frame or instruction by instruction. Current controls for this are: <br>
Activate debug mode: 6 <br>
Enter step mode (step through frame by frame): 7 <br>
Step frame: space bar <br>
Enter manual mode (step through instruction by instruction): 8 <br>
Step instruction: I <br>

Note: Manual mode is a work in progress. I will add functionality to manually decrement timers, output a frame etc  <br>

Will add more features and make key mapping configurable in the future!
## Other notes:
- There is *alot* of static casting going on in the code, because I have narrowing conversions disabled on my compiler. Most arithmetic and logic operations promote to int, so doing something like Unsigned & Unsigned will create a (signed) int temporary and require the result to be casted back to unsigned

## To-do:
- Improve user friendliness (add an easier way to swap ROMS and alter settings rather than directly changing source code)
- Maybe add a GUI to see memory contents etc
- Add more features to debug mode
- Refactor some codee

## References used for gathering the CHIP-8 specification (these are the main ones):
- https://en.wikipedia.org/wiki/CHIP-8 For general information on how everything is structured, as well as opcode specifications
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM - For opcode specifications
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/ - For information regarding quirks/ambiguous instructions as well as typical keyboard-to-input mappings

- Random.h file from [https://www.learncpp.com/](https://www.learncpp.com/cpp-tutorial/global-random-numbers-random-h/) (highly recommend this resource! its great)
