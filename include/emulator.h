#ifndef EMULATOR_H
#define EMULATOR_H

#include <cstdint>
#include <memory>

#include "utils/frametimer.h"
#include "types/displaysettings.h"
#include "statemanager.h"
#include "inputhandler.h"

class Renderer;
class ImguiRenderer;
struct DisplaySettings;

class Chip8;
class AudioPlayer;

class BadOpcodeException;
class FileInputException;


class Emulator
{
public:
    Emulator();
    ~Emulator();

    Emulator(const Emulator&) = delete;
    Emulator& operator=(const Emulator&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;

    void run();

private:
    void initialiseMainRenderer();
    void initialiseGUIRenderer();

    void processInputs();
    void emulateFrame();
    void updateAudioState();

    void render();
    void updateFrameTimingInfo();

    void handleEmulatorStateTransitions();
    void executeChipInstructions();
    int calculateNumInstructionsNeededForFrame();

    void handleChipExecutionError(const BadOpcodeException& exception);
    void handleFileInputError(const FileInputException& exception);

    std::unique_ptr<Chip8> m_chip{};

    std::unique_ptr<Renderer> m_renderer{};
    std::unique_ptr<ImguiRenderer> m_imguiRenderer{};

    FrameTimer m_frameTimer{ 60 };
    InputHandler m_inputHandler{};
    StateManager m_stateManager{};


    std::unique_ptr<AudioPlayer> m_audioPlayer{};
    std::shared_ptr<DisplaySettings> m_displaySettings{};

    bool m_isRunning{};
    uint64_t m_numInstrExecutedThisFrame{};

    std::string m_currentErrorMessage{ "No ROM loaded. Open menu to load ROM." };
};

#endif