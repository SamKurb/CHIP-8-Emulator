// SDL2 wrapper classes
#include "renderer.h"
#include "audioplayer.h"
#include "inputhandler.h"

// Display Settings struct
#include "displaysettings.h"

// Convenience namespaces
#include "utility.h"
#include "random.h"

// Chip-related classes
#include "statemanager.h"
#include "chip8.h"

// IMGUI
#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"

#include "imguirenderer.h"

#include "frameinfo.h"

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

void executeInstructionsForFrame(Chip8& chip, const int targetFPS)
{
    const int numInstructionsForThisFrame{ chip.getTargetNumInstrPerSecond() / targetFPS };
    for (int i = 0; i < numInstructionsForThisFrame; ++i)
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
    constexpr int xPos{ 10 };
    constexpr int yPos{ 10 };

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

int main([[maybe_unused]] int argc,[[maybe_unused]] char* args[])
{
    std::shared_ptr<DisplaySettings> displaySettings{ std::make_unique<DisplaySettings>() };

    Renderer renderer{
        displaySettings
    };

    Chip8 chip{};
    chip.loadFile(ROMS::pong);

    bool userHasQuit{ false };
    
    FrameInfo frameInfo{};
    // For a target fps of 60 this will be 16ms (rounded down because it is an int), so we will actually be rendering roughly 62-63 frames rather than 60
    const int targetFrameDelayMs{ 1000 / displaySettings -> targetFPS };

    AudioPlayer audio{ "assets/beep.wav" };

    InputHandler inputHandler{};
    StateManager stateManager{};

    //ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float windowScaleFactor{ renderer.getDisplayScaleFactor() };
    ImguiRenderer imguiRenderer{ renderer.getWindow(), renderer.getRenderer(), displaySettings, windowScaleFactor };

    while (!userHasQuit)
    {
		uint64_t totalInstructionsExecutedBeforeFrame{ chip.getNumInstructionsExecuted() };

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
            executeInstructionsForFrame(chip, displaySettings -> targetFPS);
        }
        else if (stateManager.getCurrentState() == StateManager::State::debug)
        {
            // Debug mode - Allows user to pause the program and step through it instruction by instruction or frame by frame. Inputs are still processed during this, so that the 
            // User can input things while debugging. Need to hold down the buttons while stepping for that input to be processed

            updateDebugModeBasedOnInput(stateManager, inputHandler);

            if (stateManager.getCurrentDebugMode() == StateManager::step && inputHandler.isSystemKeyPressed(InputHandler::K_NEXT_FRAME))
            {
                executeInstructionsForFrame(chip, displaySettings -> targetFPS);
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

        if (chip.getSoundTimer() > 0)
        {
            audio.startSound();
        }
        else if (chip.getSoundTimer() == 0)
        {
            audio.stopSound();
        }

        //bool debug{ false };

        chip.setPrevFrameInputs();

        frameInfo.endTimeMs = SDL_GetTicks();
        frameInfo.timeElapsedMs = frameInfo.endTimeMs - frameInfo.startTimeMs;
        const uint64_t totalInstructionsExecutedAfterFrame{ chip.getNumInstructionsExecuted() };

        const uint64_t numInstructionsExecutedThisFrame{
            totalInstructionsExecutedAfterFrame - totalInstructionsExecutedBeforeFrame
        };

        frameInfo.numInstructionsExecuted = numInstructionsExecutedThisFrame;
        /*
        Frames may process faster than the target frametime, so we delay to make
        sure that we only move on to the next frame when enough time has passed
        */
        if (frameInfo.timeElapsedMs < targetFrameDelayMs)
        {
			Uint32 timeToWaitMs{ targetFrameDelayMs - frameInfo.timeElapsedMs };
            SDL_Delay(timeToWaitMs);

            frameInfo.timeElapsedMs += timeToWaitMs;
        }

        frameInfo.fps = (frameInfo.timeElapsedMs > 0) ? (1000.0f / frameInfo.timeElapsedMs) : 0.0f;

        renderer.clearDisplay();
        renderer.drawChipScreenBufferToFrame(chip.getScreenBuffer());

        if (stateManager.getCurrentState() == StateManager::State::debug)
        {
            const StateManager::DebugMode currentDebugMode{ stateManager.getCurrentDebugMode() };
            drawDebugTextBasedOnMode(currentDebugMode, renderer);
        }

        if (inputHandler.isSystemKeyPressed(InputHandler::K_TOGGLE_DEBUG_WINDOWS))
        {
            displaySettings -> showDebugWindows = !displaySettings -> showDebugWindows;
        }

        if (displaySettings -> showDebugWindows)
        {
            imguiRenderer.drawAllImguiWindows(
                displaySettings, renderer, imguiRenderer,
                chip, stateManager,
                frameInfo
            );
        }

        renderer.render();


    }

    return 0;
}

