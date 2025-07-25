#include <iostream>
#include <iomanip>

// SDL2 wrapper classes
#include "renderer.h"
#include "audioplayer.h"
#include "inputhandler.h"

// Convenience namespaces
#include "utility.h"
#include "settings.h"
#include "random.h"

// Chip-related classes
#include "statemanager.h"
#include "chip8.h"


// Temporary namespace to make it easier for me to swap roms 
namespace ROMS
{
	const std::string brix{ "roms/brix.ch8" };
    const std::string hidden{ "roms/Hidden [David Winter, 1996].ch8" };
    const std::string danm8ku{ "roms/danm8ku.ch8" };

    // Currently works
    const std::string ibm{ "roms/ibm.ch8" };
    const std::string testOpc{ "roms/3-corax+.ch8" };
    const std::string testFlags{ "roms/4-flags.ch8" };
    const std::string testQuirks{ "roms/5-quirks.ch8" };
    const std::string testKeypad{ "roms/6-keypad.ch8" };
    const std::string testSound{ "roms/7-beep.ch8" };
    const std::string rngTest{ "roms/rngTest.ch8" };

    const std::string OOBTest{ "roms/oob_test_7.ch8" };
    const std::string allInOneTest{ "roms/AllInOne.ch8" };

    // Works completely fine
    const std::string tetris{ "roms/tetris.ch8" };
    const std::string snake{ "roms/snake.ch8" };
    const std::string superPong{ "roms/superpong.ch8" };
    const std::string pong{ "roms/pong.ch8" };

    // Doesnt fully work - likely due to quirks
    const std::string spaceInvaders{ "roms/Space Invaders [David Winter].ch8" };
    
    // Works, but need to increase IPF by 2-3x
    const std::string fallingGhosts{ "roms/ghosts.ch8" };

    // Also works but runs slowly, not sure if that is normal
    const std::string cellAuotmata1d{ "roms/1dcell.ch8" };

    // Raises E12 error - not an issue with the emulator. Test is outdated/incorrect
    const std::string bcTest{ "roms/BC_test.ch8" };



    // Not sure if this works properly - dont know the controls and dont know where I found it
    const std::string puzzle15{ "roms/15puzzle.ch8" };
}

void executeInstructionsForFrame(Chip8& chip)
{
    for (int i = 0; i < ChipConfig::instrPerFrame; ++i)
    {
        chip.performFDECycle();

        const Chip8::QuirkFlags& isQuirkEnabled{ chip.getEnabledQuirks() };

        if (isQuirkEnabled.displayWait && chip.executedDXYN())
        {
            chip.resetDXYNFlag();
            break;
        }
    }
}

void drawDebugTextBasedOnMode(const StateManager::DebugMode mode, Renderer& renderer)
{
    const int xPos{ 10 };
    const int yPos{ 10 };

    if (mode == StateManager::DebugMode::step)
    {
        renderer.drawTextAt("STEP MODE ON", xPos, yPos);
    }
    else if (mode == StateManager::DebugMode::manual)
    {
        renderer.drawTextAt("MANUAL MODE ON", xPos, yPos);
    }
}

void updateDebugModeBasedOnInput(StateManager& stateManager, const InputHandler& inputHandler)
{
    if (stateManager.getCurrentDebugMode() != StateManager::DebugMode::step && inputHandler.isSystemKeyPressed(InputHandler::K_ACTIVATE_STEP))
    {
        stateManager.changeDebugModeTo(StateManager::DebugMode::step);
    }

    if (stateManager.getCurrentDebugMode() != StateManager::DebugMode::manual && inputHandler.isSystemKeyPressed(InputHandler::K_ACTIVATE_MANUAL))
    {
        stateManager.changeDebugModeTo(StateManager::DebugMode::manual);
    }
}

struct FrameInfo
{
    Uint32 startTimeMs{};
    Uint32 endTimeMs{};
    Uint32 timeElapsedMs{};
};


