#include "emulator.h"

#include "renderer.h"
#include "imguirenderer.h"
#include "../include/types/displaysettings.h"
#include "../include/utils/frametimer.h"
#include "chip8.h"
#include "audioplayer.h"
#include "inputhandler.h"
#include "statemanager.h"

#include "../include/types/frameinfo.h"

#include "../include/exceptions/sdlinitexception.h"
#include "../include/exceptions/fileinputexception.h"
#include "../include/exceptions/badopcodeexception.h"
#include "exceptions/chipstackerrorexception.h"

Emulator::Emulator()
: m_chip{ std::make_unique<Chip8>() }
, m_stateManager{}
, m_displaySettings{ std::make_shared<DisplaySettings>() }
, m_isRunning{ true }
, m_numInstrExecutedThisFrame{ 0 }
, m_currentErrorMessage{ "No ROM loaded. Open menu to load ROM." }
{
    initialiseMainRenderer();
    initialiseGUIRenderer();

    m_inputHandler = InputHandler();
    m_audioPlayer = std::make_unique<AudioPlayer>("assets/beep.wav");

    m_frameTimer = FrameTimer(m_displaySettings->targetFPS);
}

Emulator::~Emulator() = default;

void Emulator::initialiseMainRenderer()
{
    m_renderer = std::make_unique<Renderer>(m_displaySettings);
}

void Emulator::initialiseGUIRenderer()
{
    const float windowScaleFactor{ m_renderer->getDisplayScaleFactor() };
    m_imguiRenderer = std::make_unique<ImguiRenderer>(
        m_renderer->getWindow(),
        m_renderer->getRenderer(),
        m_displaySettings,
        windowScaleFactor
    );
}

void Emulator::processInputs()
{
    m_inputHandler.resetSystemKeysState();

    if (m_chip->isRomLoaded())
    {
        m_inputHandler.readChipAndSystemInputs(*m_chip);
    }
    else
    {
        m_inputHandler.readSystemInputs();
    }

    if (m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_QUIT))
    {
        m_isRunning = false;
    }

    if (m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_TOGGLE_DEBUG_WINDOWS))
    {
        m_displaySettings->showDebugWindows = !(m_displaySettings->showDebugWindows);
    }
}

void Emulator::handleEmulatorStateTransitions()
{
    const bool activateDebugPressed{ m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_ACTIVATE_DEBUG) };
    if (activateDebugPressed)
    {
        m_stateManager.tryTransitionTo(StateManager::debug);
    }

    const bool deactivateDebugPressed{ m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_DEACTIVATE_DEBUG) };
    if (deactivateDebugPressed)
    {
        m_stateManager.tryTransitionTo(StateManager::running);
    }

    const bool activateStepPressed{ m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_ACTIVATE_STEP) };
    if (activateStepPressed)
    {
        m_stateManager.tryTransitionTo(StateManager::step);
    }

    const bool activateManualPressed{ m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_ACTIVATE_MANUAL) };
    if (activateManualPressed)
    {
        m_stateManager.tryTransitionTo(StateManager::manual);
    }
}

void Emulator::emulateFrame()
{
    handleEmulatorStateTransitions();

    m_chip->decrementTimers();

    executeChipInstructions();

    m_chip->setPrevFrameInputs();
}

int Emulator::calculateNumInstructionsNeededForFrame()
{
    int targetNumInstrPerSecond{ m_chip->getTargetNumInstrPerSecond() };
    if (targetNumInstrPerSecond <= 0)
    {
        return 0;
    }
    if (targetNumInstrPerSecond < m_displaySettings->targetFPS)
    {
        return 1;
    }
    return targetNumInstrPerSecond / m_displaySettings->targetFPS;
}

void Emulator::handleOpcodeExecutionError(const std::runtime_error& exception)
{
    m_chip = std::make_unique<Chip8>();
    m_currentErrorMessage = std::string(exception.what()) + " - Please fix any bugs present in the ROM or try a different ROM! "
                                                            + "Make sure it is CHIP-8 compatible";
}

void Emulator::handleFileInputError(const FileInputException &exception)
{
    m_chip = std::make_unique<Chip8>();
    m_currentErrorMessage = std::string(exception.what()) + " Failed to load file. Please ensure it is not being"
                                                            + "used by any other processes.";
}


void Emulator::updateAudioState()
{
    if (!(m_audioPlayer->isAudioLoaded()))
    {
        return;
    }

    if (m_chip->getSoundTimer() > 0)
    {
        m_audioPlayer->startSound();
    }
    else if (m_chip->getSoundTimer() == 0)
    {
        m_audioPlayer->stopSound();
    }
}

