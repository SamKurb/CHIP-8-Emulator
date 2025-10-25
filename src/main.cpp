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
#include "badopcodeexception.h"
#include "statemanager.h"
#include "chip8.h"

#include "imguirenderer.h"

#include "frameinfo.h"

#include "rendererinitexception.h"
#include "fileinputexception.h"

void executeInstructionsForFrame(Chip8& chip, const int targetFPS)
{
    int numInstructionsForThisFrame{ 1 };
    if (chip.getTargetNumInstrPerSecond() >= targetFPS)
    {
        numInstructionsForThisFrame = chip.getTargetNumInstrPerSecond() / targetFPS;
    }
    else if (chip.getTargetNumInstrPerSecond() <= 0)
    {
        numInstructionsForThisFrame = 0;
    }

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

    std::unique_ptr<Renderer> renderer{};
    try
    {
        renderer = std::make_unique<Renderer>(displaySettings);
    }
    catch (const RendererInitException& e)
    {
        std::cerr << "FATAL ERROR. Renderer initialisation failed in main(): " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Chip8 chip{};

    bool userHasQuit{ false };

    FrameInfo frameInfo{};
    // For a target fps of 60 this will be 16ms (rounded down because it is an int), so we will actually be rendering roughly 62-63 frames rather than 60
    const uint32_t targetFrameDelayMs{ 1000u / Utility::toU32(displaySettings -> targetFPS) };

    AudioPlayer audio{ "assets/beep.wav" };

    InputHandler inputHandler{};
    StateManager stateManager{};

    //ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float windowScaleFactor{ renderer->getDisplayScaleFactor() };
    ImguiRenderer imguiRenderer{ renderer->getWindow(), renderer->getRenderer(), displaySettings, windowScaleFactor };

    //bool runningChip{ false };

    std::string messageToDisplayIfNotRunning {"No ROM loaded. Open menu to load ROM." };
    while (!userHasQuit)
    {
        frameInfo.startTimeMs = SDL_GetTicks();
        inputHandler.resetSystemKeysState();

        if (chip.isRomLoaded())
        {
            inputHandler.readChipAndSystemInputs(chip);
        }
        else
        {
            inputHandler.readSystemInputs();
        }

        userHasQuit = inputHandler.isSystemKeyPressed(InputHandler::K_QUIT);

        uint64_t totalInstructionsExecutedBeforeFrame{ chip.getNumInstructionsExecuted() };

        if (chip.isRomLoaded())
        {
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
                try
                {
                    executeInstructionsForFrame(chip, displaySettings -> targetFPS);
                }
                catch (const BadOpcodeException& exception)
                {
                    chip = Chip8{};
                    messageToDisplayIfNotRunning = std::string(exception.what()) + " - Please try a different ROM";
                }
            }
            else if (stateManager.getCurrentState() == StateManager::State::debug)
            {
                // Debug mode - Allows user to pause the program and step through it instruction by instruction or frame by frame. Inputs are still processed during this, so that the
                // User can input things while debugging. Need to hold down the buttons while stepping for that input to be processed

                updateDebugModeBasedOnInput(stateManager, inputHandler);

                if (stateManager.getCurrentDebugMode() == StateManager::step
                    && inputHandler.isSystemKeyPressed(InputHandler::K_NEXT_FRAME))
                {
                    try
                    {
                        executeInstructionsForFrame(chip, displaySettings -> targetFPS);
                    }
                    catch (const BadOpcodeException& exception)
                    {
                        chip = Chip8{};
                        messageToDisplayIfNotRunning = std::string(exception.what()) + " - Please try a different ROM";
                    }
                }

                if (stateManager.getCurrentDebugMode() == StateManager::manual
                    && inputHandler.isSystemKeyPressed(InputHandler::K_NEXT_INSTRUCTION))
                {
                    try
                    {
                        chip.performFDECycle();
                    }
                    catch (const BadOpcodeException& exception)
                    {
                        chip = Chip8{};
                        messageToDisplayIfNotRunning = std::string(exception.what()) + " - Please try a different ROM";
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

            chip.setPrevFrameInputs();
        }

        const uint64_t totalInstructionsExecutedAfterFrame{ chip.getNumInstructionsExecuted() };

        const uint64_t numInstructionsExecutedThisFrame{
            totalInstructionsExecutedAfterFrame - totalInstructionsExecutedBeforeFrame
        };
        frameInfo.numInstructionsExecuted = numInstructionsExecutedThisFrame;


        renderer->clearDisplay();

        if (chip.isRomLoaded())
        {
            renderer->drawChipScreenBufferToFrame(chip.getScreenBuffer());
            if (stateManager.getCurrentState() == StateManager::State::debug)
            {
                const StateManager::DebugMode currentDebugMode{ stateManager.getCurrentDebugMode() };
                drawDebugTextBasedOnMode(currentDebugMode, *renderer);
            }
        }
        else
        {
            renderer->clearDisplay();
            renderer->drawTextAt(messageToDisplayIfNotRunning, 0, 0);
        }


        if (inputHandler.isSystemKeyPressed(InputHandler::K_TOGGLE_DEBUG_WINDOWS))
        {
            displaySettings -> showDebugWindows = !displaySettings -> showDebugWindows;
        }

        frameInfo.endTimeMs = SDL_GetTicks();
        frameInfo.timeElapsedMs = frameInfo.endTimeMs - frameInfo.startTimeMs;
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

        frameInfo.fps = (frameInfo.timeElapsedMs > 0) ? (1000.0f / static_cast<float>(frameInfo.timeElapsedMs)) : 0.0f;

        if (displaySettings -> showDebugWindows)
        {
            try
            {
                imguiRenderer.drawAllImguiWindows(
                    displaySettings, *renderer, imguiRenderer,
                    chip, stateManager,
                    frameInfo
                );
            }
            catch (const FileInputException& exception)
            {
                chip = Chip8();
                messageToDisplayIfNotRunning = std::string(exception.what()) + " === Please try again, or load a different ROM.";
            }
        }

        renderer->render();
    }

    return 0;
}