int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    constexpr Chip8::QuirkFlags baseChip8Quirks {
        true,   // reset register VF on bitwise AND/OR/XOR operation
        true,   // index register quirk
        false,  // wrap around screen quirk
        false,  // shift quirk
        false,  // jump quirk
        false,   // display wait quirk
    };

    // FOR TESTING WITH testQuirks ROM
    constexpr Chip8::QuirkFlags superChipQuirks{
        false,  // reset register VF on bitwise AND/OR/XOR operation
        false,  // index register quirk
        false,  // wrap around screen quirk
        true,   // shift quirk
        true,   // jump quirk
        false,  // display wait quirk
    };

    Renderer renderer{
        DisplayConfig::resolutionWidth,
        DisplayConfig::resolutionHeight,
        !DisplayConfig::displayGrid,
        DisplayConfig::onColour,
        DisplayConfig::offColour
    };

    Chip8 chip{ baseChip8Quirks };
    chip.loadFile(ROMS::brix);

    bool userHasQuit{ false };
    
    FrameInfo frameInfo{};
    // For a target fps of 60 this will be 16ms (rounded down because it is an int), so we will actually be rendering roughly 62-63 frames rather than 60
    const int targetFrameDelayMs{ 1000 / DisplayConfig::targetFPS };

    AudioPlayer audio{ "assets/beep.wav" };

    InputHandler inputHandler{};
    StateManager stateManager{};

    while (!userHasQuit)
    {
        frameInfo.startTimeMs = SDL_GetTicks();
        inputHandler.resetSystemKeysState();
        inputHandler.readChipAndSystemInputs(chip);

        userHasQuit = inputHandler.isSystemKeyPressed(InputHandler::K_QUIT);
        
        // State switching
        const bool activateDebugPressed{ inputHandler.isSystemKeyPressed(InputHandler::K_ACTIVATE_DEBUG) };
        const bool deactivateDebugPressed{ inputHandler.isSystemKeyPressed(InputHandler::K_DEACTIVATE_DEBUG) };
        const StateManager::State currentState{ stateManager.getCurrentState() };
        
        if (activateDebugPressed && currentState == StateManager::State::running)
        {
            stateManager.changeMainStateTo(StateManager::debug);
        }
        else if (deactivateDebugPressed && currentState == StateManager::State::debug)
        {
            stateManager.changeMainStateTo(StateManager::running);
        }

        
        chip.decrementTimers();

        if (stateManager.getCurrentState() == StateManager::State::running)
        {
            executeInstructionsForFrame(chip);
        }
        else if (stateManager.getCurrentState() == StateManager::State::debug)
        {
            // Debug mode - Allows user to pause the program and step through it instruction by instruction or frame by frame. Inputs are still processed during this, so that the 
            // User can input things while debugging. Need to hold down the buttons while stepping for that input to be processed

            FrameInfo debugFrameInfo{};

            while (stateManager.getCurrentState() == StateManager::State::debug)
            {
                inputHandler.resetSystemKeysState();
                inputHandler.readChipAndSystemInputs(chip);

                renderer.drawToScreen(chip.getScreenBuffer());
                

                const StateManager::DebugMode currentDebugMode{ stateManager.getCurrentDebugMode() };
                drawDebugTextBasedOnMode(currentDebugMode, renderer);

                renderer.render();

                updateDebugModeBasedOnInput(stateManager, inputHandler);

                // Cant use the "currentDebugMode" variable for readability here because the actual debug mode may change in these if statements, which wouldnt be reflected through the (non-reference) variable
                if (stateManager.getCurrentDebugMode() == StateManager::step && inputHandler.isSystemKeyPressed(InputHandler::K_NEXT_FRAME))
                {
                    executeInstructionsForFrame(chip);
                    break;
                }
                 
                if (stateManager.getCurrentDebugMode() == StateManager::manual)
                {
                    if (inputHandler.isSystemKeyPressed(InputHandler::K_NEXT_INSTRUCTION))
                    {
                        chip.performFDECycle();
                    }
                }

                if (inputHandler.isSystemKeyPressed(InputHandler::K_DEACTIVATE_DEBUG))
                {
                    stateManager.changeMainStateTo(StateManager::State::running);
                }
            }
        }

        if (chip.getSoundTimer() > 0)
        {
            audio.startSound();
        }
        else if (chip.getSoundTimer() == 0)
        {
            audio.stopSound();
        }

        //bool debug{ false };
        
        renderer.drawToScreen(chip.getScreenBuffer());

        renderer.render();
            
        chip.setPrevFrameInputs();

        frameInfo.endTimeMs = SDL_GetTicks();
        frameInfo.timeElapsedMs = frameInfo.endTimeMs - frameInfo.startTimeMs;

        // Frames may process faster than the target frametime, so we delay to make sure that we only move on to the next frame when enough time has passed
        if (frameInfo.timeElapsedMs < targetFrameDelayMs)
        {
            SDL_Delay(targetFrameDelayMs - frameInfo.timeElapsedMs);
        }
    }

    return 0;
}