void Emulator::executeChipInstructions()
{
    try
    {
        if (m_stateManager.getCurrentState() == StateManager::State::running)
        {
            int numInstructionsToExecute { calculateNumInstructionsNeededForFrame() };
            m_chip->executeInstructions(numInstructionsToExecute);
        }
        else if (m_stateManager.getCurrentState() == StateManager::State::debug)
        {
            // Debug mode - Allows user to pause the program and step through it instruction by instruction or frame by frame. Inputs are still processed during this, so that the
            // User can input things while debugging. Need to hold down the buttons while stepping for that input to be processed

            if (m_stateManager.getCurrentDebugMode() != StateManager::DebugMode::step
                && m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_ACTIVATE_STEP))
            {
                m_stateManager.tryTransitionTo(StateManager::DebugMode::step);
            }

            if (m_stateManager.getCurrentDebugMode() != StateManager::DebugMode::manual
                && m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_ACTIVATE_MANUAL))
            {
                m_stateManager.tryTransitionTo(StateManager::DebugMode::manual);
            }

            if (m_stateManager.getCurrentDebugMode() == StateManager::step
                && m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_NEXT_FRAME))
            {
                int numInstructionsToExecute{ calculateNumInstructionsNeededForFrame() };
                m_chip->executeInstructions(numInstructionsToExecute);
            }

            if (m_stateManager.getCurrentDebugMode() == StateManager::manual
                && m_inputHandler.isSystemKeyPressed(InputHandler::SystemKeyInputs::K_NEXT_INSTRUCTION))
            {
                m_chip->performFDECycle();
            }
        }
    }
    catch (const BadOpcodeException& exception)
    {
        handleOpcodeExecutionError(exception);
    }
    catch (const ChipStackErrorException& exception)
    {
        handleOpcodeExecutionError(exception);
    }
    catch (const ChipOOBMemoryAccessException& exception)
    {
        handleOpcodeExecutionError(exception);
    }
}

void Emulator::render()
{
    m_renderer->clearDisplay();

    if (m_chip->isRomLoaded())
    {
        m_renderer->drawChipScreenBufferToFrame(m_chip->getScreenBuffer());

        if (m_stateManager.getCurrentState() == StateManager::debug)
        {
            constexpr int xPos{ 10 };
            constexpr int yPos{ 10 };

            StateManager::DebugMode debugMode{ m_stateManager.getCurrentDebugMode() };
            if (debugMode == StateManager::DebugMode::step)
            {
                m_renderer->drawTextAt("STEP MODE ON", xPos, yPos);
            }
            else if (debugMode == StateManager::DebugMode::manual)
            {
                m_renderer->drawTextAt("MANUAL MODE ON", xPos, yPos);
            }
        }
    }
    else
    {
        m_renderer->drawTextAt(m_currentErrorMessage, 0, 0);
    }

    if (m_displaySettings->showDebugWindows)
    {
        try
        {
            FrameInfo frameInfo { m_frameTimer.getFrameInfo() };
            frameInfo.numInstructionsExecuted = m_numInstrExecutedThisFrame;

            int targetFPSBeforeUserInput{ m_displaySettings->targetFPS };

            m_imguiRenderer->drawAllImguiWindows(
                m_displaySettings,
                *m_renderer,
                *m_chip,
                m_stateManager,
                frameInfo,
                m_audioPlayer->isAudioLoaded()
            );

            int targetFPSAfterUserInput{ m_displaySettings->targetFPS };

            bool targetFPSChanged{ targetFPSAfterUserInput != targetFPSBeforeUserInput };
            if (targetFPSChanged)
            {
                m_frameTimer.setTargetFPS(targetFPSAfterUserInput);
            }
        }
        catch (const FileInputException& exception)
        {
            handleFileInputError(exception);
        }
    }

    m_renderer->render();
}

void Emulator::updateFrameTimingInfo()
{
    m_frameTimer.endFrameTiming();
    m_frameTimer.delayToReachTargetFrameTime();
}

void Emulator::run()
{
    while (m_isRunning)
    {
        m_frameTimer.startFrameTiming();

        const uint64_t totalInstrExecutedBeforeFrame{ m_chip->getRuntimeMetaData().numInstructionsExecuted };

        processInputs();

        if (m_chip->isRomLoaded())
        {
            emulateFrame();
            updateAudioState();
        }

        render();
        updateFrameTimingInfo();

        const uint64_t totalInstrExecutedAfterframe{ m_chip->getRuntimeMetaData().numInstructionsExecuted };
        m_numInstrExecutedThisFrame = totalInstrExecutedAfterframe - totalInstrExecutedBeforeFrame;
    }
}
